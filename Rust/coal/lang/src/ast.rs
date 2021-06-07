use rug::{ Integer, Float };

#[derive(Debug, Clone)]
pub struct Location {
    pub start: usize,
    pub end:   usize
}

#[derive(Debug, Clone)]
pub struct File {
    pub dirs:    Vec<String>,
    pub imports: Vec<Import>,
    pub units:   Vec<Unit>
}

#[derive(Debug, Clone)]
pub struct Import {
    pub file_name: String, 
    pub loc:    Location
}

#[derive(Debug, Clone)]
pub struct Unit {
    pub name: String,
    pub modules: Vec<Module>,
    pub file:    String
}

#[derive(Debug, Clone)]
pub struct Module {
    pub name: String,                   // Name
    pub is_static: bool,
    pub is_pub: bool,
    pub blocks: Vec<Blocks>,                 // Functions, members, and externs
    pub loc:    Location
}

#[derive(Debug, Clone)]
pub struct Members {
    pub members: Vec<(Variable, Types, Box<Expr>, Location)>,  // Member variables and their default assignments
    pub loc:    Location
}

#[derive(Debug, Clone)]
pub struct Extern {
    pub externs: Vec<String>,           // Methods to be externed 
    pub loc:    Location
}

#[derive(Debug, Clone)]
pub struct MethodDefinition {
    pub name: String,
    pub return_type: Types,             // Return type
    pub parameters:  Vec<MethodParam>,  // Parameters into the method
    pub instructions: Vec<Statements>,  // Instructions inside the method
    pub loc:    Location
}

#[derive(Debug, Clone)]
pub struct MethodParam {
    pub name:      String,              // Parameter name
    pub data_type: Types                // Type to expect
}

#[derive(Debug, Clone)]
pub struct MemberMethodExpr {    // Definition of something called in an expr
    pub method: String,                 // method_name(3, 4, 5)
    pub params: Vec<Box<Expr>>
}

#[derive(Debug, Clone)]
pub struct ConditionalBlock {    // If / else statement
    pub expression: Option<Expr>,
    pub body:       Vec<Statements>
}

#[derive(Debug, Clone)]
pub enum Variable {                  // A variable
    Simple(String),                         // my_var
    Accessed(String, Vec<AccessorType>),    // my_var["some_thing"]["something"]
    Offset(String, Vec<AccessorType>)
}

#[derive(Debug, Clone)]
pub struct DictEntry {               // Definition of a dictionary { "key": "value" }
    pub key: String,
    pub value: Box<Expr>
}

#[derive(Debug, Clone)]
pub enum Blocks {                    // All of the blocks that might be found in a module

    MemberBlock(Members),
    ExternBlock(Extern),
    MethodBlock(Vec<MethodDefinition>)
}

#[derive(Debug, Clone)]
pub enum Statements {                // Statements that can be found in a method

    Assignment(Variable, Types, Expr, Location),
    ReAssignment(Variable, Expr, Location),
    BareExpression(Expr, Location),
    Return(Option<Expr>, Location),
    LeftArrowPush(Expr, Location),
    For(Box<Statements>, Expr, Box<Statements>, Box<Vec<Statements>>, Location),
    While(Expr, Box<Vec<Statements>>, Location)
}

#[derive(Debug, Clone)]
pub enum Expr {                      // Expressions

    Integer(Integer),
    Float(Float),
    String(String),
    Variable(Variable),
    Op(Box<Expr>, Opcode, Box<Expr>),
    UnaryOp(Box<Expr>, UnaryOpcode),
    UnaryAccess(Box<Expr>, Accessors, Box<Expr>),
    Access(Box<Expr>, Accessors, MemberMethodExpr),
    Method(MemberMethodExpr),
    IfExpression(Vec<ConditionalBlock>),
    Dict(Vec<DictEntry>),
    List(Vec<Box<Expr>>),
}

#[derive(Debug, Clone)]
pub enum Types {
    Integer,
    Float,
    String,
    List(Box<Types>),
    Dict,
    Bool,
    Addr,
    Module,
    None
}

#[derive(Debug, Clone)]
pub enum Accessors {
    Dot,    // .
    Eyes,   // ::
}

#[derive(Debug, Clone)]
pub enum AccessorType {
    RawValue(String),
    Variable(String),
    Offset(rug::Integer)
}

#[derive(Debug, Clone)]
pub enum Opcode {
    Mul,
    Div,
    Add,
    Sub,
    Lte,
    Gte,
    Gt,
    Lt,
    Equal,
    Ne,
    Pow,
    Mod,
    Lsh,
    Rsh,
    BwXor,
    BwOr,
    BwAnd,
    Or,
    And
}

#[derive(Debug, Clone)]
pub enum UnaryOpcode {
    Negate,
    BwNot
}