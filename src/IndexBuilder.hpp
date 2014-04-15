#ifndef SIRIUS_INDEX_BUILDER_HPP
#define SIRIUS_INDEX_BUILDER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"
#include <list>

namespace Sirius
{
    using namespace CppJieba;

    enum {LINE_COLLUMN_N = 2};

    class IndexBuilder: public InitOnOff
    {
        private:
            MixSegment _segment;
            unordered_set<string> _stopWords;
            typedef uint32_t TokenidType;
            typedef unordered_map<string, TokenidType> WordTokenidMapType;
        private:
            TokenidType _tokenidAutoIncr;
            WordTokenidMapType _wordTokenidIndex;

            typedef uint32_t DocidType;
            typedef uint64_t FileOffsetType;
            struct DocmetaType
            {
                FileOffsetType offset;
                uint32_t length;
            };
        private:
            vector<DocmetaType> _docmetaRows;

        private:
            typedef unordered_map<TokenidType, set<DocidType> > InvertedIndexType;
            InvertedIndexType _titleInvertedIndex;

        public:
            IndexBuilder(const string& dictPath, const string& modelPath): _segment(dictPath, modelPath), _tokenidAutoIncr(0)
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
                string line;
                FileOffsetType offsetCursor = 0;
                size_t lineno = 1;
                string title, content;
                DocidType docid;
                while(_getDocInfoAndUpdateIndex(ifs, offsetCursor, lineno, docid, title, content))
                {
                    buildTitleIndex(title, docid);
                    buildContentIndex(content);
                }
                return true;
            }
        public:
            bool query(const string& title, string& rawText) const
            {
                vector<TokenidType> tokenids;
                _tokenize(title, tokenids);
                InvertedIndexType::const_iterator citer ;
                typedef InvertedIndexType::value_type InvertedIndexValueType;

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

                copy(docCountMap.begin(), docCountMap.end(), inserter(docCounts, docCounts.end()));

                return true;
            }
        public:

            bool buildTitleIndex(const string& title, const DocidType& docid)
            {
                vector<TokenidType> tokenids;
                _tokenizeAndUpdateIndex(title, tokenids);
                _buildInvertedIndex(docid, tokenids, _titleInvertedIndex);
                return true;
            }

            bool buildContentIndex(const string& content)
            {
                return true;
            }


        private:
            void _buildInvertedIndex(const DocidType & docid, const vector<TokenidType>& tokenids, InvertedIndexType& invertedIndex) const
            {
                for(size_t i = 0 ; i < tokenids.size(); i++)
                {
                    invertedIndex[tokenids[i]].insert(docid);
                }
            }
        private:
            ifstream& _getDocInfoAndUpdateIndex(ifstream& ifs, FileOffsetType& offset, size_t& lineno, DocidType& docid, string& title, string& content)
            {
                string line;
                vector<string> buf;
                DocmetaType docmeta;
                for(;getline(ifs, line); lineno ++)
                {
                    docmeta.offset = offset;
                    docmeta.length = line.size();

                    offset += docmeta.length + 1;

                    if(!split(line, buf, "\t") || buf.size() != LINE_COLLUMN_N)
                    {
                        LogError("line[%u:%s] illegal.", lineno, line.c_str());
                        continue;
                    }

                    _docmetaRows.push_back(docmeta);
                    docid = _docmetaRows.size() - 1;

                    title = buf[0];
                    content = buf[1];

                    break;
                }
                return ifs;
            }

            void _tokenize(const string& text, vector<TokenidType>& tokenids) const
            {
                vector<string> words;
                _segment.cut(text, words);
                WordTokenidMapType::const_iterator citer;
                for(size_t i = 0; i < words.size(); i ++)
                {
                    const string& word = words[i];
                    if(_wordTokenidIndex.end() != (citer = _wordTokenidIndex.find(word)))
                    {
                        tokenids.push_back(citer->second);
                    }
                }
            }

            void _tokenizeAndUpdateIndex(const string& text, vector<TokenidType>& tokenids)
            {
                vector<string> words;
                _segment.cut(text, words);
                WordTokenidMapType::const_iterator citer ;
                for(size_t i = 0; i < words.size(); i++)
                {
                    const string& word = words[i];
                    if(!isIn(_stopWords, word))
                    {
                        if(_wordTokenidIndex.end() == (citer = _wordTokenidIndex.find(word)))
                        {
                            _wordTokenidIndex[word] = _tokenidAutoIncr ++;
                        }
                        tokenids.push_back(_wordTokenidIndex[word]);
                    }
                }
            }

        public:
            bool dumpWordTokenidMap(const string& filePath) const
            {
                FILE * fout = fopen(filePath.c_str(), "w");
                if(!fout)
                {
                    LogFatal("open [%s] failed.", filePath.c_str());
                    return false;
                }
                size_t len;
                TokenidType tokenid;
                for(WordTokenidMapType::const_iterator citer = _wordTokenidIndex.begin(); citer != _wordTokenidIndex.end(); citer++)
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

    };
}


#endif
