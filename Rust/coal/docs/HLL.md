
base types:

int     - Integer
float   - Float
string  - String
list    - A list of a single type
dict    - A dictionary of mixed data
addr    - An address of a variable
module  - A group of functions and data. Only 'static' modules can extern functions!


## Removal of 'Memory'

The memory base type has been removed as a purely built-ing as it can be created as part of a standard lib, using a dict data type in the back end and the addr type to dole out direct access to a specific index


## Addr

the_addr : addr = addr::of(my_dict['key']);

the_value : int = addr::load(the_addr);

has_value : int = addr::is_valid(the_addr);


```
static module main {
    members {

        pi: float = 3.14159;

        string_pi: string = pi;
    }

    def init() -> none {

        // While the rhs value is an int, the type demands the conversion to float
        some_var: float = 2;

        // Despite the fact that this method is returning something,
        // The signature states that 'none' will be returned. This value
        // will be ignored
        return 3;
    }
}



```