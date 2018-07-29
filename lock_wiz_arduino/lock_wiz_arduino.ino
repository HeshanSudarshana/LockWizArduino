#include<ESP8266WiFi.h>

// WiFi Definitions
const char* ssid = "Esp8266TestNet";
const char* password = "Esp8266Test"; // has to be longer than 7 chars

//const char* ssid = "LockWizWifi";
//const char* password = "lockWIZ@123"; // has to be longer than 7 chars

const char* value = "";

int ledPinSuccess = D0; //green colour LED
int ledPinFailure = D1; //red colour LED 
int buzzerPin = D2; //buzzer
int lockPin = D3; //lock
int sensorPin = 13; //sensor
int lockValue = 0; //the state of the lock (0 for off / 1 for on)
int sensorState = 0; //sensor state
int intruder = 0; //intruder alert

WiFiServer server(80);

void setup() {

   Serial.begin(115200);
   delay(10);
   
   //output pin def
   pinMode(ledPinSuccess, OUTPUT);
   pinMode(ledPinFailure, OUTPUT);
   pinMode(buzzerPin, OUTPUT);
   pinMode(lockPin, OUTPUT);

   //input pin def
   pinMode(sensorPin, INPUT_PULLUP);
   
   //digitalWrite(ledPin, HIGH); // turn on

   //setting up wifi
   WiFi.mode(WIFI_AP);
   WiFi.softAP(ssid, password, 1, 1);

   server.begin();
}

void loop() {
  
  // Check of client has connected
  WiFiClient client = server.available();
  sensorState = digitalRead(sensorPin);
  Serial.println("started");
  
  if(intruder == 1) {
      digitalWrite(ledPinSuccess, LOW); //success led off
      digitalWrite(ledPinFailure, HIGH); //failure led on
      buzzerOn(); //buzzer on
      Serial.println("intruder detected!!");
  }

  /*if((!client) && (sensorState == 0)) {
    return;
  }*/

  if(!client){
    
    sensorState = digitalRead(sensorPin);
    Serial.println(sensorState);
    
    if(sensorState == HIGH){
      Serial.println("this runs");
      if(lockValue == 1) {
        //if the door open and lock open
        digitalWrite(ledPinSuccess, HIGH); //success led on
        digitalWrite(ledPinFailure, LOW); //failure led off
        buzzerOff(); //buzzer off
        Serial.println("door open and lock open");
      }
      else {
        //if the door open and lock close
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        Serial.println("door open and lock close");
      }
    }
    else if(sensorState == LOW)
    {
      Serial.println("this runs here");
      if(lockValue == 0) {
        //if the door close and lock close
        digitalWrite(ledPinSuccess, HIGH); //success led on
        digitalWrite(ledPinFailure, LOW); //failure led off
        buzzerOff(); //buzzer off
        Serial.println("door close and lock close");
      }
      else {
        //if the door close and lock open
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        Serial.println("door open and lock open");
      }
    }
  }
  
  else {
    
    // Read the request line
    String request = client.readStringUntil('\r');
    Serial.println("request : ");
    Serial.println(request);
    client.flush();
    
    // Match request
    if(request.indexOf("/lock/auth") != -1) {
        //if the auth request comes
        intruder = 0;
        digitalWrite(ledPinSuccess, HIGH); //success led on
        digitalWrite(ledPinFailure, LOW); //failure led off
        buzzerOff(); //buzzer off
        digitalWrite(lockPin, HIGH); //lock open
        lockValue = 1;
        Serial.println("door is unlocked");
    }
    else if(request.indexOf("/lock/invalid") != -1) {
        //if the invalid request comes
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        Serial.println("door is unlocked");
    }
    else if(request.indexOf("/lock/intruder") != -1) {
        //if the intruder request comes
        intruder = 1;
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        digitalWrite(lockPin, LOW); //lock close
        Serial.println("intruder is detected");
    }
    else if(request.indexOf("/lock/lock") != -1) {
        //if the lock request comes
        sensorState = digitalRead(sensorPin);
        if(sensorState == 1) {
          digitalWrite(ledPinSuccess, LOW); //success led off
          digitalWrite(ledPinFailure, HIGH); //failure led on
          buzzerOn(); //buzzer on
          Serial.println("cant lock! door is open");
        }
        else {
          digitalWrite(ledPinSuccess, HIGH); //success led on
          digitalWrite(ledPinFailure, LOW); //failure led off
          buzzerOff(); //buzzer on
          lockValue = 0;
          digitalWrite(lockPin, LOW); //lock close
          Serial.println("Locked successfully!");
        }
        
    }

    sensorState = digitalRead(sensorPin);
    Serial.println(sensorState);
    
    if(sensorState == HIGH){
      Serial.println("this runs");
      if(lockValue == 1) {
        //if the door open and lock open
        digitalWrite(ledPinSuccess, HIGH); //success led on
        digitalWrite(ledPinFailure, LOW); //failure led off
        buzzerOff(); //buzzer off
        Serial.println("door open and lock open");
      }
      else {
        //if the door open and lock close
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        Serial.println("door open and lock close");
      }
    }
    else if(sensorState == LOW)
    {
      Serial.println("this runs here");
      if(lockValue == 0) {
        //if the door close and lock close
        digitalWrite(ledPinSuccess, HIGH); //success led on
        digitalWrite(ledPinFailure, LOW); //failure led off
        buzzerOff(); //buzzer off
        Serial.println("door close and lock close");
      }
      else {
        //if the door close and lock open
        digitalWrite(ledPinSuccess, LOW); //success led off
        digitalWrite(ledPinFailure, HIGH); //failure led on
        buzzerOn(); //buzzer on
        Serial.println("door open and lock open");
      }
    }
    
    client.flush();
     
    // JSON response
    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: application/json\r\n\r\n";
    s += "{\"data\":{\"message\":\"success\",\"value\":\"";
    s += value;
    s += "\"}}\r\n";
    s += "\n";
  
    // Send the response to the client
    client.print(s);
    delay(1);
    Serial.println("Client disconnected");
  
    // The client will actually be disconnected when the function returns and the client object is destroyed
  }
  
}

void buzzerOn(){
  tone(buzzerPin, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzerPin);     // Stop sound...
  delay(1000);        // ...for 1sec
}

void buzzerOff(){
  noTone(buzzerPin);     // Stop sound...
}

