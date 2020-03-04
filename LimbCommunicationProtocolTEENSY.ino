//teensy code
/* NYU RDT
 * Limb Control code
 * 8 - bit string will be sent from the topic that the esp is subscribed to, and the
 * esp will send to the teensy
 * 0 = drum commands
 *    - will be a number 0-4
 *    - 0 - not moving
        1 - rotating forward slow/medium
        2 - rotating forward medium high
        3 - rotating backwards slow/medium
        4 - rotating backwards medium/high
 * 1-3 = arms char
 *    - first char represents whether drum is moving from configuration to configuration
 *       0 - not moving
 *       1 - moving from start to drive
 *       2 - moving from drive to start
 *     - 2nd and 3rd character will represent a number 0-99 (percentage of voltage output)
 *        - 0 - no power
 *        - 99 - max power
 * 4-6 = linear actuator
 *     - 1st character will represent whether the drum is moving up or down
 *        - 0 = drum is not moving 
 *        - 1 = drum is moving down (digging config)
 *        - 2 = drum is moving up (transport config)
 *     - 2nd and 3rd character will represent a number 0-99 (percentage of voltage output)
 *        - 0 - no power
 *        - 99 - max power
 * 7 = door 
 *    - 0 = door not moving
 *    - 1 = close the door
 *    - 2 = open the door
 */

 /*
  * PWM pins are 3, 5, 6, 9, 10, 11 on an arduino
  * teensy has 12 possible PWM outputs: 3, 4, 5, 6, 9, 10, 20, 21, 22, 23, 25, 32
  * 
  * 
  * Pin assignments are as follows: 
  * drum - pin 3
  * armOne - pin 4
  * armTwo - pin 5
  * lin - pin 6
  * door - pin 9
  */
  
//servo library to send PWM signals to motor controllers
#include <Servo.h>


//prototypes for limb actuation functions
void drum(char drumCommand);
void arm(char armCommand[3]);
void lin(char linCommand[3]);
void door(char doorCommand);

//creating servo objects for each corresponding motor controller
Servo drumControl;
Servo armControlOne; //front arms
Servo armControlTwo; //rear arms
Servo linControl;
Servo doorControl;

//teensy pins to corresponding motor controllers
int drumPin = 3;
int armPinOne = 4;
int armPinTwo = 5;
int linPin = 6;
int doorPin = 9;

int val; //used to map pulse values 

void setup() {
  // put your setup code here, to run once:
  //Attach the motor controller objects to corresponding pins on the teensy(pins 3-6)
  drumControl.attach(drumPin); 
  armControlOne.attach(armPinOne);
  armControlTwo.attach(armPinTwo);
  linControl.attach(linPin);
  doorControl.attach(doorPin);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
   * Read the commandUpdate from the ESP and then break down the 
   * command into each respective limb control portion
   * Example, the first character in the commandUpdate string will
   * determine the actuation of the drum.
   */
  String commandUpdate = Serial.readString; //maybe?
  drum(commandUpdate[0]); //actuate the drum
  arm(commandUpdate.substring(1, 3)); //actuate the arms
  lin(commandUpdate.substring(4, 6)); //actuate the linear actuators
  door(commandUpdate[7]); //actuate the door

}


void drum(char drumCommand){
    /*
     * using the SPARK MAX motor controllers and NEO brushless motors
     * motor controllers can be controlled with a 1ms-2ms variable PWM signal
     */
  switch(drumCommand){
    case '0':
      //the drum is not moving
      drumControl.writeMicroseconds(1500);
      delay(15); 
      break;
    case '1':
      //rotate the drum forward at a slow/medium speed
      drumControl.writeMicroseconds(1750);
      delay(15);
      break;
    case '2':
      //rotate the drum forward at a medium/high speed
      drumControl.writeMicroseconds(2000);
      delay(15);
      break; 
    case '3' 
      //rotate the drum backwards at a slow/medium speed
      drumControl.writeMicroseconds(1250);
      delay(15);
      break;
    case '4' 
      //rotate the drum backwards at a medium/high speed
      drumControl.writeMicroseconds(1000);
      delay(15);
  }
  
void arm(char armCommand[3]){
  /*
   * currently receiving a value between 0-180 to actuate angle
   * Might need to change in order to control individual actuation 
   * of the arms
   */
  
  if(int(armCommand) == 0){
    //stop motion of arms
    armControlOne.writeMicroseconds(1500); 
    armControlTwo.writeMicroseconds(1500); 
  }
  else{
      //code to rotate rear motors by angle armCommand
      val = 180 - (int(armCommand)); 
      val = map(val, 0, 180, 1000, 1499); 
      armControlOne.writeMicroseconds(val);
      
      //code to rotate front motors by angle armCommand
      val = (int(armCommand)); 
      val = map(val, 0, 180, 1500, 2000); 
      armControlTwo.writeMicroseconds(val);
      delay(15);  
  }
}


void lin(char linCommand[3]){

   switch(linCommand[0]){
    case '0':
      //drum is not moving 
      linControl.writeMicroseconds(1500);
      delay(15);
      break;
    case '1':
      //drum is moving down into the digging configuration at speed linCommand.substring(1, 2)
      val = 100 - (int(linCommand.substring(1, 2))); //the lower the number, the faster the acceleration
      val = map(val, 0, 99, 1000, 1499); //scale the number to the correct pulse signal
      linControl.writeMicroseconds(val);
      delay(15);
      break;
    case '2':
      //drum is moving up into the starting configuration at speed linCommand.substring(1, 2)
      val = int(linCommand.substring(1, 2)); //the higher the number, the faster the acceleration
      val = map(val, 0, 99, 1000, 1499); //scale the number to the correct pulse signal
      linControl.writeMicroseconds(val);    
      delay(15);
      break;
   } 
}

void door(char doorCommand){
  switch(doorCommand){
    case '0':
      //door is not moving
      doorControl.writeMicroseconds(1500);
      delay(15);
      break;
    case '1':
      //close the door
      val = 100 - (int(doorCommand.substring(1, 2))); //the lower the number, the faster the acceleration
      val = map(val, 0, 99, 1000, 1499); //scale the number to the correct pulse signal
      doorControl.writeMicroseconds(val);
      delay(15);
      break;
    case '2':
      //open the door
      val = int(doorCommand.substring(1, 2)); //the higher the number, the faster the acceleration
      val = map(val, 0, 99, 1000, 1499); //scale the number to the correct pulse signal
      linControl.writeMicroseconds(val);    
      delay(15);
      
      break;
  }  
