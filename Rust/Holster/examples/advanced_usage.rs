extern crate holster;

use holster::Holster;       // The actual holster object
use holster::Holstable;     // The trait that allows us to make something holstable
use holster::HolsterError;  // Errors in case we want to throw one

// Create a struct that has some stuff that we want to keep around
//      Everything is made public for this example, but it is not required
//      for Holster
struct MyItem {

    pub data: i32,
    pub also_data: String
}

// Basic 'new' function for our struct
impl MyItem {
    fn new() -> Self {
        Self {
            data: 0,
            also_data: String::from("This is the default string data")
        }
    }
}

// Implementation of Hostable for the struct that we made so we can store
// it in a Holster object
impl Holstable for MyItem {

    // Set the type
    type DataType = MyItem;

    // Specify the means by which the encoder will work
    fn holster_encode(&self) -> Result<Vec<u8>, HolsterError> {

        // Load the string from bytes
        let mut result = Vec::from(self.also_data.as_bytes());

        // Manually load in the integer
        result.push((self.data >> 24) as u8);
        result.push((self.data >> 16) as u8);
        result.push((self.data >> 8)  as u8);
        result.push((self.data >> 0)  as u8);

        // Return the data
        Ok(result)
    }

    // Specify the means by which the decoder will work
    fn holster_decode(data: &Vec<u8>) -> Result<Self::DataType, HolsterError> {

        // We create a temp and an m_data as we don't want to modify the incoming data
        let mut temp : Vec<u8> = Vec::new();
        let mut m_data = data.clone();

        // Pop off the integer data
        temp.push(m_data.pop().unwrap());
        temp.push(m_data.pop().unwrap());
        temp.push(m_data.pop().unwrap());
        temp.push(m_data.pop().unwrap());

        // Load the string from the remaining parts
        let str_data : String = match String::from_utf8(m_data.clone()) {
            Ok(r) =>  r,
            Err(_) => return Err(HolsterError::FromUtf8Failure)
        };

        // Splice together the integer from the data that we removed earlier
        let int_data : i32 = 
                            (temp[3] as i32) << 24 |
                            (temp[2] as i32) << 16 | 
                            (temp[1] as i32) << 8  |
                            (temp[0] as i32) << 0;

        // Construct and return the MyItem
        Ok(MyItem{
            data: int_data,
            also_data: str_data
        })
    }
}

//  Main
//
fn main() {

    // Create an item
    let mut my_item : MyItem = MyItem::new();

    // Set some special data 
    my_item.data = 42;
    my_item.also_data = String::from("This is not the default string, this is a string I set !");

    // Create a simple holster to take the data in
    let mut simple_holster = Holster::new();

    // Store the thing
    match simple_holster.holster(&my_item) {
        Ok(_) => {}
        Err(e) => {
            panic!("Failed to holster item : {:?}", e);
        }
    }

    // Take it out again
    let my_item_out = match simple_holster.un_holster::<MyItem>() {
        Ok(val) => val,
        Err(e)  => {
            panic!("Failed to un holster item : {:?}", e);
        }
    };

    // Check to ensure that the data out is the same as the data in
    assert_eq!(my_item.data,      my_item_out.data);
    assert_eq!(my_item.also_data, my_item_out.also_data);

    // Indicate that everything worked just fine!
    println!("Everything worked great!");
}