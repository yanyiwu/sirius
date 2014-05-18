#ifndef SIRIUS_REQUESTHANDLER_HPP
#define SIRIUS_REQUESTHANDLER_HPP

#include "Husky/EpollServer.hpp"
#include "SearchEngine.hpp"

namespace Sirius
{
    using namespace Husky;

    class RequestHandler: public IRequestHandler
    {
        private:
            const SearchEngine& _searcher;
        public:
            RequestHandler(const SearchEngine& searcher)
                : _searcher(searcher)
            {};
            virtual ~RequestHandler(){};
        public:
            virtual bool do_GET(const HttpReqInfo& httpReq, string& strSnd) const
            {
                return false;
            }
            virtual bool do_POST(const HttpReqInfo& httpReq, string& strSnd) const
            {
                const string& body = httpReq.getBody();
                vector<string> lines;
                if(!split(body, lines, "\t") || lines.size() != POST_COLLUMN_N)
                {
                    LogError("post body illegal [%s]", body.c_str());
                    return false;
                }
                RequestData reqdata;
                ResponseData resdata;

                reqdata.id = atoi(lines[0].c_str());
                if(!reqdata.id)
                {
                    LogError("id[%s] illegal.", lines[0].c_str());
                    return false;
                }
                reqdata.title = lines[1];
                reqdata.content = lines[2];

                _searcher.search(reqdata, resdata);
                _toJson(resdata, strSnd);
                LogInfo("do_POST result [%s]", strSnd.c_str());
                return true;
            }
        private:
            void _toJson(const ResponseData& resdata, string& json) const
            {
                if(resdata.empty())
                {
                    json = "[]";
                    return;
                }
                string tmp;
                json = "[";
                tmp << resdata[0];
                json += tmp;
                for(size_t i = 1; i < resdata.size(); i++)
                {
                    tmp << resdata[i];
                    json += ',';
                    json += tmp;
                }
                json += ']';
            }
    };
}

#endif
