#include "cli.h"

#include <array>
#include <mutex>
#include <iostream>
#include <vector>

namespace MOD
{

namespace
{
    struct _history
    {
        std::string line;
        CLI_RESULT  result;
    };

    unsigned _historyIndex(0);
    std::mutex _historyMutex;

    std::array<_history, MOD_CLI_HISTORY_RESERVE> _cliHistory;

void addHistory(std::string line, CLI_RESULT result)
{
    std::lock_guard<std::mutex> lock(_historyMutex);

    _cliHistory[_historyIndex % MOD_CLI_HISTORY_RESERVE] = {line, result};
    ++_historyIndex;

    if(_historyIndex > MOD_CLI_HISTORY_RESERVE)
    {
        _historyIndex = 0;
    }
}

CLI_RESULT processLine(std::string line)
{
    std::cout << "MOD::ANON::"  << __FUNCTION__
              << "(string: "    << line << ")"
              << std::endl;

    CLI_RESULT result;

    addHistory(line, result);
    return result;
}

} // End anon namespace

CLI_RESULT cliProcess(std::string line)
{
    std::cout << "MOD::"        << __FUNCTION__
              << "(string: "    << line << ")"
              << std::endl;

    return processLine(line);
}

}
