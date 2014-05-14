#ifndef SIRIUS_INDEXER_HPP
#define SIRIUS_INDEXER_HPP

#include "Tokenizer.hpp"
#include "globals.hpp"

namespace Sirius
{
    typedef unordered_map<TokenidType, set<DocidType> > InvertedIndexType;
    typedef InvertedIndexType::mapped_type InvertedIndexValueType;
}

namespace Sirius
{

    class Indexer: public InitOnOff
    {
        private:
            vector<DocInfo> _docInfoRows;

        private:
            InvertedIndexType _titleInvertedIndex;
            InvertedIndexType _contentInvertedIndex;

        private:
            Tokenizer& _tokenizer;
        public:
            void tokenize(const string& text, vector<TokenidType>& res) const
            {
                _tokenizer.tokenize(text, res);
            }

        public:
            Indexer(Tokenizer& tokenizer)
                : _tokenizer(tokenizer)
            {}
            ~Indexer(){}
        public:
            bool build(const string& filePath)
            {
                _wrapDocGeneralInfos(filePath, _docInfoRows);
                _buildDocForwardIndexInfos(_docInfoRows);

                _buildInvertedIndex(_docInfoRows, _titleInvertedIndex, GetTitleTokensFunct());
                _buildInvertedIndex(_docInfoRows, _contentInvertedIndex, GetContentTokensFunct());

                return true;
            }

            struct GetTitleTokensFunct
            {
                const vector<TokenidType>& operator() (const DocInfo& docInfo)
                {
                    return docInfo.titleTokens;
                }
            };
            struct GetContentTokensFunct
            {
                const vector<TokenidType>& operator() (const DocInfo& docInfo)
                {
                    return docInfo.contentTokens;
                }
            };


            template <class Function>
                void _buildInvertedIndex(const vector<DocInfo>& docInfos, InvertedIndexType& iindex, Function fn) const
                {
                    for(size_t docid = 0; docid < docInfos.size(); docid ++)
                    {
                        const vector<TokenidType>& tokens = fn(docInfos[docid]);
                        for(size_t ti = 0; ti < tokens.size(); ti++)
                        {
                            iindex[tokens[ti]].insert(docid);
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
            void _buildDocForwardIndexInfos(vector<DocInfo>& docInfos)
            {
                for(size_t docid = 0; docid < docInfos.size(); docid++)
                {
                    _tokenizer.tokenize(docInfos[docid].title, docInfos[docid].titleTokens);
                    _tokenizer.tokenize(docInfos[docid].content, docInfos[docid].contentTokens);
                }
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



    };
}


#endif
