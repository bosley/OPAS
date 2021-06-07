extern crate holster;

use holster::Holster;   // The actual holster object

fn main() {

    // Create the holster
    let mut simple_holster = Holster::new();

    // Make some random x
    let x = 9032;

    // Store x
    match simple_holster.holster(&x) {
        Ok(_) => {}
        Err(e) => {
            panic!("Failed to holster item : {:?}", e);
        }
    }

    // Get it and call it y
    let y = match simple_holster.un_holster::<i16>() {
        Ok(val) => val,
        Err(e)  => {
            panic!("Failed to un holster item : {:?}", e);
        }
    };

    // Ensure its the same thing
    assert_eq!(x, y);

    // Now we'll use the same holster to store a string
    let my_string = String::from("This is a string that we are going to holster!");

    // Store the string
    match simple_holster.holster(&my_string) {
        Ok(_) => {}
        Err(e) => {
            panic!("Failed to holster item : {:?}", e);
        }
    }

    // Get the string back out
    let my_string_out = match simple_holster.un_holster::<String>() {
        Ok(val) => val,
        Err(e)  => {
            panic!("Failed to un holster item : {:?}", e);
        }
    };

    // Ensure the strings are the same
    assert_eq!(my_string, my_string_out);

    // Indicate that everything worked just fine!
    println!("Everything worked great!");
}