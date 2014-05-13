#include <unistd.h>
#include <algorithm>
#include <string>
#include <ctype.h>
#include <string.h>
#include "CppJieba/Limonp/Config.hpp"
#include "RequestHandler.hpp"

using namespace Sirius;

bool run(int argc, char** argv)
{
    if(argc < 2)
    {
        return false;
    }
    Config conf(argv[1]);
    if(!conf)
    {
        return false;
    }
    unsigned int port = 0;
    string dictPath;
    string modelPath;
    string val;
    if(!conf.get("port", val))
    {
        LogFatal("conf get port failed.");
        return false;
    }
    port = atoi(val.c_str());

    if(!conf.get("dict_path", dictPath))
    {
        LogFatal("conf get dict_path failed.");
        return false;
    }
    if(!conf.get("model_path", modelPath))
    {
        LogFatal("conf get model_path failed.");
        return false;
    }

    IndexBuilder indexBuilder(dictPath, modelPath);
    RequestHandler reqHandler(indexBuilder);
    EpollServer sf(port, &reqHandler);
    return sf.start();
}

int main(int argc, char* argv[])
{
    if(!run(argc, argv))
    {
        printf("usage: %s <config_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

