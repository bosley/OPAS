
use std::collections::HashMap;
use std::rc::Rc;
use std::cell::RefCell;


use coal_ast::Module;
use crate::EngineModule;

#[derive(Debug)]
pub enum EngineError {
    DuplicateModuleName(String),
    MalformedModule(&'static str),
    NonStaticExternPresent(String),
    ExternLacksDefinition(String, String),
}

pub struct Engine {
    static_modules: HashMap<String, Rc<RefCell<EngineModule>>>,
    modules:        HashMap<String, EngineModule>
}

impl Engine {

    pub fn new() -> Self {
        Self {
            static_modules: HashMap::new(),
            modules:        HashMap::new(),
        }
    }

    pub fn load(&mut self, modules: Vec<Module>) -> Option<EngineError> {

        let mut seen : Vec<String> = Vec::new();
        for module in modules.iter() {
            
            // Make sure that all of the modules are unique
            for item in seen.iter() {
                if *item== module.name {
                    return Some(EngineError::DuplicateModuleName(module.name.clone()))
                }
            }

            // Add this one to the list to ensure unique
            seen.push(module.name.clone());

            // Check that main is set to static
            if module.name == String::from("main") {
                if !module.is_static {
                    return Some(EngineError::MalformedModule("Main module not declared 'static'"));
                }
            }

            // Create engine module representations from each ast module and store in corresponding list
            if module.is_static {

                // Insert the module
                self.static_modules.insert(module.name.clone(), Rc::new(RefCell::new(EngineModule::from_ast_module(module))));

                {
                    // Ensure all externed methods exist
                    let e_mod = self.static_modules.get(&module.name.clone()).unwrap().borrow();
                    for extern_method in e_mod.extern_methods.iter() {
                        match e_mod.methods.get(extern_method) {
                            Some(_) => { }
                            None    => {
                                return Some(EngineError::ExternLacksDefinition(
                                    e_mod.name.clone(),
                                    extern_method.clone()
                                ));
                            }
                        }
                    }
                }

                // Instantiate all static modules that aren't main
                if module.name != String::from("main") {

                    self.init_static_module(&module.name.clone())?;
                }

            } else {

                // Make sure that non-static modules aren't attempting to extern 
                for block in module.blocks.iter() {
                    match block {
                        coal_ast::Blocks::ExternBlock(eb) => {
                            if eb.externs.len() > 0 {
                                return Some(EngineError::NonStaticExternPresent(module.name.clone()))
                            }
                        }
                        _=> {}
                    }
                }
                self.modules.insert(module.name.clone(), EngineModule::from_ast_module(module));
            }
        }
        None
    }

    pub fn start(&mut self) -> Option<EngineError> {

        return self.init_static_module(&String::from("main"));
    }

    //pub fn call_extern_method(&mut self, module: &String, method: &String) {
    //    
    //}

    fn init_static_module(&mut self, module_name: &String) -> Option<EngineError> {

        // Execute population of default_members

        // Attempt to get 'init' method from static method, and execute its instruction set

        let e_mod = match self.static_modules.get(module_name) {
            Some(module) => { module.borrow_mut() }
            None => {
                panic!("Asked to init a static module that doesn't exist!");
            }
        };

        match e_mod.methods.get(&String::from("init")) {
            Some(method) => {
                if method.parameters.len() > 0 {
                    return Some(EngineError::MalformedModule("Unhandled static module parameters for 'init'"));
                }

                match method.return_type {
                    coal_ast::Types::None => { }
                    _ => {
                        return Some(EngineError::MalformedModule("Static module return type must be of type 'none'"));
                    }
                }
            }

            None => {
                // No init method

                return None;
            }
        }

        None
    }
}