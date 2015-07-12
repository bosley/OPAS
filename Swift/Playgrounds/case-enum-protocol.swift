

enum DaysOfTheWeek: Int
{
    case Monday = 0
    case Tuesday = 1
    case Wednesday = 2
    case Thursday = 3
    case Friday = 4
    case Saturday = 5
    case Sunday = 6
    
    func getInfo() -> String
    {
        switch self
        {
        case .Monday:
            
            return "Monday"
            
        case .Tuesday:
            
            return "Tuesday"
            
        case .Wednesday:
            
            return "Wednesday"
            
        case .Thursday:
            
            return "Thursday"
            
        case .Friday:
            
            return "Friday"
            
        case .Saturday:
            
            return "Saturday"
            
        case .Sunday:
            
            return "Sunday"
            
        default:
            
            return String(self.toRaw())
            
        }
    }
}

// Accessing the Enum Value, and its Raw data
// ; are optional!
let Saturday = DaysOfTheWeek.Saturday;
let SaturdayRawValue = Saturday.toRaw();

// Accessing the function getInfo() inside enum
let SaturdayInfo = Saturday.getInfo();

// ---------------------------------------------------------------------

// As the swift tutorial points out, it is important to remember that
// Classes are passed by reference, and structures copied as they are passed
// With that said, here is a structure implementing a protocol

protocol DescriptionProtocol
{
    // Giving description var { get } allows it to get, not set.
    // Set will take place during initialization
    
    var description: String { get }
    
    // If set, isn't set a mutating function can be added to mutate 
    // the protocol var
    mutating func adjust()

    // The body of the adjust() function can not be stored in the protocol
    // The body must be used in the struct/class that the protocol is used in
}


struct stuctThatNeedsDescription: DescriptionProtocol
{
    // Need to set description, and adjust() body to conform to 
    // the declaired protocol
    var description: String = " The Initial Description "
    
    mutating func adjust()
    {
        description += " ( Changed ) "
    }
}

// Set structure, and get initial description 
// Adjust(), and get new description

var a = stuctThatNeedsDescription()
var aDesc = a.description
a.adjust()
aDesc = a.description

// Extension used here with the Protocol made earlier on the Int data type.

extension Int: DescriptionProtocol
{
    // Extensions can not store variables, so func it.

    var description: String
    {
        return "The Number : \(self)"
    }

    mutating func adjust()
    {
        self += 42
    }

}


let newInt = 7

newInt.description

// ---------------------------------------------------------------------













