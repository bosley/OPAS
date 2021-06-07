/*
    Since coal is an interpreted language (for now) we won't be doing too much semantic analysis. 
    All we want to make sure here is that the static and extern rules are followed, and that there aren't 
    duplicate methods / modules. 
*/

use crate::ast;

#[derive(Debug)]
pub enum SemanticError {
    DuplicateModuleName {module: String, first_seen: ast::Location, duplicate_loc: ast::Location },
    MalformedModule { module: String, loc: ast::Location },
    NonStaticExternPresent{ module: String, loc: ast::Location },
    ExternLacksDefinition { module: String, extern_def: String, loc: ast::Location },
    DuplicateMethod { module: String, method: String, first_seen: ast::Location, duplicate_loc: ast::Location },
    StaticInitContainsParameters{ module: String, loc: ast::Location },
    StaticModuleContainsNoInit{ module: String, loc: ast::Location },
    DuplicateUnits{ first_seen: String, duplicate_loc: String, unit: String }
}

pub struct Semantics <'a> {
    source: &'a Vec<ast::Unit>

}

impl <'a> Semantics <'a> {

    pub fn new(source: &'a Vec<ast::Unit> ) -> Self {
        Self {
            source: source
        }
    }

    pub fn analyze(& mut self) -> Option<SemanticError> {

        // Check for duplicate units and ensure the existence of a 'main' unit
        {
            let mut units_seen : Vec<(String, String)> = Vec::new();

            for unit in self.source.iter() {
                for seen in units_seen.iter() {
                    if seen.1 == unit.name.as_str() {
                        return Some(SemanticError::DuplicateUnits{ 
                            first_seen: seen.0.clone(), 
                            duplicate_loc: unit.file.clone(),
                            unit: seen.1.clone()
                        })
                    }
                }
                units_seen.push((unit.file.clone(), unit.name.clone()));
            }
        }

        // Go through all units, and their modules
        for current_unit in self.source.iter() {

            let mut seen : Vec<(String, ast::Location)> = Vec::new();
            for module in current_unit.modules.iter() {

                // Make sure that all of the modules are unique
                for item in seen.iter() {
                    if *item.0 == module.name {
                        return Some(SemanticError::DuplicateModuleName{ 
                            module: module.name.clone(), 
                            first_seen: item.1.clone(),
                            duplicate_loc: module.loc.clone()
                        });
                    }
                }

                // Add this one to the list to ensure unique
                seen.push((module.name.clone(), module.loc.clone()));

                let mut externs : Vec<ast::Extern> = Vec::new();
                let mut members : Vec<ast::Members> = Vec::new();
                let mut methods : Vec<ast::MethodDefinition> = Vec::new();

                for block in module.blocks.iter() {
                    match block {
                        ast::Blocks::ExternBlock(e_block) => {  externs.push(e_block.clone()); }
                        ast::Blocks::MemberBlock(m_block) => {  members.push(m_block.clone()); }
                        ast::Blocks::MethodBlock(d_block) => {  methods = d_block.clone();     }
                    }
                }

                // Check that main is set to static
                if module.name == String::from("main") {

                    if !module.is_static {
                        return Some(SemanticError::MalformedModule{ module: module.name.clone(), loc: module.loc.clone() });
                    }
                }
                
                if module.is_static {
                    let mut found : bool = false;
                    'init_search: for method in methods.iter() {

                        if method.name == "init" {

                            if method.parameters.len() > 0 {
                                return Some(SemanticError::StaticInitContainsParameters{
                                    module: module.name.clone(),
                                    loc:    method.loc.clone()
                                });
                            }
                            found = true;
                            break 'init_search;
                        }
                    }

                    if!found {
                        return Some(SemanticError::StaticModuleContainsNoInit{module: module.name.clone(), loc: module.loc.clone()});
                    }

                }

                // There is only one item in extern_methods as-per rules enforced in the grammar, 
                // so this won't be as bad as it looks
                for extern_methods in externs.iter() {

                    // Only static methods can extern
                    if !module.is_static {
                        return Some(SemanticError::NonStaticExternPresent{ module: module.name.clone(), loc: module.loc.clone()});
                    }

                    let mut found : bool = false;

                    for item in extern_methods.externs.iter() {

                        'inner: for method in methods.iter() {
        
                            if method.name == *item {
                                found = true;
                                break 'inner;
                            }
                        }

                        // If the item isn't found, report the error
                        if !found {
                            return Some(SemanticError::ExternLacksDefinition{ module: module.name.clone(), 
                                                                            extern_def: item.clone(), 
                                                                            loc: extern_methods.loc.clone()});
                        }
                    }
                }

                // Check for duplicate methods
                {
                    let mut method_names : Vec<(String, ast::Location)> = Vec::new();

                    for method in methods.iter() {

                        for pushed in method_names.iter() {

                            if method.name == *pushed.0 {
                                // Duplicate method

                                return Some(SemanticError::DuplicateMethod{
                                    module: module.name.clone(),
                                    method: method.name.clone(),
                                    first_seen: pushed.1.clone(),
                                    duplicate_loc: method.loc.clone()
                                });
                            }
                        }
                        method_names.push((method.name.clone(), method.loc.clone()));

                        /*
                        
                            This is where we 'could' drill into the method's statements and ensure each return statement
                            matches the type of the function return

                        */
                    }
                }
            }
        }

        None
    }
}