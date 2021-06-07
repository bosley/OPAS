extern crate holster;

use holster::*;

use std::{
     u8,
    u16,
    u32,
    u64,
     i8,
    i16,
    i32,
    i64,
    f32
};

const TEST_RANGE : usize = 100;


#[test]
fn test_u8() {

    type CurrentType = u8;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}


#[test]
fn test_u16() {

    type CurrentType = u16;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}

#[test]
fn test_u32() {

    type CurrentType = u32;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}


#[test]
fn test_u64() {

    type CurrentType = u64;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}



#[test]
fn test_i8() {

    type CurrentType = i8;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}


#[test]
fn test_i16() {

    type CurrentType = i16;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}

#[test]
fn test_i32() {

    type CurrentType = i32;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}


#[test]
fn test_i64() {

    type CurrentType = i64;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}

#[test]
fn test_u128() {

    type CurrentType = u128;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}

#[test]
fn test_i128() {

    type CurrentType = i128;

    for x in (CurrentType::MAX - TEST_RANGE as CurrentType) .. CurrentType::MAX {

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);
    }
}


#[test]
fn test_f32() {

    type CurrentType = f32;

    let mut x : CurrentType = CurrentType::MAX - TEST_RANGE as CurrentType;

    let mut counter : u64 = CurrentType::MAX as u64 - TEST_RANGE as u64;

    'float_loop: loop {

        x += 1.0;
        counter += 1;

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);

        if counter == (CurrentType::MAX as u64) {
            break 'float_loop;
        }
    }
}


#[test]
fn test_f64() {

    type CurrentType = f64;

    let mut x : CurrentType = CurrentType::MAX - TEST_RANGE as CurrentType;

    let mut counter : u64 = CurrentType::MAX as u64 - TEST_RANGE as u64;

    'float_loop: loop {

        x += 1.0;
        counter += 1;

        let encoded = match x.holster_encode() {
            Ok(v) => v,
            Err(e) => panic!("Failed to encode : {:?}", e)
        };
    
        let decoded = match CurrentType::holster_decode(&encoded) {
            Ok(v) => v,
            Err(e) => panic!("Failed to decode : {:?}", e)
        };
    
        assert_eq!(x, decoded);

        if counter == (CurrentType::MAX as u64) {
            break 'float_loop;
        }
    }
}


#[test]
fn test_strings() {

    type CurrentType = String;

    let item : String = "This is a test string to see if everything will work at least okay-ly".to_string();

    let encoded = match item.holster_encode() {
        Ok(v) => v,
        Err(e) => panic!("Failed to encode : {:?}", e)
    };

    let decoded = match CurrentType::holster_decode(&encoded) {
        Ok(v) => v,
        Err(e) => panic!("Failed to decode : {:?}", e)
    };

    assert_eq!(item, decoded);
}