const int LED1 = 13;
const int LED2 = 12;
const int LED3 = 11;
int incomingByte = 0;   // init incoming serial data
 
void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(LED1,OUTPUT);
        digitalWrite(LED1,LOW);
        pinMode(LED2,OUTPUT);
        digitalWrite(LED2,LOW);
        pinMode(LED3,OUTPUT);
        digitalWrite(LED3,LOW);
}
 
void loop() {
 
        // send data only when you receive data:
        if (Serial.available() > 0) {
                incomingByte = Serial.read();
                if((incomingByte & 4) == 4){//100との論理和を取る　
                  //3桁目が1ならLED3を点灯
                  digitalWrite(LED3,HIGH);
                }else{
                  digitalWrite(LED3,LOW);
                }
                
                if((incomingByte & 2) == 2){//010との論理和を取る　
                  //3桁目が1ならLED3を点灯
                  digitalWrite(LED2,HIGH);
                }else{
                  digitalWrite(LED2,LOW);
                }
                
                if((incomingByte & 1) == 1){//001との論理和を取る　
                  //3桁目が1ならLED3を点灯
                  digitalWrite(LED1,HIGH);
                }else{
                  digitalWrite(LED1,LOW);
                }
        }
      
        delay(10);
}
