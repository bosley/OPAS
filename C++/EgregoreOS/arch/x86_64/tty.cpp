#include "tty.hpp"

namespace EGREGORE
{

Tty::Tty(Screen::Region region): Screen(region)
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Tty::print_char(char character)
{
    this->output_char(character);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Tty::print_str(char * str)
{
    for (size_t i = 0; 1; i++) 
    {
        char character = static_cast<uint8_t>(str[i]);

        if (character == '\0') 
        {
            return;
        }

        this->output_char(character);
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Tty::print_str(const char * str)
{
    for (size_t i = 0; 1; i++) 
    {
        char character = static_cast<uint8_t>(str[i]);

        if (character == '\0') 
        {
            return;
        }

        this->output_char(character);
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Tty::reprint_str(char * str)
{
    this->reset_line();
    
    for (size_t i = 0; 1; i++) 
    {
        char character = static_cast<uint8_t>(str[i]);

        if (character == '\0') 
        {
            return;
        }

        this->output_char(character);
    }
}

}