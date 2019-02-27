#include<Keypad.h>  // Aurduino Library for keypad
#include <SDHCI.h>  // Sony Spresense SD Card Library
#include <Audio.h>  // Sony Spresense Audio Library

SDClass theSD;  // Instance of the class SDClass
AudioClass *theAudio; // Instance of the class AudioClass
File myFile; // Instance of the class File

#define ldr1 A0  // ldr on the inner left side of the headphone
#define ldr2 A1  // ldr on the inner right side of the headphone
#define Password_Length 5  // Define the password length + the null character

bool ErrEnd = false;
char Data[Password_Length]; // Declare the array to store the pin
char Master[Password_Length] = "1234";  // Declare the password *Here it is declared for prototype purposes(Can use API) 
byte data_count = 0, master_count = 0;  
char customKey; // To store the typed character on the keypad
const byte ROWS = 4;  // Declare the number of rows for the keypad
const byte COLS = 4;  // Declare the number of columns for the keypad
byte rowPins[ROWS] = {8, 7, 6,5};  // Delcare the pins to which the corresponding row is attached on the Spresense board
byte colPins[COLS] = { 4, 3, 2};   // Declare the pins to which the corresponding column is attached on the Spresense board
// Here I have used a 4 * 3 keypad hence there are 7 keys
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
/* Delaring the amount to be paid by the user */
/* For prototype purpose I am defining an amount */
/* In the real world it can be fetched from API or software the seller is using*/
int pylen =4; // The length of the array in which the amount for payemnt is stored.
int payment_amt = 100; // Amount to be paid by the user to complete his payment.
int val = 0; // This is used in the program to store the value of the numpad function
bool verified = false; // Store the value here if the pin is verified

/*Delcaration of audio_attention_cb funtion */
static void audio_attention_cb(const ErrorAttentionParam *atprm){
    puts("Attention!");
    if (atprm->error_code >= AS_ATTENTION_CODE_WARNING){
        ErrEnd = true;
    }
}

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); // Create instance of the class keypad from the Keypad.h library

void setup(){
    Serial.begin(115200); // Here the baud rate is set to 115200. It can be varied.
    pinMode(ldr1,INPUT); // Declares ldr pins at input
    pinMode(ldr2,INPUT);
    pinMode(LED0,OUTPUT);  // LED0 represents the led on the Spresense board
}

void loop()
{
    bool check_headphone = ldr_detect(500); // Check headphone stores the value whether headphones are put on or not
    while(check_headphone){
        //Serial.println("Headphones are put on hence the function is working") // Uncomment this line for testing
        /* The entire process runs under this while loop*/
        play_sound("headph.mp3",40000) // Plays "headphones are put on perfectly do not remove untill the transaction is over"
        /*** Add "center.mp3" and "brail.mp3" or integrate them into headph.mp3*/
        while(val!=1){
            delay(3000); // This wait is added so that there is a pause before playing the mp3 again
            /*** Add "press.mp3" or "Press hash '#' to continue or press asterisk to go back"*/ 
            val = numpad(); 
            if(val==1){
                val = 0;
                /*** Continue with the next process */
                /*** Add "do you want to make a payemnt of xxxx dollars" */
                while(val!=1){
                    delay(3000); // This wait is added so that there is a pause before playing the mp3 again
                    /*** Add "press.mp3" or "Press hash '#' to continue or press asterisk to go back"*/ 
                    val = numpad();
                    if(val==1){
                        val = 0;
                        /*** Continue the next process */
                        /*** Add "Enter your pin "*/
                        check_headphone = ldr_detect(500); // Take a precautionary check whther headphones are put on before entering the pin
                        if(!check_headphone){
                            break; // Break the whole transaction process if headphones are not on
                        }
                        verified = check_pin();
                        if(verified){
                            /*** Add "your transaction is being processed" ****/
                            amt_balance = amt_balance - payment_amt;
                            /*** Add "Add your transaction is completed Thank you" ***/
                            /*** Add "you may remove the headphones" */
                            while(1){  // Wait untill the user removes the headphones
                                check_headphone = ldr_detect(500); // Take a precautionary check whther headphones are put on before entering the pin
                                if(!check_headphone){
                                    break; // Break the whole transaction process if headphones are not on
                                }
                            }
                            break;
                        }
                        else
                        {
                            break; // Break the whole transaction process if the pin is not verified
                        }
                        
                    }
                }
            }                
        }
        
        
    }
    // ***Do not add any time consuming program here: It can cause delay in detection of headphones
    //Serial.pirntln("Headphones are not put on but the function is working") // Uncomment this line for testing   
}

bool ldr_detect(int light_threshold){  
    //Pass the value of light_threshold which it should detect that the headphones are put on (0-1023). 
    //Generally the value is 500
    /* This function is used to detect whether the headphone is put on or not */         
    int ldr1_val = analogRead(ldr1);
    int ldr2_val = analogRead(ldr2); 
    if(ldr1_val>=light_threshold && ldr2_val>= light_threshold){
        //Serial.println("The headphones are put on by the user"); // Uncomment this line for testing
        return true;
    }
    else{
        //Serial.println("The headphones are not put on by the user"); // Uncomment this line for testing
        return false;
    }
}

/* This function is created for the ease of playing the audio file*/
/* It takes the audio file name and the playtime as the parameters */
void play_audio(String filename,int delay_time) // Generally delay time is 40000
{  
    /* Setup the audio player */
    theAudio = AudioClass::getInstance();
    theAudio->begin(audio_attention_cb);
    puts("initialization Audio Library");
    theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
    theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);
    err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/spif/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);
    if (err != AUDIOLIB_ECODE_OK){
        printf("Player0 initialize error\n");
        exit(1);
    }
    myFile = theSD.open(filename); // Takes the file name as the function parameter
    if (!myFile){
        printf("File open error\n");
        exit(1);
    }
    printf("Open! %d\n",myFile);
    err = theAudio->writeFrames(AudioClass::Player0, myFile);
    if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)){
        printf("File Read Error! =%d\n",err);
        myFile.close();
        exit(1);
    }
    puts("Play!");
    theAudio->setVolume(-160);
    theAudio->startPlayer(AudioClass::Player0);
    while(1){
        puts("loop!!");
        int err = theAudio->writeFrames(AudioClass::Player0, myFile);
        if (err == AUDIOLIB_ECODE_FILEEND){
            printf("Main player File End!\n");
            break;
        }
        if (err){
            printf("Main player error code: %d\n", err);
               sleep(1);
            theAudio->stopPlayer(AudioClass::Player0);
            myFile.close();
            break; // Break the while loop
            exit(1);  
        }
        if (ErrEnd){
            printf("Error End\n");
               sleep(1);
            theAudio->stopPlayer(AudioClass::Player0);
            myFile.close();
            break; // Break the while loop
            exit(1);  
        }
        usleep(40000); 
    }
}

/* This function blinks the LED0 on the spresense board and is used to indicate the excecution of certain programs*/
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

/* The check_pin function returns a boolean value and does not take any parameters*/
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

/* The numpad function returns a integer value and does not take any parameters*/
/* It 1 true if # is pressed while it returns 2 if * is pressed and it returns 3 if invalid key is pressed*/
int numpad(){
    int key_return = 0; // Used to return which key is pressed
    while(1){
        customKey = customKeypad.getKey();  // the getKey() method returns the character pressed on the keypad
        if (customKey=='#'){
            //Serial.println("Continue"); // Uncomment this line for testing
            key_return = 1;
            break;
        }
        else if(customKey=='*'){
            //Serial.println("Repeat the previous sentence");  // Uncomment this line for testing
            key_return = 2;
            break;
        }
        else{
            //Serial.println("Invalid key pressed");  // Uncomment this line for testing
        }
    }
    return key_return;
}
