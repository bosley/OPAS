use crate::Holstable;
use crate::HolsterError;

/// The Holster - Basically just a vector wrapper to hold onto and facilitate 
/// the encoding and decoding of a Holstable item
/// 
/// # Built-In Holster Implementations
/// 
/// u8, u16, u32, u64, u128, i8, i16, i32, i64, i128, f32, f64, and String
/// 
/// When encoding / decoding the above types the Holster implementation takes some steps
/// to attempt keeping proper endianess in the event of a holstered item being written out
/// and loaded in on a different system. This probably isn't an issue with almost everything
/// using little endian representations, but it *could* be an issue somewhere so it converts
/// to and from little endian when encoding and decoding.
/// 
/// # Basic Example
/// 
/// ```
///     extern crate holster;
///     use holster::Holster;  
/// 
///     // Create the holster
///     let mut simple_holster = Holster::new();
///     
///     // Make some random x
///     let x : i32 = 9032;
///     
///     // Store x
///     match simple_holster.holster(&x) {
///         Ok(_) => {}
///         Err(e) => {
///             panic!("Failed to holster item : {:?}", e);
///         }
///     }
///     
///     // Get it and call it y
///     let y = match simple_holster.un_holster::<i32>() {
///         Ok(val) => val,
///         Err(e)  => {
///             panic!("Failed to un holster item : {:?}", e);
///         }
///     };
/// ```
/// 
pub struct Holster {
    data: Vec<u8>
}

impl Holster {

    /// Create a new Holster
    pub fn new() -> Self {
        Self {
            data: Vec::new()
        }
    }

    /// Attempt to holster an item. Returns a HolsterError iff something is thrown
    /// from the encoder implementation
    pub fn holster<T: Holstable>(&mut self, item: &T) -> Result<(), HolsterError>
    {
        self.data = item.holster_encode()?;
        Ok(())
    }

    /// Attempt to take and item out of the holster. Returns a HolsterError iff something
    /// is thrown from the decoder implementation
    pub fn un_holster<T: Holstable>(&mut self) -> Result<T::DataType, HolsterError>
    {
        let item : T::DataType = T::holster_decode(&self.data)?;
        self.data.clear();
        Ok(item)
    }

    /// Retrieve the encoded data
    pub fn get_data(&self) -> Vec<u8> {
        self.data.clone()
    }
}