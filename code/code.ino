//Jacob Holwill 10859926
//this file is to intergrate my other code together to create the initialisation and loop for the camera 2 operation

#include "MyLib.hpp"

enum SystemMode {
  MODE_WIFI,
  MODE_SD
};
SystemMode currentMode = MODE_SD;

#define Sample_Sync_Pin D1 // offline image sync between cams
volatile bool Trigger_Flaged = false;
volatile uint32_t sample_id = 0; // id to match data from same sample

void IRAM_ATTR Triggered() {
    Trigger_Flaged = true;
}

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

  SetUp_PWM();

  pinMode(Sample_Sync_Pin, INPUT_PULLDOWN);
  attachInterrupt(Sample_Sync_Pin, Triggered, RISING);

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

void Trigger_Sample() {
  if (!xSemaphoreTake(SDMutex, pdMS_TO_TICKS(100))) { // if SD mutex is not free return
    return;
  }
  sample_id++; // add one to sample id
  
  camera_fb_t *fb = esp_camera_fb_get(); // get the current fram
  if (fb) { // if frame received
    String filename = "/cam2_" + String(sample_id) + ".jpg"; // create the file name
    File file = SD_MMC.open(filename, FILE_WRITE); // create a file with that frame to SD card with that name
    if (file) { //if file create was succesful 
      file.write(fb->buf, fb->len); // write the file 
      file.close(); // close it
    }
    esp_camera_fb_return(fb); // return the frame
  }
  xSemaphoreGive(SDMutex); // relock the mutex
}

void loop() {
  static bool server_running = (WiFi.status() == WL_CONNECTED);
  static unsigned long WIFI_Retry = 0;

  Light_Check(); // run ring light code

  if (WiFi.status() == WL_CONNECTED){ //  if wifi is connected
    if (currentMode != MODE_WIFI){ // if current mode is not wifi
      currentMode = MODE_WIFI; // swap mode
      Serial.println("Switched to WIFI mode");
      Cam2_Server_Init(); // start the server
      server_running = true;
    }
  } 
  else { // if wifi not connected
    if (currentMode != MODE_SD){ // if not in SD mode
      currentMode = MODE_SD; //swap mode
      Serial.println("Switched to SD mode");
      SD_Stop_Sending(); // stop sending SD data
      if (server_running) { // check if the server is running
        httpd_stop(Server); // stop the server
        server_running = false;
      }
    }
  }
  if (WiFi.status() == WL_DISCONNECTED && millis() - WIFI_Retry > 5000) { // attempt to reconect to wifi every 5 seconds
    WIFI_Retry = millis();
    WiFi.disconnect(true); // disconect
    WiFi.begin(ssid, password); // reconect
  }

  if (Trigger_Flaged){ // if a sample is triggered from the other esp
    Trigger_Sample(); // execture sampling function
    Trigger_Flaged = false;
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}
