#ifndef SIRIUS_REQUESTHANDLER_HPP
#define SIRIUS_REQUESTHANDLER_HPP

#include "Husky/EpollServer.hpp"
#include "IndexBuilder.hpp"

namespace Sirius
{
    using namespace Husky;

    class RequestHandler: public IRequestHandler
    {
        public:
            RequestHandler(IndexBuilder & index): _index(index)
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
                size_t id = atoi(lines[0].c_str());
                if(!id)
                {
                    LogError("id[%s] illegal.", lines[0].c_str());
                    return false;
                }
                const string& title = lines[1];
                //const string& content = lines[2];
                vector<size_t> docIds;
                _index.queryTitle(title, docIds);
                strSnd << docIds;
                LogInfo("do_POST result [%s]", strSnd.c_str());
                return true;
            }
        private:
            IndexBuilder & _index;
    };
}

#endif
