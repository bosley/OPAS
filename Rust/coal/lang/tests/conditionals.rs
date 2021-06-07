
extern crate coal_lang;

#[test]
fn conditionals () {

    let code_chunks = vec![
r#"
unit main{
static module main {

    members {
        pi: float = 3.14159;
    }
    
    def init() -> none {
    
        if 1 > 0 { 
            some_int : int = 3;
        } elif 1 == 1 {
            some_int : int = 3;
        } else {
            some_int : int = 3;
        };
    }

    def as_expression() -> none {

        some_value : int = if 1 > 0 {
            <- 4;
        } else {
            <- 0;
        };
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