#ifndef SIRIUS_INDEX_BUILDER_HPP
#define SIRIUS_INDEX_BUILDER_HPP

#include "globals.hpp"
#include "CppJieba/MixSegment.hpp"

namespace Sirius
{
    using namespace CppJieba;
    class IndexBuilder
    {
        private:
            MixSegment _segment;
            unordered_set<string> _stopWords;
        public:
            IndexBuilder(const string& dictPath, const string& modelPath): _segment(dictPath, modelPath){}
            ~IndexBuilder(){}
        public:
            void build()
            {

            }
            template <class ContainType>
                void _filerStopWords(ContainType& words) const
                {
                    for(typename ContainType::iterator iter = words.begin(); iter != words.end(); )
                    {
                        if(isIn(_stopWords, *iter))
                        {
                            iter = words.erase(iter);
                        }
                        else
                        {
                            iter ++;
                        }
                    }
                }
    };
}


#endif
