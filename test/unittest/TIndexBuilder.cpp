#include "src/IndexBuilder.hpp"
#include "gtest/gtest.h"

using namespace Sirius;

const char * const FILE_PATH = "../test/testdata/title_content_rows_2000.utf8";
const char * const OUT_FILE_PATH = "word_tokenid_map";

TEST(IndexBuilder, Test1)
{
    IndexBuilder builder(WORD_DICT_PATH, HMM_MODEL_PATH);
    ASSERT_TRUE(builder);
    ASSERT_TRUE(builder.build(FILE_PATH));
    ASSERT_TRUE(builder.dumpWordTokenidMap(OUT_FILE_PATH));
}
