//-----------------------------includes----------------------------
 #include <Servo.h>        //Servo library
 #include <SoftwareSerial.h>
 
//-----------------------------Globals-----------------------------
SoftwareSerial ArduinoUno(3,2);
Servo servo;      
int servoPin = 9;
const unsigned char Passive_buzzer = 5;
const int trigPin = 10;
const int echoPin = 11;
long duration;
int distance;
enum e_Task {
  DROP_FOOD = 1,
  MAKE_NOISE,
  DISTANCE
};
e_Task task;

//-----------------------Dunction Declartion-----------------
void onServo();
void onBuzzer();
void onUltrasonic();
void initializeSerial();
void initializeServo();
void initializeBuzzer();
void initializeUltrasonic();
void checkDistance();

void setup() {
  initializeSerial();
  initializeServo();
  initializeBuzzer();
  initializeUltrasonic();
}

void loop() {

  if(ArduinoUno.available()>0){
    task = (e_Task)ArduinoUno.parseInt();
    if (task == DROP_FOOD){
      onServo();
    }
    else if(task == MAKE_NOISE){
      onBuzzer();
    }
    else if(task == DISTANCE){
        onUltrasonic();
    }    
  }
  else{
    checkDistance();
  }

  delay(1000);
}

void checkDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  
  delay(10000); 
  if (distance < 50){
   
    Serial.println(distance);
  }
  else{
    Serial.println(distance);
    ArduinoUno.print(distance);
  }
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
  ArduinoUno.begin(4800);
}

void initializeUltrasonic(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
}

void onUltrasonic(){
  ArduinoUno.print(distance);
}
