#ifndef SIRIUS_TOKENIZER_HPP
#define SIRIUS_TOKENIZER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"

namespace Sirius
{
    using namespace CppJieba;
    class Tokenizer
    {
        public:
            Tokenizer(const string& dictPath, const string& modelPath, const string& stopWordPath)
                : _segment(dictPath, modelPath)
            {
                assert(_segment);
                _loadStopWords(stopWordPath);
            }
            ~Tokenizer()
            {}
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
            MixSegment _segment;
        private:
            unordered_set<string> _stopWords;
        private:
            typedef unordered_map<string, TokenidType> WordMapType;
            WordMapType _wordMap;
        public:
            void tokenize(const vector<string>& words, vector<TokenidType>& tokenids) const
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

            void tokenize(const string& text, vector<TokenidType>& tokenids) const
            {
                vector<string> words;
                _segment.cut(text, words);
                tokenize(words, tokenids);
            }

            void tokenize(const string& text, vector<TokenidType>& tokenids)
            {
                vector<string> words;
                _segment.cut(text, words);
                updateWordMap(words);
                tokenize(words, tokenids);
            }
        private:

            void updateWordMap(const vector<string>& words)
            {
                WordMapType::const_iterator citer ;
                for(size_t i = 0; i < words.size(); i++)
                {
                    const string& word = words[i];
                    if(!isIn(_stopWords, word))
                    {
                        if(_wordMap.end() == (citer = _wordMap.find(word)))
                        {
                            _wordMap[word] = _wordMap.size();
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
    };
}

#endif
