use std::fmt;

pub const RECORD_NOT_FOUND      : u64 = 100;
pub const DB_DESERIALIZATION    : u64 = 200;
pub const DB_SERIALIZATION      : u64 = 201;
pub const DB_WRITE_FAILURE      : u64 = 202;

/// A Database error
pub struct DbError{
    pub code:       u64,
    pub message:    String
}

// Different error messages according to DbError.code
impl fmt::Display for DbError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "[{}]: {}", self.code, self.message)
    }
}

// A unique format for debugging output
impl fmt::Debug for DbError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "DbError {{ code: {}, message: {} }}",
            self.code, self.message
        )
    }
}
