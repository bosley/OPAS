from os import name as osid

# Color dict	
color = {
	"red": "\033[1;31m",
	"blue": "\033[1;34m",
	"cyan": "\033[1;36m",
	"green": "\033[0;32m",
	"bold": "\033[;1m",
	"rev": "\033[;7m",
	"res": "\033[0;0m",
	"yellow": "\033[93m",
	"lightgreen": "\033[92m",
	"fail": "\033[91m",
	"underlined": "\033[4m",
    "kill_color": "\033[0m",
}

# parameters : (color, message, endline)
def color_pout(*args):
    
    if len(args) < 2:
        print("Error: Color_pout in module xnsdisplay->color requires at least 2 arguements")
        return

    if args[0] not in color.keys():
        print("Error: Color[", args[0], "] is not in xnsdisplay->color module")
        return

    if len(args) == 2:
        if osid == 'nt':
            print(args[1])
            return
        print(color[args[0]], args[1], color["kill_color"])
        return

    if len(args) == 3:
        if osid == 'nt':
            print(args[1], end=args[2])
            return
        print(color[args[0]], args[1], color["kill_color"], end=args[2])
        return


def colorize_text(c, text):
    
    if c not in color.keys():
        print("Error: Color[", c, "] is not in xnsdisplay->color module")
        return

    if osid == 'nt':
        return text

    return color[c] + text + color["kill_color"]
    