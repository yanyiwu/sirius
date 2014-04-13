#include "src/IndexBuilder.hpp"
#include "gtest/gtest.h"

using namespace Sirius;

TEST(IndexBuilder, Test1)
{
    IndexBuilder(WORD_DICT_PATH, HMM_MODEL_PATH);
}
