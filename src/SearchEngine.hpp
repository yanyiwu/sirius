#ifndef SIRIUS_SEARCH_ENGINE_HPP
#define SIRIUS_SEARCH_ENGINE_HPP

#include "IndexBuilder.hpp"

namespace Sirius
{
    class SearchEngine
    {
        public:
            SearchEngine(const IndexBuilder& index)
                : _index(index)
            {
                assert(_index);
            }
            ~SearchEngine(){}
        private:
            const IndexBuilder& _index;
        public:
            void query(const RequestData& req, ResponseData& res) const
            {
                vector<DocidType> docids;
                vector<TokenidType> tokenids;
                _index.tokenize(req.title, )
            }
    };
}

#endif
