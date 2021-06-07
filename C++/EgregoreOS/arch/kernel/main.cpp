#include "version.hpp"
#include "screen.hpp"
#include "tty.hpp"



// -----------------------------------------------------------------------
//  Simple splash screen
// -----------------------------------------------------------------------

void splash(EGREGORE::Tty &output)
{
    output.set_foreground(EGREGORE::Screen::Color::CYAN);
    output.clear();
    output.print_str((char*)"___________                                                    \n");               
    output.print_str((char*)"\\_   _____/  ___________   ____   ____   ___________   ____    \n");
    output.print_str((char*)" |    __)_  / ___\\_  __ \\_/ __ \\ / ___\\ /  _ \\_  __ \\_/ __ \\   \n");
    output.print_str((char*)" |        \\/ /_/  >  | \\/\\  ___// /_/  >  <_> )  | \\/\\  ___/   \n");
    output.print_str((char*)"/_______  /\\___  /|__|    \\___  >___  / \\____/|__|    \\___  >  \n");
    output.print_str((char*)"        \\//_____/             \\/_____/                    \\/   \n");
    output.print_str("\n\nVersion : ");
    output.print_str(EGREGORE::VERSION::INFO);
    output.print_str("\n");
    output.set_foreground(EGREGORE::Screen::Color::WHITE);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------

void start_services(EGREGORE::Tty &output)
{
}


// -----------------------------------------------------------------------
//  Entry point from main64
// -----------------------------------------------------------------------

extern "C" void kernel_main() {

    // Define a screen region
    EGREGORE::Screen::Region full_screen_region;

    // Create a tty object for screen writing
    EGREGORE::Tty tty(full_screen_region);

    splash(tty);

    start_services(tty);
}

