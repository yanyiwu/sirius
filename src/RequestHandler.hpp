#ifndef SIRIUS_REQUESTHANDLER_HPP
#define SIRIUS_REQUESTHANDLER_HPP

#include "Husky/EpollServer.hpp"
#include "IndexBuilder.hpp"

namespace Sirius
{
    using namespace Husky;

    class ReqHandler: public IRequestHandler
    {
        public:
            ReqHandler(IndexBuilder & index): _index(index)
            {};
            virtual ~ReqHandler(){};
        public:
            virtual bool do_GET(const HttpReqInfo& httpReq, string& strSnd) const
            {
                return false;
            }
            virtual bool do_POST(const HttpReqInfo& httpReq, string& strSnd) const
            {
                return false;
            }
        private:
            IndexBuilder & _index;
    };
}

#endif
