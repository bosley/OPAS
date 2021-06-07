#ifndef EGREGORE_SCREEN_HPP
#define EGREGORE_SCREEN_HPP

#include <stdint.h>
#include <stddef.h>

namespace EGREGORE
{

/*

*/
class Screen
{
public:

    /*
        Colors
    */
    enum class Color
    {
        BLACK       = 0,
        BLUE        = 1,
        GREEN       = 2,
        CYAN        = 3,
        RED         = 4,
        MAGENTA     = 5,
        BROWN       = 6,
        LIGHT_GRAY  = 7,
        DARK_GRAY   = 8,
        LIGHT_BLUE  = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN  = 11,
        LIGHT_RED   = 12,
        PINK        = 13,
        YELLOW      = 14,
        WHITE       = 15,
    };

    /*
        Screen Region
    */
    class Region
    {
    public:

        const static size_t DEFAULT_X      =  0;
        const static size_t DEFAULT_Y      =  0;
        const static size_t DEFAULT_HEIGHT = 25;
        const static size_t DEFAULT_WIDTH  = 80;

        Region() : _x(DEFAULT_X),      _y(DEFAULT_Y), 
                   _h(DEFAULT_HEIGHT), _w(DEFAULT_WIDTH) { }

        Region(size_t pos_x, size_t pos_y, size_t height, size_t width) : _x(pos_x),  _y(pos_y), 
                                                                          _h(height), _w(width) { }
        const size_t get_x() { return _x; }
        const size_t get_y() { return _y; }
        const size_t get_h() { return _h; }
        const size_t get_w() { return _w; }

    private:
        size_t _x; // Top left position X
        size_t _y; // Top left position Y
        size_t _h; // Top left position X
        size_t _w; // Top left position Y
    };

    // Screen
    Screen(Screen::Region region);
    
    Screen(Screen::Region region, 
            Screen::Color foreground, 
            Screen::Color background);

    void set_background(Screen::Color color);
    void set_foreground(Screen::Color color);
    void clear();

protected:
    Screen::Region _region;
    Screen::Color  _foreground;
    Screen::Color  _background;

    void output_char(char character);
    void output_newline();
    void reset_line();

private:
    uint8_t _output_color;
    void update_output_color();

    void clear_row(size_t row);

    size_t _current_row;
    size_t _current_col;


};

}

#endif