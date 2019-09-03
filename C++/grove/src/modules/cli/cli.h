#ifndef MOD_CLI_H
#define MOD_CLI_H

/*

    Functions to handle parsing command line commands for grove.
    Parsed lines will be pushed to grove for execution.

*/

#define MOD_CLI_HISTORY_RESERVE 5000

#include <string>

namespace MOD
{

//!
//! \brief The CLI_RESULT struct
//!        This result struct is the result of syntax analyzation.
//!        Only commands with valid syntax will be sent to be executed.
struct CLI_RESULT
{
    bool success;

    CLI_RESULT()
    {
        success = false;
    }

    CLI_RESULT & operator= (const CLI_RESULT rhs)
    {
        this->success = rhs.success;
        return *this;
    }
};

//!
//! \brief cliProcess
//! \param line - The line to process
//! \return CLI_RESULT
//!
CLI_RESULT cliProcess(std::string line);

}

#endif // CLI_H
