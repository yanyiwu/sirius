#include "src/SearchEngine.hpp"
#include "gtest/gtest.h"

using namespace Sirius;

const char * const FILE_PATH = "../test/testdata/id_title_content_rows2000.utf8";
const char * const OUT_FILE_PATH = "word_tokenid_map";
const char * const TITLE_QUERY_PATH = "../test/testdata/title_query.utf8";
const char * const CONTENT_QUERY_PATH = "../test/testdata/content_query.utf8";
const char * const WORD_DICT_PATH = "../dict/word_dict.utf8";
const char * const HMM_MODEL_PATH = "../dict/hmm_model.utf8";
const char * const STOP_WORD_PATH = "../dict/stop_words.utf8";


TEST(SearchEngine, test1)
{
    Tokenizer tokenizer(WORD_DICT_PATH, HMM_MODEL_PATH, STOP_WORD_PATH);
    Indexer indexer(tokenizer);
    indexer.build(FILE_PATH);
    SearchEngine searcher(indexer);
    RequestData reqdata;
    reqdata.id = 1;
    reqdata.title = "中国政府看望中国乘客家属";
    reqdata.content = "content1";
    ResponseData resdata;
    searcher.search(reqdata,resdata);
    cout << resdata << endl;
}
