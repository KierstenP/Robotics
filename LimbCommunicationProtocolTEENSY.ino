//teensy code
/* NYU RDT
 * Limb Control code
 * 8 - bit string will be sent from the topic that we the esp is subscribed to
 * 0 = drum commands
 *    - will be a number 0-4
 *    - 0 - not moving
        1 - rotating forward slow/medium
        2 - rotating forward medium high
        3 - rotating backwards slow/medium
        4 - rotating backwards medium/high
 * 1-3 = arms char
 *    - will represent a number 0 - 180
 *    - this is an angle corresponding to the front set of wheels
 *    - the "opposite" of this angle will be calculated for the rear set of wheels
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

void drum(char drumCommand);
void arm(char armCommand[3]);
void lin(char linCommand[3]);
void door(char doorCommand);

//creating servo objects for each motor controller
Servo drumControl;
Servo armControlOne; //front arms
Servo armControlTwo; //rear arms
Servo linControl;
Servo doorControl;

//teensy pins to motor controllers
int drumPin = 3;
int armPinOne = 4;
int armPinTwo = 5;
int linPin = 6;
int doorPin = 9;

int val; //used to map pulse values 

void setup() {
  // put your setup code here, to run once:
  //Attach the motor controllers to corresponding pins on the teensy(pins 3-6)
  drumControl.attach(drumPin);
  armControlOne.attach(armPinOne);
  armControlTwo.attach(armPinTwo);
  linControl.attach(linPin);
  doorControl.attach(doorPin);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  String commandUpdate = Serial.readString; //maybe?
  drum(commandUpdate[0]); //get drum command and send
  arm(commandUpdate.substring(1, 3)); //get arm command and send
  lin(commandUpdate.substring(4, 6)); //get lin command and send
  door(commandUpdate[7]); //get door command and send

}


void drum(char drumCommand){
    //using the SPARK MAX motor controllers and NEO brushless motors
    //datasheet for the NEO motors: http://www.revrobotics.com/content/docs/REV-21-1650-DS.pdf
    //datasheet for the SPARK MAX motor controllers
    //motor controllers can be controlled with PWM, CAN, and USB interfaces
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