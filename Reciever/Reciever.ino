//-----------------------------includes----------------------------
 #include <Servo.h>        //Servo library
//-----------------------------Globals-----------------------------
int data;
Servo servo;      
int servoPin = 9;
const unsigned char Passive_buzzer = 5;
//-----------------------Dunction Declartion-----------------
void onServo();
void onBuzzer();
void initializeSerial();
void initializeServo();
void initializeBuzzer();


void setup() {
  initializeSerial();
  initializeServo();
  initializeBuzzer();
}

void loop() {
  data = Serial.read(); //Read the serial data and store it
  Serial.print(data);
  if (data == 1){
    onServo();
  }
  else if(data == 2){
    onBuzzer();
  }
  delay(1000);
}

void onServo(){
  servo.write(0); 
  delay(1000); 
  servo.write(90); 
  delay(1000);
}

void onBuzzer(){
  tone(Passive_buzzer, 523) ; //DO note 523 Hz
  delay (1000); 
  tone(Passive_buzzer, 587) ; //RE note ...
  delay (1000); 
  tone(Passive_buzzer, 659) ; //MI note ...
  delay (1000); 
  tone(Passive_buzzer, 783) ; //FA note ...
  delay (1000); 
  tone(Passive_buzzer, 880) ; //SOL note ...
  delay (1000); 
  tone(Passive_buzzer, 987) ; //LA note ...
  delay (1000); 
  tone(Passive_buzzer, 1046) ; // SI note ...
  delay (1000); 
  noTone(Passive_buzzer) ; //Turn off the pin attached to the tone()
}

void initializeBuzzer(){
  pinMode (Passive_buzzer,OUTPUT) ;
}

void initializeServo(){
  servo.attach(9);
  servo.write(0);
  delay(2000);
}

void initializeSerial(){
  Serial.begin(9600);
}
