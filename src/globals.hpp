#ifndef SIRIUS_GLOBAL_HPP
#define SIRIUS_GLOBAL_HPP

#include <string>

namespace Sirius
{
    const size_t LINE_COLLUMN_N = 2;
    const size_t POST_COLLUMN_N = 3;
    const size_t TITLE_TOP_N = 5;
    const size_t CONTENT_TOP_N = 1;

    struct DocInfo
    {
        size_t id;
        std::string title;
        std::string content;
    };
}

#endif
