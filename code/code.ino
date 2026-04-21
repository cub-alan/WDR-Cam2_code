//Jacob Holwill 10859926
//

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
  if (!xSemaphoreTake(SDMutex, pdMS_TO_TICKS(100))) {
    return;
  }
  sample_id++;
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    String filename = "/cam2_" + String(sample_id) + ".jpg";
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (file) {
      file.write(fb->buf, fb->len);
      file.close();
    }
    esp_camera_fb_return(fb);
  }
  xSemaphoreGive(SDMutex);
}

void loop() {
  static bool server_running = (WiFi.status() == WL_CONNECTED);
  static unsigned long WIFI_Retry = 0;

  Light_Check();

  if (WiFi.status() == WL_CONNECTED){
    if (currentMode != MODE_WIFI){
      currentMode = MODE_WIFI;
      Serial.println("Switched to WIFI mode");
      Cam2_Server_Init();
      server_running = true;
      SD_Start_Sending();
    }
  } 
  else {
    if (currentMode != MODE_SD){
      currentMode = MODE_SD;
      Serial.println("Switched to SD mode");
      SD_Stop_Sending();
      if (server_running) {
        httpd_stop(Server);
        server_running = false;
      }
    }
  }
  if (WiFi.status() == WL_DISCONNECTED && millis() - WIFI_Retry > 5000) {
    WIFI_Retry = millis();
    WiFi.disconnect(true);
    WiFi.begin(ssid, password);
  }

  if (Trigger_Flaged){
    Trigger_Sample();
    Trigger_Flaged = false;
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}
