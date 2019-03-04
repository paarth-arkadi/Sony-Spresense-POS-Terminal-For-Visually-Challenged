/* This program is used to test the ldr_detect function */
#define ldr1 A0  // ldr on the inner left side of the headphone

void setup()
{
    Serial.begin(115200); // Here the baud rate is set to 115200. It can be varied.
    pinMode(ldr1,INPUT); // Declares ldr pins at input
}

void loop()
{
   bool check_headphone = ldr_detect(300); // Check headphone stores the value whether headphones are put on or not
   if(check_headphone)
   {
       //Serial.println("Headphones are put on hence the function is working"); // Uncomment this line for testing
   }
   else
   {
       //Serial.pirntln("Headphones are not put on but the function is working"); // Uncomment this line for testing
   }
   
}

bool ldr_detect(int light_threshold){  
    //Pass the value of light_threshold which it should detect that the headphones are put on (0-1023). Generally the value is 500
    /* This function is used to detect whether the headphone is put on or not */         
    int ldr1_val = analogRead(ldr1);
    if(ldr1_val<=light_threshold)
    {
        //Serial.println("The headphones are put on by the user"); // Uncomment this line for testing
        return true;
    }
    else
    {
        //Serial.println("The headphones are not put on by the user"); // Uncomment this line for testing
        return false;
    }
}