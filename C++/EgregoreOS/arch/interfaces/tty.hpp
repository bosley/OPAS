#ifndef EGREGORE_TTY_HPP
#define EGREGORE_TTY_HPP

#include "screen.hpp"

namespace EGREGORE
{

class Tty : public Screen
{
public:
    Tty(Screen::Region region);

    void print_char(char  character);
    void print_str (char* string);
    void print_str (const char* string);
    void reprint_str(char * string);        // Print over current line (useful for status bars)

private:
};

}

#endif