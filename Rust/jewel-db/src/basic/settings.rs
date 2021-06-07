
//
/// Settings for the database
//
pub struct Settings {

    pub data_store: String,
}

impl Default for Settings {

    //
    /// Default settings for Jewel
    //
    fn default() -> Settings {

        Settings{
            data_store: ".jewel.db".to_string()
        }
    }
}