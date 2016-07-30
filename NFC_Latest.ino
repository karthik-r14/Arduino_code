#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

#include <SoftwareSerial.h>
SoftwareSerial mySerial(4,5);//Rx,Tx

const int LDR2pin = A5;
const int LDR1pin = A4;
const int LDR3pin = A3;
const int LDR4pin = A2;

int LDR1value, LDR2value, LDR3value, LDR4value;
int amplifiedValue1, amplifiedValue2, amplifiedValue3, amplifiedValue4;

void setup(void) {
    pinMode(8, OUTPUT);
    pinMode(LDR1pin, INPUT);
    pinMode(LDR2pin, INPUT);
    pinMode(LDR3pin, INPUT);
    pinMode(LDR4pin, INPUT);
    
    Serial.begin(9600);
    //Serial.println("NDEF Reader");
    mySerial.begin(9600);
    nfc.begin();
}

const int DELAY_TIME= 20000;

void loop(void) {
    //Serial.println("99");
    sendControlSignal(99);
    
    LDR1value = analogRead(LDR1pin);
    LDR2value = analogRead(LDR2pin);
    LDR3value = analogRead(LDR3pin);
    LDR4value = analogRead(LDR4pin);
    
    amplifiedValue1 = map(LDR1value, 0, 1023, 0, 2048);
    amplifiedValue2 = map(LDR2value, 0, 1023, 0, 2048);
    amplifiedValue3 = map(LDR3value, 0, 1023, 0, 2048);
    amplifiedValue4 = map(LDR4value, 0, 1023, 0, 2048);
     
    Serial.println("L:" + String(amplifiedValue1));
    Serial.println("R:" + String(amplifiedValue2));
    Serial.println("S:" + String(amplifiedValue3));
    Serial.println("P:" + String(amplifiedValue4));
    //delay(100);
    
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        NdefMessage message = tag.getNdefMessage();
        NdefRecord record = message.getRecord(0);
        
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);
        
        String payloadAsString = ""; // Processes the message as a string vs as a HEX value
        
        //The payload is the the form  : ' 'en2 
        //Serial.println(payloadLength);
        
        for (int c = 3; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
          //Serial.println("Payload");
          //Serial.println(payloadAsString);
        }
        //Serial.println(payloadAsString);
        sendControlSignal(payloadAsString.toInt());
        delay(DELAY_TIME);    
    }
    delay(500);
}

void sendControlSignal(int productId) { 
   int quotient = productId / 256;
   int remainder = productId % 256; 
   
   byte productIdBuffer[2];
   productIdBuffer[0] = quotient;
   productIdBuffer[1] = remainder;
   
   //Serial.println(quotient);
   //Serial.println(remainder);
   mySerial.write(productIdBuffer, 2);
   
   if(productId != 99)
    buzz();
}

void buzz(){
    digitalWrite(8, HIGH);
    delay(200);
    digitalWrite(8, LOW);  
  }  
