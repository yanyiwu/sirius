#ifndef SIRIUS_INDEX_BUILDER_HPP
#define SIRIUS_INDEX_BUILDER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"

namespace Sirius
{
    using namespace CppJieba;

    enum {LINE_COLLUMN_N = 2};

    class IndexBuilder: public InitOnOff
    {
        private:
            MixSegment _segment;
            unordered_set<string> _stopWords;
        public:
            typedef uint32_t TokenidType;
            typedef unordered_map<string, TokenidType> WordTokenidMapType;
        private:
            TokenidType _tokenidAutoIncr;
            WordTokenidMapType _wordTokenidIndex;

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
                vector<string> buf;
                vector<string> words;
                for(size_t lineno = 1; getline(ifs, line); lineno++)
                {
                    if(!split(line, buf, "\t") || buf.size() != LINE_COLLUMN_N)
                    {
                        LogError("line[%u:%s] illegal.", lineno, line.c_str());
                        continue;
                    }

                    const string& title = buf[0];
                    const string& content = buf[1];

                    buildTitleIndex(title);
                    buildContentIndex(content);
                }
                return true;
            }

            bool buildTitleIndex(const string& title)
            {
                vector<TokenidType> tokenids;
                _tokenizeAndUpdateIndex(title, tokenids);
                return true;
            }

            bool buildContentIndex(const string& content)
            {
                return true;
            }

        private:
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
