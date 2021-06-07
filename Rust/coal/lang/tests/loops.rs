
extern crate coal_lang;

#[test]
fn loops () {

    let code_chunks = vec![
r#"
unit main{
static module main {
    def init() -> none {
        while 1 {
        }
    }
}}
"#,
r#"
unit main{
static module main {
    def init() -> none {
        for ( some_var: int = 3; some_var != 0; some_var = some_var - 1; ) {   
        }
    }
}}
"#,
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