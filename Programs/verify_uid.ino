// This program is used to test the function check_card
// It returns 'true' if the Card is verified else 'false' if Card is not verified
// It takes the parameter of the Card Number.
/*
 * 
 * All the resources for this project: http://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */
 
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10   // SDA is connected to pin 10 of arduino
#define RST_PIN 9   // RESET is connected to pin 9 of arduino
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String card_number = "91 47 BE 2F";    // Enter the card number for prototype

void setup() 
{
    Serial.begin(115200);   // Initiate a serial communication
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
    Serial.println("Approximate your card to the reader...");   //Uncomment this line for testing
 
    pinMode(7,OUTPUT); // Send the signal that rfid is verified
}

void loop() 
{       
        digitalWrite(7,LOW);
        bool verified = check_card(card_number);
        if(verified){
           while(1){
            Serial.println("The card is verified");
            digitalWrite(7,HIGH);
           }
        }
        else{
            Serial.println("The card is not verified");
        }
    
} 

bool check_card(String card_number)
{
    int val = 0;    // Stores whether the card is verified or not
    while(1){
        if ( ! mfrc522.PICC_IsNewCardPresent()) {
            // Look for new cards
            continue;
        }
        if ( ! mfrc522.PICC_ReadCardSerial()) {
            // Select one of the cards
            continue;
        }
        //Show UID on serial monitor
        //Serial.print("UID tag :");    //Uncomment this line for testing
        String content= "";
        byte letter;
        for (byte i = 0; i < mfrc522.uid.size; i++) 
        {
            //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); // Uncomment this line for testing
            //Serial.print(mfrc522.uid.uidByte[i], HEX);    //Uncomment this line for testing
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        //Serial.print("Message : ");   // Uncomment this line for testing   
        content.toUpperCase();
        if (content.substring(1) == card_number){
            //Change the UID to give access or use api to fetch it here
            Serial.println("Authorized access");  // Uncomment this line for testing
            delay(3000);
            val = 1;
            break;
        }
        else{
            Serial.println(" Access denied"); // Uncomment this line for testing
            delay(3000);
            val = 0;
            break;
        }
    }
    if(val){
        return true;
    }
    else{
        return false;
    }
}
