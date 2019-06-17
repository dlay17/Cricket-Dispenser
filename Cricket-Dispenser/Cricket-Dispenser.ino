#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38

#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN 60 
#define Y_DIR_PIN 61 
#define Y_ENABLE_PIN 56 

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#define FAN_PIN            9

#define PS_ON_PIN          12
#define KILL_PIN           -1


#include <Stepper.h>
/*
 * This is the start to the code: Please enter the number of crickets in each motor.
 */
//How many chambers in total and how many steps it takes for a stepper to do a rotation
#define num_of_chambers   8
#define total_steps_for_rotation  800


//How many Crickets are in the 1/3 and 2/4 chambers
#define crickets_in_motor1 5
#define crickets_in_motor2 5

//pin to signal cricket drop
#define cricket_drop         11

//amount for a turn 
const int turn = total_steps_for_rotation/num_of_chambers;

//how many turn have gone by already
int turnCount = 0;

//keeps track of how many steps have been taken by each motor
int totalSteps1 = 0;
int totalSteps2 = 0;

Stepper cricketFeeder = Stepper(200, X_STEP_PIN, X_DIR_PIN);

void setup() {
//initializes the 1/3 chamber's stepper motor
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  digitalWrite(X_ENABLE_PIN    , LOW);
  
//initializes the 2/4 chamber's stepper motor
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  digitalWrite(Y_ENABLE_PIN    , LOW);
  
//initializes the pulse signal for the entire system
  pinMode(cricket_drop, OUTPUT);


   Serial.begin(9600);
   Serial.println("Program Start: enter t to turn the stepper, press r to reset. Press 1 or 2 to increment the respective motors a small amount.");
}





void loop () {
  char input = Serial.read();         //reads serial input for what to do next

  if(input== 't'){                    //for continuation
    if(turnCount == crickets_in_motor1 + crickets_in_motor2){
      resetTask();
    }
    
    if(turnCount>=crickets_in_motor1){
      cricketFeeder = Stepper(200, Y_STEP_PIN, Y_DIR_PIN);
      Serial.print("Stepper 2: ");
    }else{
      cricketFeeder = Stepper(200, X_STEP_PIN, X_DIR_PIN);
      Serial.print("Stepper 1: ");
    }
    //accounts for the slack in the first turn of each motor
    if(turnCount==0){
      cricketFeeder.setSpeed(10);
      cricketFeeder.step(33);
      totalSteps1 += 33;
    }
    if(turnCount==crickets_in_motor1){
      cricketFeeder.setSpeed(10);
      cricketFeeder.step(33);
      totalSteps2+=33;
    }
    
    turnTask();                     
    sendSignal();                     //sends a signal for the main system that a cricket has dropped
    
  }


  if(input == 'r'){                   //reset the motors
    resetTask();
    sendSignal();
  }

  

  if(input == '1'){                   //used to increment first motor
    Serial.println("incrementing motor 1...");
    turnIncrement(1);
    sendSignal();
  }
  if(input == '2'){                   //used to increment second motor
    Serial.println("incrementing motor 2...");
    turnIncrement(2);
    sendSignal();
  }
}

void turnTask () {
      turnCount++;              
      cricketFeeder.setSpeed(10);
      cricketFeeder.step(turn);
     
      if(turnCount>=crickets_in_motor1+1){
        totalSteps2 +=turn;
        Serial.println(turnCount-crickets_in_motor1);
      }else{
        totalSteps1 +=turn;
        Serial.println(turnCount);
      }
      
      
}

void turnIncrement (int stepper) {
      if(stepper == 2){
        cricketFeeder = Stepper(200, Y_STEP_PIN, Y_DIR_PIN);
        totalSteps2 += 10;
      }else{
        cricketFeeder = Stepper(200, X_STEP_PIN, X_DIR_PIN);
        totalSteps1  =+ 10;
      }
      cricketFeeder.setSpeed(10);
      cricketFeeder.step(10);
      
}

void resetTask () {
    Serial.println("Resetting");
    //if on second motor
    if(turnCount>8){
      cricketFeeder.step(-totalSteps2);
      turnCount=8;                                            //if on the second motor, that means that the first motor has already done 8 rotations
      cricketFeeder = Stepper(200, X_STEP_PIN, X_DIR_PIN);    //setting output to the first motor
    }
    //steps for the first motor
    cricketFeeder.step(-totalSteps1 );
    turnCount = 0;
}

void sendSignal (){
  
    digitalWrite(cricket_drop, HIGH);
    delay(200);
    digitalWrite(cricket_drop, LOW);
}
