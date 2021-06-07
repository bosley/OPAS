
use crate::error::HolsterError;
use crate::Holstable;

use std::convert::TryInto;

/*

        String

*/

/// Holstable for std::string::String
impl Holstable for String {
    
    type DataType = String;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let result = Vec::from(self.as_bytes());
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        let result : Self::DataType = match String::from_utf8(data.clone()) {
            Ok(r) =>  r,
            Err(_) => return Err(HolsterError::FromUtf8Failure)
        };
        Ok(result)
    }
}

/*

        Unsigned Numbers

*/

/// Holstable for u8
impl Holstable for u8 {
    
    type DataType = u8;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {
        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push(item);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 1 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = Self::DataType::from_le( data[0] ); 
        Ok(result)
    }
}

/// Holstable for u16
impl Holstable for u16 {

    type DataType = u16;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);

        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 2 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 8  |
                            (data[1] as Self::DataType) << 0;
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for u32
impl Holstable for u32 {

    type DataType = u32;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 24) as u8);
        result.push((item >> 16) as u8);
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 4 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 24 |
                            (data[1] as Self::DataType) << 16 | 
                            (data[2] as Self::DataType) << 8  |
                            (data[3] as Self::DataType) << 0;

        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for u64
impl Holstable for u64 {

    type DataType = u64;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 56) as u8);
        result.push((item >> 48) as u8);
        result.push((item >> 40) as u8);
        result.push((item >> 32) as u8);
        result.push((item >> 24) as u8);
        result.push((item >> 16) as u8);
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 8 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 56 |
                            (data[1] as Self::DataType) << 48 | 
                            (data[2] as Self::DataType) << 40 |
                            (data[3] as Self::DataType) << 32 |
                            (data[4] as Self::DataType) << 24 |
                            (data[5] as Self::DataType) << 16 | 
                            (data[6] as Self::DataType) << 8  |
                            (data[7] as Self::DataType) << 0;
        Ok(Self::DataType::from_le(result))
    }
}

/*

        Signed Numbers

*/

/// Holstable for i8
impl Holstable for i8 {
    
    type DataType = i8;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push(item as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 1 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = data[0] as i8; 
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for i16
impl Holstable for i16 {

    type DataType = i16;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 2 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 8  |
                            (data[1] as Self::DataType) << 0;
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for i32
impl Holstable for i32 {

    type DataType = i32;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 24) as u8);
        result.push((item >> 16) as u8);
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 4 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 24 |
                            (data[1] as Self::DataType) << 16 | 
                            (data[2] as Self::DataType) << 8  |
                            (data[3] as Self::DataType) << 0;
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for i64
impl Holstable for i64 {

    type DataType = i64;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 56) as u8);
        result.push((item >> 48) as u8);
        result.push((item >> 40) as u8);
        result.push((item >> 32) as u8);
        result.push((item >> 24) as u8);
        result.push((item >> 16) as u8);
        result.push((item >> 8)  as u8);
        result.push((item >> 0)  as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 8 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0] as Self::DataType) << 56 |
                            (data[1] as Self::DataType) << 48 | 
                            (data[2] as Self::DataType) << 40 |
                            (data[3] as Self::DataType) << 32 |
                            (data[4] as Self::DataType) << 24 |
                            (data[5] as Self::DataType) << 16 | 
                            (data[6] as Self::DataType) << 8  |
                            (data[7] as Self::DataType) << 0;
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for u128
impl Holstable for u128 {

    type DataType = u128;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 120) as u8);
        result.push((item >> 112) as u8);
        result.push((item >> 104) as u8);
        result.push((item >> 96 ) as u8);
        result.push((item >> 88 ) as u8);
        result.push((item >> 80 ) as u8);
        result.push((item >> 72 ) as u8);
        result.push((item >> 64 ) as u8);
        result.push((item >> 56 ) as u8);
        result.push((item >> 48 ) as u8);
        result.push((item >> 40 ) as u8);
        result.push((item >> 32 ) as u8);
        result.push((item >> 24 ) as u8);
        result.push((item >> 16 ) as u8);
        result.push((item >> 8  ) as u8);
        result.push((item >> 0  ) as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 16 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0]  as Self::DataType) << 120 |
                            (data[1]  as Self::DataType) << 112 | 
                            (data[2]  as Self::DataType) << 104 |
                            (data[3]  as Self::DataType) << 96  |
                            (data[4]  as Self::DataType) << 88  |
                            (data[5]  as Self::DataType) << 80  | 
                            (data[6]  as Self::DataType) << 72  |
                            (data[7]  as Self::DataType) << 64  |
                            (data[8]  as Self::DataType) << 56  |
                            (data[9]  as Self::DataType) << 48  | 
                            (data[10] as Self::DataType) << 40  |
                            (data[11] as Self::DataType) << 32  |
                            (data[12] as Self::DataType) << 24  |
                            (data[13] as Self::DataType) << 16  | 
                            (data[14] as Self::DataType) << 8   |
                            (data[15] as Self::DataType) << 0   ;
        Ok(Self::DataType::from_le(result))
    }
}

/// Holstable for i128
impl Holstable for i128 {

    type DataType = i128;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        let item : Self::DataType = self.to_le();
        let mut result = Vec::new();
        result.push((item >> 120) as u8);
        result.push((item >> 112) as u8);
        result.push((item >> 104) as u8);
        result.push((item >> 96 ) as u8);
        result.push((item >> 88 ) as u8);
        result.push((item >> 80 ) as u8);
        result.push((item >> 72 ) as u8);
        result.push((item >> 64 ) as u8);
        result.push((item >> 56 ) as u8);
        result.push((item >> 48 ) as u8);
        result.push((item >> 40 ) as u8);
        result.push((item >> 32 ) as u8);
        result.push((item >> 24 ) as u8);
        result.push((item >> 16 ) as u8);
        result.push((item >> 8  ) as u8);
        result.push((item >> 0  ) as u8);
        Ok(result)
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 16 {
            return Err(HolsterError::InsufficientData);
        }

        let result : Self::DataType = 
                            (data[0]  as Self::DataType) << 120 |
                            (data[1]  as Self::DataType) << 112 | 
                            (data[2]  as Self::DataType) << 104 |
                            (data[3]  as Self::DataType) << 96  |
                            (data[4]  as Self::DataType) << 88  |
                            (data[5]  as Self::DataType) << 80  | 
                            (data[6]  as Self::DataType) << 72  |
                            (data[7]  as Self::DataType) << 64  |
                            (data[8]  as Self::DataType) << 56  |
                            (data[9]  as Self::DataType) << 48  | 
                            (data[10] as Self::DataType) << 40  |
                            (data[11] as Self::DataType) << 32  |
                            (data[12] as Self::DataType) << 24  |
                            (data[13] as Self::DataType) << 16  | 
                            (data[14] as Self::DataType) << 8   |
                            (data[15] as Self::DataType) << 0   ;
        Ok(Self::DataType::from_le(result))
    }
}

/*

        Floating Point Numbers

*/

/// Holstable for f32
impl Holstable for f32 {

    type DataType = f32;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        Ok(Vec::from(self.to_le_bytes()))
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 4 {
            return Err(HolsterError::InsufficientData);
        }

        let arr_data : [u8; 4] = match data.as_slice().try_into() {
            Ok(d) => d,
            Err(_) => {
                return Err(HolsterError::FloatFromSliceFailure);
            }
        };

        let result : Self::DataType = Self::DataType::from_le_bytes(arr_data);
        Ok(result)
    }
}

/// Holstable for f64
impl Holstable for f64 {

    type DataType = f64;

    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        Ok(Vec::from(self.to_le_bytes()))
    }

    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError>{

        if data.len() != 8 {
            return Err(HolsterError::InsufficientData);
        }

        let arr_data : [u8; 8] = match data.as_slice().try_into() {
            Ok(d) => d,
            Err(_) => {
                return Err(HolsterError::FloatFromSliceFailure);
            }
        };

        let result : Self::DataType = Self::DataType::from_le_bytes(arr_data);
        Ok(result)
    }
}