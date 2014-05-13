#ifndef SIRIUS_INDEX_BUILDER_HPP
#define SIRIUS_INDEX_BUILDER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"
#include <list>

namespace Sirius
{
    using namespace CppJieba;
    class IndexBuilder: public InitOnOff
    {
        private:
            MixSegment _segment;
            unordered_set<string> _stopWords;
        private:
            typedef unordered_map<string, TokenidType> WordMapType;
            WordMapType _wordMap;
            

        private:
            vector<DocInfo> _docInfoRows;

        private:
            typedef unordered_map<TokenidType, set<DocidType> > InvertedIndexType;
            typedef InvertedIndexType::mapped_type InvertedIndexValueType;
            InvertedIndexType _titleInvertedIndex;
            InvertedIndexType _contentInvertedIndex;

        public:
            IndexBuilder(const string& dictPath, const string& modelPath, const string& stopWordPath): _segment(dictPath, modelPath)
            {
                assert(_segment);
                _loadStopWords(stopWordPath);
                _setInitFlag(_segment);
            }
            ~IndexBuilder(){}
        public:
            bool build(const string& filePath)
            {
                _wrapDocGeneralInfos(filePath, _docInfoRows);
                _buildDocForwardIndexInfos(_docInfoRows, _wordMap);

                _buildTitleInvertedIndex(_docInfoRows, _titleInvertedIndex);
                _buildContentInvertedIndex(_docInfoRows, _contentInvertedIndex);

                return true;
            }
        private:
            void _query(const string& text, const InvertedIndexType& index, const size_t topN, vector<DocidType>& docIds) const
            {
                vector<TokenidType> tokenids;
                _tokenize(text, tokenids);
                _searchTopN(index, tokenids, topN, docIds);
            }
        public:
            void query(const RequestData& req, ResponseData& res)
            {
            }
        public:
            void queryTitle(const string& title, vector<DocidType>& docIds) const
            {
                _query(title, _titleInvertedIndex, TITLE_TOP_N, docIds);
            }

            void queryContent(const string& content, vector<DocidType>& docIds) const
            {
                _query(content, _contentInvertedIndex, CONTENT_TOP_N, docIds);
            }

        private:
            void _buildTitleInvertedIndex(const vector<DocInfo>& docInfos, InvertedIndexType& titleIIndex) const
            {
                for(size_t docid = 0; docid < docInfos.size(); docid ++)
                {
                    const vector<TokenidType>& tokens = docInfos[docid].titleTokens;
                    for(size_t ti = 0; ti < tokens.size(); ti++)
                    {
                        titleIIndex[tokens[ti]].insert(docid);
                    }
                }
            }
            void _buildContentInvertedIndex(const vector<DocInfo>& docInfos, InvertedIndexType& contentIIndex) const
            {
                for(size_t docid = 0; docid < docInfos.size(); docid ++)
                {
                    const vector<TokenidType>& tokens = docInfos[docid].contentTokens;
                    for(size_t ti = 0; ti < tokens.size(); ti++)
                    {
                        contentIIndex[tokens[ti]].insert(docid);
                    }
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
            void _buildDocForwardIndexInfos(vector<DocInfo>& docInfos, WordMapType& wordMap) const
            {
                vector<string> words;
                for(size_t docid = 0; docid < docInfos.size(); docid++)
                {
                    _segment.cut(docInfos[docid].title, words);
                    _updateWordMap(words, wordMap);
                    _tokenize(words, docInfos[docid].titleTokens);
                    _segment.cut(docInfos[docid].content, words);
                    _updateWordMap(words, wordMap);
                    _tokenize(words, docInfos[docid].contentTokens);
                }
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
            void _wrapDocGeneralInfos(const string& filePath, vector<DocInfo>& docInfos) const
            {
                ifstream ifs(filePath.c_str());
                assert(ifs);
                string line;
                vector<string> buf;
                DocInfo docInfo;
                size_t offset = 0;
                for(size_t lineno = 0; getline(ifs, line); lineno ++)
                {

                    docInfo.offset = offset;
                    docInfo.length = line.size();

                    offset += docInfo.length + 1;

                    if(!split(line, buf, "\t") || buf.size() != LINE_COLLUMN_N)
                    {
                        LogWarn("line[%u:%s] illegal.", lineno, line.c_str());
                        continue;
                    }

                    docInfo.id = atoi(buf[0].c_str());
                    assert(docInfo.id);
                    docInfo.title = buf[1];
                    docInfo.content = buf[2];
                    docInfos.push_back(docInfo);
                }
            }

            void _tokenize(const vector<string>& words, vector<TokenidType>& tokenids) const
            {
                WordMapType::const_iterator citer;
                for(size_t i = 0; i < words.size(); i ++)
                {
                    if(!isIn(_stopWords, words[i]) && _wordMap.end() != (citer = _wordMap.find(words[i])))
                    {
                        tokenids.push_back(citer->second);
                    }
                }
            }

            void _tokenize(const string& text, vector<TokenidType>& tokenids) const
            {
                vector<string> words;
                _segment.cut(text, words);
                _tokenize(words, tokenids);
            }

            void _updateWordMap(const vector<string>& words, WordMapType& mp) const
            {
                WordMapType::const_iterator citer ;
                size_t size;
                for(size_t i = 0; i < words.size(); i++)
                {
                    const string& word = words[i];
                    if(!isIn(_stopWords, word))
                    {
                        if(mp.end() == (citer = mp.find(word)))
                        {
                            size = mp.size();
                            mp[word] = size;
                        }
                    }
                }
            }

        public:
            bool dumpWordMap(const string& filePath) const
            {
                FILE * fout = fopen(filePath.c_str(), "w");
                if(!fout)
                {
                    LogFatal("open [%s] failed.", filePath.c_str());
                    return false;
                }
                size_t len;
                TokenidType tokenid;
                for(WordMapType::const_iterator citer = _wordMap.begin(); citer != _wordMap.end(); citer++)
                {
                    len = citer->first.size();
                    tokenid = citer->second;
                    fwrite(&len, sizeof(len), 1, fout);
                    fwrite(citer->first.c_str(), len, 1, fout);
                    fwrite(&tokenid, sizeof(len), 1, fout);
                }
                fclose(fout);
                return true;
            }
        private:
            void _loadStopWords(const string& filePath)
            {
                assert(_stopWords.empty());
                ifstream ifs(filePath.c_str());
                assert(ifs);
                string word;
                while(getline(ifs, word))
                {
                    _stopWords.insert(word);
                }
                assert(_stopWords.size());

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
