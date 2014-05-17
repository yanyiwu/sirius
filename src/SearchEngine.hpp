#ifndef SIRIUS_SEARCH_ENGINE_HPP
#define SIRIUS_SEARCH_ENGINE_HPP

#include "Indexer.hpp"

namespace Sirius
{
    const size_t TITLE_TOP_N = 10;
    const size_t CONTENT_TOP_N = 10;

    static const double TITLE_WEIGHT_FACTOR = 0.7;
    static const double CONTENT_WEIGHT_FACTOR = 0.3;

    class SearchEngine
    {
        private:
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
                vector<TokenidType> titleTokens, contentTokens;
                set<DocidType> docidSet;
                vector<DocidType> docids;
                ResponseDataUnit unit;
                const DocInfo* ptDocInfo;
                DocidType docid;

                _index.tokenize(req.title, titleTokens);
                _index.tokenize(req.content, contentTokens);

                _searchFromTitle(titleTokens, TITLE_TOP_N, docids);

                for(size_t i = 0; i < docids.size() ; i++)
                {
                    docid = docids[i];
                    if(docidSet.insert(docid).second)
                    {
                        ptDocInfo = _index.find(docid);
                        assert(docid == ptDocInfo->id);
                        unit.id = docid;
                        unit.title = ptDocInfo->title;
                        unit.similar = TITLE_WEIGHT_FACTOR * _calculateSimilarityRate(ptDocInfo->titleTokens, titleTokens) + CONTENT_WEIGHT_FACTOR * _calculateSimilarityRate(ptDocInfo->contentTokens, contentTokens);
                        res.push_back(unit);
                    }
                }
                
                docids.clear();
                _searchFromContent(contentTokens, CONTENT_TOP_N, docids);

                for(size_t i = 0; i < docids.size() ; i++)
                {
                    docid = docids[i];
                    if(docidSet.insert(docid).second)
                    {
                        ptDocInfo = _index.find(docid);
                        assert(docid == ptDocInfo->id);
                        unit.id = docid;
                        unit.title = ptDocInfo->title;
                        unit.similar = _calculateSimilarityRate(ptDocInfo->titleTokens, titleTokens) + _calculateSimilarityRate(ptDocInfo->contentTokens, contentTokens);
                        res.push_back(unit);
                    }
                }

                sort(res.begin(), res.end());
                if(res.size() > RESPONSE_TOP_N)
                {
                    res.resize(RESPONSE_TOP_N);
                }
            }
        private:
            void _searchFromTitle(const vector<TokenidType>& tokenids, const size_t topN, vector<DocidType>& docIds) const
            {
                return _searchTopN(_index.getTitleIndex(), tokenids, topN, docIds);
            }
            void _searchFromContent(const vector<TokenidType>& tokenids, const size_t topN, vector<DocidType>& docIds) const
            {
                return _searchTopN(_index.getContentIndex(), tokenids, topN, docIds);
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
