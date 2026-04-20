//Jacob Holwill 10859926
//

#include "MyLib.hpp"

#define Sample_Sync_Pin D1 // offline image sync between cams
volatile uint32_t sample_id = 0; // id to match data from same sample

void setup() {
  
  Serial.begin(115200); // set the terminal baud rate and begin the terminal
  Serial.setDebugOutput(true);

  for(int i=0;i<40;i++){
    Serial.println(); // clear 40 lines of the terminal 
  } 

  Cam2_init(); // initilise the Camera
  Serial.println("CAM Initialized");
  delay(2000); // Wait 2000 ms

  Light_init(); // initialise the ring light and LDR code
  Serial.println("Lighting Initialized");

  SD_Init(); // initialise the SD card 
  Serial.println("SD Initialized");

  pinMode(Sample_Sync_Pin, INPUT);

  WIFI_Connect(); // attempt to connect to the wifi
  
  if (WiFi.status() == WL_CONNECTED){ // if wifi is connected
    Cam2_Server_Init(); // initialise the server for camera 1
    currentMode = MODE_WIFI; // set the mode to WIFI

    // print out the weblinks for the Camera and gnss
    Serial.print("for camera use 'http://");
    Serial.print(WiFi.localIP()); // paste the ip of the camera stream 
    Serial.println("/stream2");

  }
  
}

void loop() {

  

  Light_Check();
  vTaskDelay(pdMS_TO_TICKS(10));
}
