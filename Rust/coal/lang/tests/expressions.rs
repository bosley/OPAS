
extern crate coal_lang;

#[test]
fn expressions () {

    let code_chunks = vec![
r#"

unit main{
static module other {

    def init() -> none {}

    def give_42() -> int {

        return 42;
    }
}

static module main {

    members {
        pi: float = 3.14159;
    }
    
    def init() -> none {
        local_var : int = 3 + 5 + 7.0 * (pi ** var);

        local_var = self.give_something();
    }

    def give_something() -> int {

        return 3 + other::give_42().to_string().to_int().fail().arbitrary().length().access().is_okay();
    }
}}"#,
    ];

    for item in code_chunks.iter() {
        match coal_lang::from_source(&item.to_string()) {
            Ok(_)   => {},
            Err(e)  => {
                panic!("Failed to parse code : \n\n {} \n-----------\n {:?}", item, e);
            }
        };
    }
}