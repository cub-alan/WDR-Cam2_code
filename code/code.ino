//
//

#include "MyLib.hpp"

const char* ssid = "iPhone"; 
const char* password = "12345678";

void setup() {
  
  Serial.begin(115200); // set the terminal baud rate and begin the terminal
  Serial.setDebugOutput(true);
  for(int i=0;i<40;i++){
    Serial.println(); // clear 40 lines of the terminal 
  } 

  Cam2_init(); // initilise the Camera
  Serial.println("CAM Initialized");
  delay(2000); // Wait 2000 ms

  Serial.println("");
  Serial.print("Connecting to WIFI");

  WiFi.begin(ssid, password); // connect the wifi to the set ssid and password
  WiFi.setAutoReconnect(true); // set it to auto reconnect to the wifi if it becomes disconected

  while (WiFi.status() != WL_CONNECTED) { // while wifi isnt connected
    delay(500); // wait 500ms
    Serial.print("."); // print a dot to the terminal to show its still attempting to connect
  }

  Serial.println("");
  Serial.println("WiFi connected"); // print once wifi is connected

  Cam2_Server_Init(); // connect the gnss data stream and camera data stream to the wifi

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP()); // paste the ip of the stream so it can be opened on the browser
  
}

void loop() {

  moveServo(Servo_Pan, 0);
  delay(1000);
  moveServo(Servo_Pan, 90);
  delay(1000);
}
