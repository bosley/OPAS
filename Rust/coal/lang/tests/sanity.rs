
extern crate coal_lang;

#[test]
fn sanity_checks () {

    let code_chunks = vec![
r#"
unit main{
static module main {
    def init() -> none {}
}}"#,
r#"
unit main{
static module main {
    extern  {}
    def init() -> none {
    }
}}"#,
r#"
unit main{
static module main {
    members {}
    def init() -> none {}
}}"#,
r#"
unit main{
static module main {
    members {}
    extern  {}
    def init() -> none {}
}}"#,
r#"
unit main{
static module main {
    members {}
    extern  {}
    def init() -> none {}
    def method_1() -> int {}
    def method_2() -> int {}
    def method_3() -> int {}
    def method_4() -> int {}
    def method_5() -> int {}
}}"#,
r#"
unit main{
static module main {
    members { local: int = 0; local_f: float = 0.00; local_s: string = "A string"; }
    extern  { method_3, method_5 }
    def init() -> none {}
    def method_1() -> list<int> {}
    def method_2() -> list<int> {}
    def method_3() -> list<int> {}
    def method_4() -> list<int> {}
    def method_5() -> list<int> {}
}}"#,
r#"
unit main{
static module main {
    def init() -> none {}
    def method_1(v: int, f: float) -> float {}
    def method_2(i: int) -> none { g: int = i; }
    def method_3() -> none {g: int = i; h: int = i; m : int = i;}
    def method_4(var: list<int>) -> none { b: bool = 1; }
    def method_5(var: list<float>) -> none {}
    def method_6(var: list<string>) -> none {}
    def method_7(var: list<dict>) -> none {}
    def method_8(var: list<addr>) -> none {}
    def method_9(var: list< list<int> >) -> bool {}
}}"#,
r#"
unit main{
static module main {
    def init() -> none {
        some_static::call();
        some_static::call(3, 5, 7, a + 3 - p);
        func();
    }
    def func() -> int {

        local : string = "This is a local string";
        return 33;
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