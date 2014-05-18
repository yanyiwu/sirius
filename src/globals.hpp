#ifndef SIRIUS_GLOBAL_HPP
#define SIRIUS_GLOBAL_HPP

#include <string>
#include <vector>
#include <fstream>
#include "CppJieba/Limonp/str_functs.hpp"

namespace Sirius
{
    using namespace std;
    using namespace Limonp;

    const size_t LINE_COLLUMN_N = 3;
    const size_t POST_COLLUMN_N = 3;
    
    const size_t RESPONSE_TOP_N = 10;


    typedef size_t TokenidType;
    typedef size_t DocidType;

    struct RequestData
    {
        size_t id;
        string title;
        string content;
    };
    struct ResponseDataUnit
    {
        size_t id;
        string title;
        double similar;
        bool operator < (const ResponseDataUnit& rhs) const
        {
            return similar > rhs.similar;
        }
    };
    inline ostream& operator << (ostream& os, const ResponseDataUnit& reqdata)
    {
        return os << string_format("{\"id\": \"%u\", \"title\": \"%s\", \"similar\": \"%lf\"}", reqdata.id, reqdata.title.c_str(), reqdata.similar);
    }
    typedef vector<ResponseDataUnit> ResponseData; 

    struct DocInfo
    {
        size_t id;
        size_t offset;
        size_t length;
        string title;
        //string content;
        vector<TokenidType>  titleTokens;
        vector<TokenidType> contentTokens;
    };
}

#endif
