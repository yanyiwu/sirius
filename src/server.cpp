#include <unistd.h>
#include <algorithm>
#include <string>
#include <ctype.h>
#include <string.h>
#include "CppJieba/Limonp/Config.hpp"
#include "RequestHandler.hpp"

using namespace Sirius;

class SiriusServer: public InitOnOff
{
    private:
        size_t _port;
        string _dictPath;
        string _modelPath;
        string _stopWordPath;
    public:
        SiriusServer(const Config& conf)
        {
            CHECK(!_getConfigArg(conf, "port", _port));
            CHECK(!_getConfigArg(conf, "dict_path", _dictPath));
            CHECK(!_getConfigArg(conf, "model_path", _modelPath));
            CHECK(!_getConfigArg(conf, "stop_word_path", _stopWordPath));
        };
        ~SiriusServer(){};
    private:
        bool _getConfigArg(const Config& conf, const string& key, string& value) const
        {
            if(!conf.get(key, value))
            {
                return false;
            }
            return true;
        }
        bool _getConfigArg(const Config& conf, const string& key, size_t& value) const
        {
            string str;
            if(!conf.get(key, str))
            {
                return false;
            }
            value = atoi(str.c_str());
            if(!value)
            {
                return false;
            }
            return true;
        }
    public:
        bool start()
        {
            IndexBuilder indexBuilder(_dictPath, _modelPath, _stopWordPath);
            RequestHandler reqHandler(indexBuilder);
            EpollServer server(_port, &reqHandler);
            return server.start();
        }

};

int main(int argc, char* argv[])
{
    if(argc < 2 || 0 ==  strcmp(argv[1], "-h") || 0 == strcmp(argv[1], "--help"))
    {
        fprintf(stderr, "usage: %s <config_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Config conf(argv[1]);
    assert(conf);

    SiriusServer server(conf);
    server.start();

    return EXIT_SUCCESS;
}

