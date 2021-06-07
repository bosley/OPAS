
extern crate coal_lang;

#[test]
fn basic_module () {

    let code = r#"
unit main{
static module main {

    extern {
        some_func
    }
    
    members {
        counter: int   = 0;
        moot:    float = 99.0;
        data:    dict  = {
            'key_1': 3,
            'key_2': "A string"
        };
        my_list: list<int> = [];
    }

    def init() -> none {
        
    }

    def some_func() -> none {

    }
}}"#;

    let source_tree = match coal_lang::from_source(&code.to_string()) {
        Ok(result) => { result },
        Err(e)     => {
            panic!("Parse Error: {:?}", e);
        }
    };


    assert_eq!(1, source_tree.len());
    assert_eq!(1, source_tree[0].modules.len());

    assert_eq!(true, source_tree[0].modules[0].is_static);
    assert_eq!("main", source_tree[0].modules[0].name);
    assert_eq!(3, source_tree[0].modules[0].blocks.len());

    for item in source_tree[0].modules[0].blocks.iter() {

        match item {
            coal_lang::ast::Blocks::MemberBlock(member) => {
                assert_eq!(4, member.members.len());

                match &member.members[0].0 {
                    coal_lang::ast::Variable::Simple(v) => {
                        assert_eq!("counter", v)
                    }
                    _ => { panic!("Parser build incorrect variable type"); }
                }

                match &member.members[0].1 {
                    coal_lang::ast::Types::Integer => {}
                    _ => { panic!("Parser build incorrect data type"); }
                }

                match &member.members[1].0 {
                    coal_lang::ast::Variable::Simple(v) => {
                        assert_eq!("moot", v)
                    }
                    _ => { panic!("Parser build incorrect variable type"); }
                }

                match &member.members[1].1 {
                    coal_lang::ast::Types::Float => {}
                    _ => { panic!("Parser build incorrect data type"); }
                }

                match &member.members[2].0 {
                    coal_lang::ast::Variable::Simple(v) => {
                        assert_eq!("data", v)
                    }
                    _ => { panic!("Parser build incorrect variable type"); }
                }

                match &member.members[2].1 {
                    coal_lang::ast::Types::Dict => {}
                    _ => { panic!("Parser build incorrect data type"); }
                }

                match &member.members[3].0 {
                    coal_lang::ast::Variable::Simple(v) => {
                        assert_eq!("my_list", v)
                    }
                    _ => { panic!("Parser build incorrect variable type"); }
                }

                match &member.members[3].1 {
                    coal_lang::ast::Types::List(_) => {}
                    _ => { panic!("Parser build incorrect data type"); }
                }
            }

            coal_lang::ast::Blocks::ExternBlock(member) => {
                assert_eq!(1, member.externs.len());
                assert_eq!("some_func", member.externs[0]);
            }

            coal_lang::ast::Blocks::MethodBlock(methods) => {

                assert_eq!(2, methods.len());
            }
        }
    }

}