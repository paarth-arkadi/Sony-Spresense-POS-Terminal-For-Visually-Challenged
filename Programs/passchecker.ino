/* This program is used to check the working of the fucntion check_pin */
/* The check_pin function returns a boolean value and does not take any parameters*/
#include<Keypad.h>  // Aurduino Library for keypad

#define Password_Length 5  // Define the password length + the null character

char Data[Password_Length]; // Declare the array to store the pin
char Master[Password_Length] = "1234";  // Declare the password *Here it is declared for prototype purposes(Can use API) 
byte data_count = 0, master_count = 0;  
char customKey; // To store the typed character on the keypad

const byte ROWS = 4;  // Declare the number of rows for the keypad
const byte COLS = 4;  // Declare the number of columns for the keypad

// Here I have used a 4 * 3 keypad hence there are 7 keys
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {8, 7, 6,5};  // Delcare the pins to which the corresponding row is attached on the Spresense board
byte colPins[COLS] = { 4, 3, 2};   // Declare the pins to which the corresponding column is attached on the Spresense board

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); // Create instance of the class keypad from the Keypad.h library

void setup(){
  Serial.begin(115200);  // Declare the baud rate , Here I am using 115200
  pinMode(LED0,OUTPUT);  // LED0 represents the led on the Spresense board
}

void loop(){
  check_pin();
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}

void blink_led0(int choice,int time_delay){
  while(1)
  {
    digitalWrite(LED0,HIGH);
    delay(time_delay);
    digitalWrite(LED0,LOW);
    delay(time_delay);
    if(choice=0)
    {
      break;
    }
  }
  return;
}

bool check_pin(){
  bool pass_check =false; // False if the password is incorrect
  while(1){
    //Serial.print("Enter Password:");  //Uncomment the line for testing 
    customKey = customKeypad.getKey();  // the getKey() method returns the character pressed on the keypad
    if (customKey){
      Data[data_count] = customKey;  
      data_count++; 
    }
    if(data_count == Password_Length-1){
      /* If the number of digits entered matches the number of digits in the set password */
      if(!strcmp(Data, Master)){
        /* If the entered password matches the set password */
        //Serial.print("Correct"); // Uncomment the line for testing
        clearData(); // Clears the data in the array that holds the user typed pin
        /* If the password matches then enter this loop */
        //Serial.println("Password Matching"); // Uncomment the line for testing
        pass_check = true;
        break;
      }
      else{
      //Serial.print("Incorrect");  // Uncomment the line for testing
      delay(1000); // This delay is used to ensure proper detection of the keys pressed
      }
    }
  }
  if(pass_check){
    return true;
  }
  else{
    return false;
  }
}
