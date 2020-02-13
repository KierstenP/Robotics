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
 *    - 0 = closed
 *    - 1 = open
 */
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "Team_16"
#define WIFI_PASS "lunabots"
#define WIFI_CHANNEL 1

#define SERVER_ADDR "192.168.1.4"
#define SERVER_PORT 1883
#define TOPIC_NAME_OUT "demoTopicOut"
#define TOPIC_NAME_IN "robotCmds/limbs"
#define MQTT_RECONNECT_TIMEOUT 200
#define MQTT_READ_TIMEOUT 50

// Create MQTT client on the ESP
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER_ADDR, SERVER_PORT, "demoTopic", "");
//new topic that we are reading from is "robotCmds/limbs"
Adafruit_MQTT_Publish outTopic = Adafruit_MQTT_Publish(&mqtt, TOPIC_NAME_OUT);
Adafruit_MQTT_Subscribe inTopic = Adafruit_MQTT_Subscribe(&mqtt, TOPIC_NAME_IN);


void checkConnection();
void publishTest();
void subscribeTest();
void officialSubscribe(); 

void drum(char drumCommand);
void arm(char armCommand[3]);
void lin(char linCommand[3]);
void door(char doorCommand[1]);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
 
  WiFi.begin(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);

  //  Wait until ESP is connected to wifi to proceed
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  mqtt.subscribe(&inTopic); //this subscribes to a topic i.e. reads from a topic
}

void loop() {
  // Constantly check the connection and reconnect if it is dropped
  checkConnection();

  //publishTest();
  //subscribeTest();
  officialSubscribe();
  
}

 
void checkConnection(){
  if (mqtt.connected()) return; 
  
  int8_t connectionStatus;
  connectionStatus = mqtt.connect();
  while (connectionStatus != 0){
    mqtt.disconnect();
    delay(MQTT_RECONNECT_TIMEOUT);
    connectionStatus = mqtt.connect();
  }
}

void publishTest() {
  mqtt.publish("demoTopicOut", "hi");
}

void subscribeTest() {
  Adafruit_MQTT_Subscribe* subPtr;
  while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
    if (subPtr == &inTopic){
      Serial.println((char*) inTopic.lastread);  
    }
  }
}

void officialSubscribe(){
  Adafruit_MQTT_Subscribe* subPtr;
  while ((subPtr = mqtt.readSubscription(MQTT_READ_TIMEOUT))){
    if (subPtr == &inTopic){
      String commandUpdate = Serial.readString((char*) inTopic.lastread); 
      drum(commandUpdate[0]); //get drum command and send
      arm(commandUpdate.substring(1, 3)); //get arm command and send
      lin(commandUpdate.substring(4, 6)); //get lin command and send
      door(commandUpdate[7]); //get door command and send
    }
  }
}

//function to handle drum code interpretation
void drum(char drumCommand){
  switch(drumCommand){
    case '0':
      //the drum is not moving
      break;
    case '1':
      //rotate the drum forward at a slow/medium speed
      break;
    case '2':
      //rotate the drum forward at a medium/high speed
      break; 
    case '3' 
      //rotate the drum backwards at a slow/medium speed
      break;
    case '4' 
      //rotate the drum backwards at a medium/high speed
  }
}

//function to handle arm code interpretation
void arm(char armCommand[3]){
  //code to rotate front motors by angle armCommand
  //code to rotate rear motors by angle armCommand
}

//function to handle linear actuator code interpretation
void lin(char linCommand[3]){
   switch(linCommand[0]){
    case '0':
      //drum is not moving 
      break;
    case '1':
      //drum is moving down into the digging configuration at speed linCommand.substring(1, 2)
      break;
    case '2':
      //drum is moving up into the starting configuration at speed linCommand.substring(1, 2)
      break;
   } 
}

//function to handle door code interpretation
void door(char doorCommand){
  switch(doorCommand){
    case '0':
      //door is not moving
      break;
    case '1':
      //close the door
      break;
    case '2':
      //open the door
      break;
  }  
}
