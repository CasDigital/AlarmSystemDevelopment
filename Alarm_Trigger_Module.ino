#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // Configure the UART pins to be used between the SIM808 and Arduino


String msg = String(""); //message buffer
int SmsContentFlag = 0; //flag used when the shield message has an sms

#define CH2 6   // Connect Digital Pin 6 on Arduino to CH1 on Relay Module
#define Buzzer 10 //Connect Digital Pin 10 on Arduino to Buzzer alarm
#define Powerkey 9 //Assign pin 9 as a automatic power key for the GSM Module

void setup() {

   // the GPRS baud rate
   mySerial.begin(19200);               
   Serial.begin(19200);            
    
    //initialis relay pins
   pinMode(CH2, OUTPUT);
   digitalWrite( CH2 , LOW );

   //initialise  buzzer pins
  // pinMode(Buzzer, OUTPUT);
  // digitalWrite( Buzzer, LOW );


  //PowerUp the GSM module at startup
  // PowerGSM();

}

void loop() 
{
  //initiate reading the sms from the SIM808 module
     char SerialInByte;
 
    if(Serial.available())
    {
       mySerial.print((unsigned char)Serial.read());
     }  
    else  if(mySerial.available())
    {
        char SerialInByte;
        SerialInByte = (unsigned char)mySerial.read();
 
        // Print the output on the serial monitor
   
        Serial.print( SerialInByte );
    
        // Process the message if you detect <CR> at the end
       
        if( SerialInByte == 13 ){
          // Store the message in a buffer
        
          ProcessGprsMsg();
         }
         if( SerialInByte == 10 ){
            //Skip the line feed
         }
         else {
           // store the current character in the message string buffer
 
           msg += String(SerialInByte);
         }
     }    
}

/*
//PowerUp the GSM module automatically
void PowerGSM(void)
{
  digitalWrite(Powerkey, LOW);
  delay(1000);
  digitalWrite(Powerkey, HIGH);
}*/


//Look for the contents of the sms
void ProcessSms( String sms ){
  Serial.print( "ProcessSms for [" );
  Serial.print( sms );
  Serial.println( "]" );

  //trigger the conrresponding action after processing the sms
   if( sms.indexOf("trigger") >= 0 ){
    digitalWrite( CH2, HIGH );
    digitalWrite( Buzzer, HIGH );
    Serial.println( "RELAY AND BUZZER ON" );
    return;
  }
  if( sms.indexOf("notrigger") >= 0 ){
    digitalWrite( CH2, LOW );
     digitalWrite( Buzzer, LOW );
    Serial.println( "RELAY AND BUZZER OFF" );
    return;
  }
 
}
 
// Request Text Mode for SMS messaging
void GprsTextModeSMS(){
  mySerial.println( "AT+CMGF=1" );
}
 
void GprsReadSmsStore( String SmsStorePos ){
  mySerial.print( "AT+CMGR=" );
  mySerial.println( SmsStorePos );
}
 
//Clear the message buffer for the next read

void ClearGprsMsg(){
  msg = "";
}


//Look for contents of a GPRS message
void ProcessGprsMsg() {
  Serial.println("");
  Serial.print( "GPRS Message: [" );
  Serial.print( msg );
  Serial.println( "]" );
 
 
  if( msg.indexOf( "Call Ready" ) >= 0 ){
     Serial.println( "*** GPRS Shield registered on Mobile Network ***" );
     GprsTextModeSMS();
  }
 
  // unsolicited message received when getting a SMS message
  
  if( msg.indexOf( "+CMTI" ) >= 0 ){
     Serial.println( "*** SMS Received ***" );
     // Look for the coma in the full message
     // In the sample, the SMS is stored at position 6
     int iPos = msg.indexOf( "," );
     String SmsStorePos = msg.substring( iPos+1 );
     Serial.print( "SMS stored at " );
     Serial.println( SmsStorePos );
 
     // Ask to read the SMS store
     GprsReadSmsStore( SmsStorePos );
  }
 
  // SMS store read msg 
  if( msg.indexOf( "+CMGR:" ) >= 0 ){
    // Next message will contains the BODY of SMS
    SmsContentFlag = 1;
    // set buffer to ""
    ClearGprsMsg();
    return;
  }
 

  //spit out of the serial port the contents of the sms for verification

  if( SmsContentFlag == 1 ){
    Serial.println( "*** SMS MESSAGE CONTENT ***" );
    Serial.println( msg );
    Serial.println( "*** END OF SMS MESSAGE ***" );
    ProcessSms( msg );
  }

  // set buffer to ""
  ClearGprsMsg();
  
  // Always clear the flag to ensure that this process can be done again
  SmsContentFlag = 0; 
}


