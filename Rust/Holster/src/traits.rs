use crate::error::HolsterError;

/// A trait to make something that can be put into a Holster
/// Check out examples/advanced_usage to see how we can leverage this
/// to Holster special types
pub trait Holstable {

    /// The data type that the item will represent
    type DataType;

    /// Encode the item, and hand off the bytes or an error
    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError>;

    /// Decode the item, and hand back the data type or some error
    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>;
}
