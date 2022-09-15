// Writing to the HTML tree
document.write("I am learning JavaScript<br>");

// Declaring and defining variables then displaying on console 
let x,y;
x=5; y=6;
console.log(x + y);
// Prompting for password until number is given and then displaying on screen
let password = Number(prompt("Enter your password: "));

while (Number.isNaN(password)){
    password = Number(prompt("Enter your password: "));
}
document.write("The password entered: " + password);

// An array of integers
const array = [1,2,3,4];
console.log(array);

// Creating an object with 3 properties & 3 methods
const person = {
    firstName:"Zero", lastName:"Nelson", age:20,
    fullName: function(){
        return this.firstName + " " + this.lastName;
    } 
};
console.log(person.fullName());

//Gets the type of the variable --- variables are dynamic;
console.log(typeof(x));
x = "Harry Potter";
console.log(typeof(x));

// Using JS functions 
function add(x,y){
    return x + y;
}
// X = "Harry Potter" + Y = 6 = Harry Potter6
let z = add(x,y);
document.write("<br>" + z);

function displayDate(){
    innerHTML = Date();
}

let text="Jacayla Nelson";
text = text.replace("Jacayla", "Zero");
console.log(text);
text = text.toLowerCase();
console.log(text);

// To set precision 
x = 10.245356;
x.toFixed(2);
console.log(x.toFixed(2));
console.log(x.toPrecision(4));

// Parsing strings for integer or float 
x = " 0 Nelson";
console.log(parseInt(x));
