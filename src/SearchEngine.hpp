#ifndef SIRIUS_SEARCH_ENGINE_HPP
#define SIRIUS_SEARCH_ENGINE_HPP

#include "Indexer.hpp"

namespace Sirius
{
    class SearchEngine
    {
        public:
            SearchEngine(const Indexer& index)
                : _index(index)
            {
                assert(_index);
            }
            ~SearchEngine(){}
        private:
            const Indexer& _index;
        public:
            void query(const RequestData& req, ResponseData& res) const
            {
                vector<DocidType> docids;
                vector<TokenidType> tokenids;
                _index.tokenize(req.title, )
            }
        private:
            void _query(const string& text, const InvertedIndexType& index, const size_t topN, vector<DocidType>& docIds) const
            {
                vector<TokenidType> tokenids;
                _tokenizer.tokenize(text, tokenids);
                _searchTopN(index, tokenids, topN, docIds);
            }
            const InvertedIndexValueType* _search(const InvertedIndexType& index, const InvertedIndexType::key_type& key) const
            {
                const InvertedIndexType::const_iterator iter = index.find(key);
                if(index.end() == iter)
                {
                    return NULL;
                }
                return &iter->second;
            }
        private:

            template<class keyT, class valueT>
                void _sortTopN(const map<keyT, valueT>& mp, vector<pair<keyT, valueT> >& tops, const size_t topN) const
                {
                    tops.clear();
                    copy(mp.begin(), mp.end(), inserter(tops, tops.begin()));
                    size_t n = min(topN, tops.size());
                    partial_sort(tops.begin(), tops.begin() + n, tops.end(), _greater_pair_second<pair<keyT, valueT> >());
                    tops.resize(n);
                }

        private:
            template <class T>
                struct _greater_pair_second: public binary_function<T, T, T>
            {
                bool operator()(const T& lhs, const T& rhs) const
                {
                    return lhs.second > rhs.second;
                }
            };
        private:
            void _searchTopN(const InvertedIndexType& index, const vector<TokenidType>& tokenids, const size_t topN, vector<DocidType>& docs) const 
            {
                map<DocidType, size_t> docCountMap;

                for(size_t i = 0; i < tokenids.size(); i ++)
                {
                    const InvertedIndexValueType* ptr = _search(index, tokenids[i]);
                    if(ptr)
                    {
                        for(InvertedIndexValueType::const_iterator viter = ptr->begin(); viter != ptr->end(); viter++)
                        {
                            docCountMap[*viter] ++;
                        }
                    }
                }

                vector<pair<DocidType, size_t> > docCounts;
                _sortTopN(docCountMap, docCounts, topN);

                for(size_t i = 0; i  < docCounts.size(); i ++)
                {
                    //docid = docCounts[i].first;
                    //print(_calculateSimilarityRate(tokenids, _docInfoRows[docid].index.titleTokens));
                    //print(_docInfoRows[docid].general.title);
                    docs.push_back(docCounts[i].first);
                }
            }
    };
}

#endif
