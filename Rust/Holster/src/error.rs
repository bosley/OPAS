use std::fmt;

/// The errors that a Holstable item can throw
pub enum HolsterError {

    /// Data length does not match the expected type
    InsufficientData,

    /// Error getting a float from a slice
    FloatFromSliceFailure,
    
    /// Error building a utf-8 from item
    FromUtf8Failure,

    /// Custom Failure for implementers to use
    CustomFailure(&'static str)
}

impl fmt::Debug for HolsterError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            HolsterError::InsufficientData => {

                write!(f, "Insufficient data for decode")
            }

            HolsterError::FloatFromSliceFailure => {

                write!(f, "Error decoding Float object")
            }

            HolsterError::FromUtf8Failure => {

                write!(f, "Error decoding string from suspected UTF-8 bytes")
            }

            HolsterError::CustomFailure(s) => {

                write!(f, "Failure: {}", s)
            }
        }
    }
}

impl fmt::Display for HolsterError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            HolsterError::InsufficientData => {

                write!(f, "Insufficient data for decode")
            }

            HolsterError::FloatFromSliceFailure => {

                write!(f, "Error decoding Float object")
            }

            HolsterError::FromUtf8Failure => {

                write!(f, "Error decoding string from suspected UTF-8 bytes")
            }

            HolsterError::CustomFailure(s) => {

                write!(f, "Failure: {}", s)
            }
        }
    }
}