#ifndef SIRIUS_GLOBAL_HPP
#define SIRIUS_GLOBAL_HPP

#include <string>

namespace Sirius
{
    const char* const WORD_DICT_PATH = "../dict/word_dict.utf8";
    const char* const HMM_MODEL_PATH = "../dict/hmm_model.utf8";

    struct DocInfo
    {
        size_t id;
        std::string title;
        std::string content;
    };
}

#endif
