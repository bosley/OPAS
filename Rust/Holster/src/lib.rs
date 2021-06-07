mod error;
pub use error::HolsterError;

mod traits;
pub use traits::Holstable;

mod holst;
pub use holst::Holster;

pub mod types;

#[cfg(test)]
mod tests {

    use super::*;

    #[test]
    fn holster_test() {

        let mut local_holster : Holster = Holster::new();

        for x in 0..100 {

            let x = x as u32;

            match local_holster.holster(&x) {
                Ok(_) => {}
                Err(e) => {
                    panic!("Failed to holster : {} | {:?}", x, e)
                }
            }

            let item : Result<u32, HolsterError> = local_holster.un_holster::<u32>();

            assert!(item.is_ok());
        }

    }
}
