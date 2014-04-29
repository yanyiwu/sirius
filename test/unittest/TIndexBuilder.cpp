#include "src/IndexBuilder.hpp"
#include "gtest/gtest.h"

using namespace Sirius;

const char * const FILE_PATH = "../test/testdata/title_content_rows_2000.utf8";
const char * const OUT_FILE_PATH = "word_tokenid_map";
const char * const TITLE_QUERY_PATH = "../test/testdata/title_query.utf8";
const char * const CONTENT_QUERY_PATH = "../test/testdata/content_query.utf8";

TEST(IndexBuilder, Title)
{
    IndexBuilder builder(WORD_DICT_PATH, HMM_MODEL_PATH);
    ASSERT_TRUE(builder);
    ASSERT_TRUE(builder.build(FILE_PATH));
    ASSERT_TRUE(builder.dumpWordMap(OUT_FILE_PATH));
    string res;
    ifstream ifs(TITLE_QUERY_PATH);
    ASSERT_TRUE(ifs);
    string line;
    while(getline(ifs, line))
    {
        print(line);
        builder.queryTitle(line, res);
    }
    
}

TEST(IndexBuilder, Content)
{
    IndexBuilder builder(WORD_DICT_PATH, HMM_MODEL_PATH);
    ASSERT_TRUE(builder);
    ASSERT_TRUE(builder.build(FILE_PATH));
    ASSERT_TRUE(builder.dumpWordMap(OUT_FILE_PATH));
    string res;
    ifstream ifs(CONTENT_QUERY_PATH);
    ASSERT_TRUE(ifs);
    string line;
    while(getline(ifs, line))
    {
        print(line);
        builder.queryContent(line, res);
    }
    
}
