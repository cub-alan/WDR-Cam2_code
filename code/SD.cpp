void SD_Init() {
    if (!SD.begin()) { // if the SD card fails to start skip
        Serial.println("SD init failed");
        return;
    }

    SDMutex = xSemaphoreCreateMutex(); // create a mutx for the sd 

    xTaskCreatePinnedToCore(SD_Task,"SD Task",8192,NULL,1,NULL,1); // create a multithreading task pinned to core 1
}
void SD_StartSending() {
    if (xSemaphoreTake(SDMutex, portMAX_DELAY)) { // check the mutex is free
        // open the file and set the send variable to true
        root = SD.open("/");
        currentFile = File();
        SD_send = true;

        xSemaphoreGive(SDMutex); // give back the mutex
    }
}

void SD_StopSending() {
    SD_send = false; // stop the code from sending data
}