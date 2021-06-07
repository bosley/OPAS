#include "screen.hpp"

namespace EGREGORE
{

extern "C" 
{

// Screen Char
struct Char {
    uint8_t character;
    uint8_t color;
};

// Screen Memory Region
static struct Char* screen_region_buffer = (struct Char*) 0xB8000;

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

Screen::Screen(Screen::Region region) : _region(region),
                                        _foreground(Screen::Color::WHITE),
                                        _background(Screen::Color::BLACK)
{
    this->_current_row = 0;
    this->_current_col = 0;
    this->update_output_color();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

Screen::Screen(Screen::Region region,
            Screen::Color foreground, 
            Screen::Color background) 
                                      : _region(region),
                                        _foreground(foreground),
                                        _background(background)
{
    this->update_output_color();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::set_background(Screen::Color color)
{
    _background = color;
    this->update_output_color();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::set_foreground(Screen::Color color)
{
    _foreground = color;
    this->update_output_color();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::update_output_color()
{
    _output_color = static_cast<uint8_t>(_foreground) + 
                   (static_cast<uint8_t>(_background) << 4);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::clear()
{
    // Clear all of the rows within the screen region
    for( size_t i = this->_region.get_y(); i < this->_region.get_h(); i++ )
    {
        this->clear_row(i);
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::clear_row(size_t row)
{
    // Create the output char
    struct Char empty = (struct Char) 
    {
        character: ' ',
        color: _output_color,
    };

    // Output the clear chars on the whole row
    for (size_t col = this->_region.get_x(); col < this->_region.get_w(); col++) 
    {
        screen_region_buffer[col + this->_region.get_w() * row] = empty;
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::output_newline()
{
    this->_current_col = 0;
    if ( this->_current_row < this->_region.get_h() - 1)
    {
        this->_current_row += 1;
        return;
    }

    for (size_t row = 1; row < this->_region.get_h(); row++) 
    {
        for (size_t col = 0; col < this->_region.get_w(); col++) 
        {
            struct Char character = screen_region_buffer[col + this->_region.get_w() * row];
            screen_region_buffer[col + this->_region.get_w() * (row - 1)] = character;
        }
    }

    clear_row(this->_region.get_h() - 1);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::output_char(char character)
{
    if (character == '\n') 
    {
        this->output_newline();
        return;
    }

    if (this->_current_col > this->_region.get_w()-1) 
    {
        this->output_newline();
    }

    screen_region_buffer[this->_current_col + this->_region.get_w() * this->_current_row] = 
        (struct Char) 
        {
            character: static_cast<uint8_t>(character),
            color:     static_cast<uint8_t>(this->_output_color),
        };

    this->_current_col++;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void Screen::reset_line()
{
    if (this->_current_col == 0 )
    {
        return;
    }

    clear_row(this->_current_row);

    this->_current_col = 0;
}

}