#ifndef SIRIUS_INDEX_BUILDER_HPP
#define SIRIUS_INDEX_BUILDER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"
#include <list>

namespace Sirius
{
    using namespace CppJieba;

    const size_t LINE_COLLUMN_N = 2;
    const size_t TOP_N = 5;

    class IndexBuilder: public InitOnOff
    {
        private:
            MixSegment _segment;
            unordered_set<string> _stopWords;
            typedef uint32_t TokenidType;
            typedef unordered_map<string, TokenidType> WordMapType;
        private:
            WordMapType _wordMap;

            typedef uint32_t DocidType;
            typedef uint64_t FileOffsetType;
            struct DocmetaType
            {
                FileOffsetType offset;
                uint32_t length;
            };
        private:
            struct DocGeneralInfo
            {
                DocmetaType meta;
                string title;
                string content;
            };
            struct DocForwardIndexInfo
            {
                vector<TokenidType>  titleTokens;
                vector<TokenidType> contentTokens;
            };
            struct DocInfo
            {
                DocGeneralInfo general;
                DocForwardIndexInfo index;
            };
            vector<DocInfo> _docInfoRows;

        private:
            typedef unordered_map<TokenidType, set<DocidType> > InvertedIndexType;
            InvertedIndexType _titleInvertedIndex;
            InvertedIndexType _contentInvertedIndex;

        public:
            IndexBuilder(const string& dictPath, const string& modelPath): _segment(dictPath, modelPath)
            {
                assert(_segment);
                _setInitFlag(_segment);
            }
            ~IndexBuilder(){}
        public:
            bool build(const string& filePath)
            {
                ifstream ifs(filePath.c_str());
                assert(ifs);
                _wrapDocGeneralInfos(ifs, _docInfoRows);
                _buildDocForwardIndexInfos(_docInfoRows, _wordMap);

                _buildTitleInvertedIndex(_docInfoRows, _titleInvertedIndex);
                _buildContentInvertedIndex(_docInfoRows, _contentInvertedIndex);
                
                return true;
            }
        public:
            bool query(const string& title, string& rawText) const
            {
                vector<TokenidType> tokenids;
                _tokenize(title, tokenids);
                InvertedIndexType::const_iterator citer ;
                typedef InvertedIndexType::mapped_type InvertedIndexValueType;

                map<DocidType, size_t> docCountMap;

                for(size_t i = 0; i < tokenids.size(); i ++)
                {
                    if(_titleInvertedIndex.end() !=( citer = _titleInvertedIndex.find(tokenids[i])))
                    {
                        const InvertedIndexValueType & value = citer->second;
                        for(InvertedIndexValueType::const_iterator viter = value.begin(); viter != value.end(); viter++)
                        {
                            docCountMap[*viter] ++;
                        }
                    }
                }

                vector<pair<DocidType, size_t> > docCounts;

                copy(docCountMap.begin(), docCountMap.end(), inserter(docCounts, docCounts.begin()));
                size_t topN = (TOP_N < docCounts.size() ? TOP_N : docCounts.size());
                partial_sort(docCounts.begin(), docCounts.begin() + topN, docCounts.end(), _greater_pair_second<pair<DocidType, size_t> >());
                docCounts.resize(topN);

                size_t docid;
                for(size_t i = 0; i  < docCounts.size(); i ++)
                {
                    docid = docCounts[i].first;
                    cout << _calculateSimilarityRate(tokenids, _docInfoRows[docid].index.titleTokens) << endl;
                    print(_docInfoRows[docid].general.title);
                }
                return true;
            }

        private:
            void _buildTitleInvertedIndex(const vector<DocInfo>& docInfos, InvertedIndexType& titleIIndex) const
            {
                for(size_t docid = 0; docid < docInfos.size(); docid ++)
                {
                    const vector<TokenidType>& tokens = docInfos[docid].index.titleTokens;
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
                    const vector<TokenidType>& tokens = docInfos[docid].index.contentTokens;
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
                    _segment.cut(docInfos[docid].general.title, words);
                    _updateWordMap(words, wordMap);
                    _tokenize(words, docInfos[docid].index.titleTokens);
                    _segment.cut(docInfos[docid].general.content, words);
                    _updateWordMap(words, wordMap);
                    _tokenize(words, docInfos[docid].index.contentTokens);
                }
            }
            void _wrapDocGeneralInfos(ifstream& ifs, vector<DocInfo>& docInfos) const
            {
                assert(ifs);
                string line;
                vector<string> buf;
                DocmetaType docmeta;
                DocInfo docInfo;
                FileOffsetType offset = 0;
                for(size_t lineno = 0; getline(ifs, line); lineno ++)
                {
                    docmeta.offset = offset;

                    docInfo.general.meta.offset = offset;
                    docInfo.general.meta.length = line.size();

                    offset += docmeta.length + 1;

                    if(!split(line, buf, "\t") || buf.size() != LINE_COLLUMN_N)
                    {
                        LogError("line[%u:%s] illegal.", lineno, line.c_str());
                        continue;
                    }
                    

                    docInfo.general.title = buf[0];
                    docInfo.general.content = buf[1];
                    docInfos.push_back(docInfo);
                }
            }

            void _tokenize(const vector<string>& words, vector<TokenidType>& tokenids) const
            {
                WordMapType::const_iterator citer;
                for(size_t i = 0; i < words.size(); i ++)
                {
                    const string& word = words[i];
                    if(_wordMap.end() != (citer = _wordMap.find(word)))
                    {
                        tokenids.push_back(citer->second);
                    }
                }
            }

            void _tokenize(const string& text, vector<TokenidType>& tokenids) const
            {
                vector<string> words;
                _segment.cut(text, words);
                WordMapType::const_iterator citer;
                for(size_t i = 0; i < words.size(); i ++)
                {
                    const string& word = words[i];
                    if(_wordMap.end() != (citer = _wordMap.find(word)))
                    {
                        tokenids.push_back(citer->second);
                    }
                }
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
            template <class T>
                struct _greater_pair_second: public binary_function<T, T, T>
                {
                    bool operator()(const T& lhs, const T& rhs) const
                    {
                        return lhs.second > rhs.second;
                    }
                };
    };
}


#endif
