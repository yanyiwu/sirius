#ifndef SIRIUS_SEARCH_ENGINE_HPP
#define SIRIUS_SEARCH_ENGINE_HPP

#include "Indexer.hpp"

namespace Sirius
{
    const size_t TITLE_TOP_N = 10;
    const size_t CONTENT_TOP_N = 10;

    class SearchEngine
    {
        public:
            SearchEngine(const Indexer& index)
                : _index(index)
            {}
            ~SearchEngine(){}
        private:
            const Indexer& _index;
        public:
            void search(const RequestData& req, ResponseData& res) const
            {
                
                set<DocidType> docidSet;
                _searchAndMerge(req, docidSet);
            }
        private:
            void _searchAndMerge(const RequestData& req, set<DocidType>& res) const
            {
                vector<DocidType> docids;
                _searchFromTitle(req.title, TITLE_TOP_N, docids);
                for(size_t i = 0; i < docids.size(); i++)
                {
                    res.insert(docids[i]);
                }
                
                docids.clear();
                _searchFromContent(req.content, CONTENT_TOP_N, docids);
                for(size_t i = 0; i < docids.size(); i++)
                {
                    res.insert(docids[i]);
                }
            }
        private:
            void _searchFromTitle(const string& text, const size_t topN, vector<DocidType>& docIds) const
            {
                return _searchTopN(text, _index.getTitleIndex(), topN, docIds);
            }
            void _searchFromContent(const string& text, const size_t topN, vector<DocidType>& docIds) const
            {
                return _searchTopN(text, _index.getContentIndex(), topN, docIds);
            }
            void _searchTopN(const string& text, const InvertedIndexType& index, const size_t topN, vector<DocidType>& docIds) const
            {
                vector<TokenidType> tokenids;
                _index.tokenize(text, tokenids);
                _searchTopN(index, tokenids, topN, docIds);
            }
            void _searchTopN(const InvertedIndexType& index, const vector<TokenidType>& tokenids, const size_t topN, vector<DocidType>& docs) const 
            {
                map<DocidType, size_t> docCountMap;

                for(size_t i = 0; i < tokenids.size(); i ++)
                {
                    const InvertedIndexValueType* ptr = _index.find(index, tokenids[i]);
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
            double _calculateSimilarityRate(const vector<TokenidType>& lhs, const vector<TokenidType>& rhs) const
            {
                size_t commonCnt = 0;
                for(size_t i = 0; i < lhs.size(); i++)
                {
                    if(rhs.end() != find(rhs.begin(), rhs.end(), lhs[i]))
                    {
                        commonCnt ++;
                    }
                }
                return 2.0 * commonCnt / (lhs.size() + rhs.size());
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
                struct _greater_pair_second
                : public binary_function<T, T, T>
                {
                    bool operator()(const T& lhs, const T& rhs) const
                    {
                        return lhs.second > rhs.second;
                    }
                };
    };
}

#endif
