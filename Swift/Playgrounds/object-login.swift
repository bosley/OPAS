// Playground - noun: a place where people can play


// Dictionaries for user login information
var userCodes =
[
    0: "Admin",
    1: "Power User",
    2: "Restricted User",
    3: "Temporary User"
];

var userPasswords =
[
    "Josh": "awesome_pass",
    "Guest": "lame_pass1",
    "Admin": "Admin_Awesome_#!"
];

var userCodeAssignment =
[
    "Josh" : 1,
    "Guest": 2,
    "Admin": 3
];

// Class structures for login, and for the user
class UserClass
{
    let userName: String;
    let userType: String;

    // Constuctor
    init(name: String, userCode: Int)
    {
        userName = name;
        userType = userCodes[ userCode ]!;
        
        displayUserName();
    }
    
    // Display Information
    func displayUserName()
    {
        println(userName + " : " + userType);
    }
}

class userLogin
{
    let inputName: String;
    let inputPass: String;
    
    init(userName: String, userPass: String)
    {
        inputName = userName;
        inputPass = userPass; // inputPass = MD5(userPass);
        checkPassword();
    }
    
    func checkPassword()
    {
        if userPasswords[inputName] == inputPass
        {
            println("Correct Password")
            
            var usersId = userCodeAssignment[inputName];
            let userIn = UserClass(name: inputName, userCode: usersId!)
        }
        else if userPasswords[inputName] != inputPass
        {
            println("Incorrect Password")
        }
    }
}

// ============== List of login objects ==============

var logins = [userLogin]()

// Correct Password
logins.append(userLogin(userName: "Josh", userPass: "awesome_pass"));

// Incorrect Password
logins.append(userLogin(userName: "Guest", userPass: "Wrong_Pass"));


