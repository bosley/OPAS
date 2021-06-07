use coal_lang;

use std::collections::HashMap;

pub(crate) struct ModuleMethod {
    pub(crate) return_type : coal_ast::Types,
    pub(crate) parameters  : Vec<coal_ast::MethodParam>,
    pub(crate) instructions: Vec<coal_ast::Statements>
}

impl ModuleMethod {
    fn new(ast_def: &coal_ast::MethodDefinition) -> Self {
        Self {
            return_type:  ast_def.return_type.clone(),
            parameters:   ast_def.parameters.clone(),
            instructions: ast_def.instructions.clone()
        }
    }
}


pub(crate) struct EngineModule {

    pub(crate) name: String,
    pub(crate) methods: HashMap<String, ModuleMethod>,
    pub(crate) default_members: Vec<(coal_ast::Variable, coal_ast::Types, Box<coal_ast::Expr>, coal_ast::Location)>,
    pub(crate) extern_methods: Vec<String>


    // Members: Hashmap<String, RecordData>

}

impl EngineModule {

    pub(crate) fn from_ast_module(module: &coal_ast::Module) -> Self {

        let mut engine_methods : HashMap<String, ModuleMethod> = HashMap::new();
        let mut default_members :  Vec<(coal_ast::Variable, coal_ast::Types, Box<coal_ast::Expr>, coal_ast::Location)> = Vec::new();
        let mut module_externs : Vec<String> = Vec::new();

        for block in module.blocks.iter() {
            match block {
        
                // Create the 'methods' hashmap 
                coal_ast::Blocks::MethodBlock(methods) => {
                    for method in methods.iter(){
                        engine_methods.insert(method.name.clone(), ModuleMethod::new(&method));
                    }
                }

                // Copy over the member definition
                coal_ast::Blocks::MemberBlock(members) => {
                    default_members = members.members.clone();
                }

                // Copy extern list
                coal_ast::Blocks::ExternBlock(externs) => {
                    module_externs = externs.externs.clone() 
                }
            }
        }

        Self {
            name: module.name.clone(),
            methods: engine_methods,
            default_members: default_members,
            extern_methods: module_externs
        }
    }
}