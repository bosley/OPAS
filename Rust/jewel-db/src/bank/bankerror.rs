use std::fmt;


/// A Database error
pub enum BankError{
    PathError(&'static str),
    BankNotFound(&'static str),
    RecordNotFound(&'static str),
    RecordCreationError,
    RecordWriteError,
    RecordReadError,
    RecordRemoveError,
    InvalidKey,
    LockError
}

impl fmt::Display for BankError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        display_error(self, f)
    }
}

impl fmt::Debug for BankError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        display_error(self, f)
    }
}

fn display_error(err: &BankError, f: &mut fmt::Formatter) -> fmt::Result {

    match err {
        BankError::PathError(s) => {
            write!(f, "There was an error with the given bank path : {}", s)
        }
        BankError::BankNotFound(s) => {
            write!(f, "Unable to find data bank [{}]", s)
        },
        BankError::RecordNotFound(s) => {
            write!(f, "Unable to find record [{}]", s)
        },
        BankError::RecordCreationError => {
            write!(f, "Unable to create record")
        },
        BankError::RecordWriteError => {
            write!(f, "Unable to write record")
        },
        BankError::RecordReadError => {
            write!(f, "Unable to read record")
        },
        BankError::RecordRemoveError => {
            write!(f, "Unable to remove record")
        },
        BankError::InvalidKey => {
            write!(f, "Key is not a valid UTF-8 sequence")
        }
        BankError::LockError => {
            write!(f, "An unforseen lock error occured")
        }
    }
}
