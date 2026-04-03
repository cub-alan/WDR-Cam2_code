//Jacob Holwill 10859926
//The point of this file is to initialise and run the code to controll and stream the camera

#include "Cam.hpp" // include the hpp file so it has acess to all nessesary librarys

bool camera_initialized = false;
httpd_handle_t Server = NULL;

// create a funtion to handle the stream and return any esp errors and points to the http request object

void Cam2_init() {
    //Pin configuration were taken from the camera webserver example code and adjusted where nessesary
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;
    
    // Initialize directly at QVGA for stability and memory efficiency
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QQVGA; // start at lower fram size 

    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_LATEST;
        Serial.println("PSRAM working");
    }   
     else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_DRAM;
        Serial.println("PSRAM failed");
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        camera_initialized = false;
        return;
    }
    
    Serial.println("Camera init succeeded");
    camera_initialized = true;

    // Wait a moment for the sensor to stabilize
    delay(500);

    sensor_t * sens = esp_camera_sensor_get();
    if (sens) {
        sens->set_vflip(sens, 0);      // Flip vertically if needed
        sens->set_hmirror(sens, 0);    // Horizontal mirror
        sens->set_brightness(sens, 0); // Default brightness
        sens->set_contrast(sens, 0);   // Default contrast
    }
    camera_fb_t * fb = esp_camera_fb_get();

    if (!fb) {
        Serial.println("CAMERA CAPTURE FAILED");
    }   
    else {
        Serial.printf("CAMERA FRAME OK: %d bytes\n", fb->len);
        esp_camera_fb_return(fb);
    }
}
esp_err_t Cam_Stream_Handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;

    res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
    if(res != ESP_OK) return res;

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    while(true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        // Send boundary
        res = httpd_resp_send_chunk(req, "--frame\r\n", 9);

        // Send headers
        if(res == ESP_OK) {
            char part_buf[128];
            int len = snprintf(part_buf, sizeof(part_buf),"Content-Type: image/jpeg\r\n" "Content-Length: %u\r\n\r\n",fb->len);
            res = httpd_resp_send_chunk(req, part_buf, len);
        }

        // Send image
        if(res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        }

        // End frame
        if(res == ESP_OK) {
            res = httpd_resp_send_chunk(req, "\r\n", 2);
        }

        esp_camera_fb_return(fb);

        if(res != ESP_OK) break;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    return res;
}
void Cam2_Server_Init() {

  httpd_config_t Cam2_Server_Config = HTTPD_DEFAULT_CONFIG(); // create a variable to save all server info/ settings
  Cam2_Server_Config.server_port = 81; // set the webservers pot to number 80 which is standard
  Cam2_Server_Config.core_id = 0;

  // create a variable and store the cameras URL in it for streaming and to asses the cams veiw
  static httpd_uri_t Stream_URI = {.uri = "/stream", .method = HTTP_GET, .handler = Cam_Stream_Handler, .user_ctx  = NULL}; 
  
  // Start the server
  if (httpd_start(&Server, &Cam2_Server_Config) == ESP_OK) { //check everything is set up correctly start the server
    httpd_register_uri_handler(Server, &Stream_URI); // create the cameras page on the server 
    Serial.println("Camera 2 server began"); // print to the terminal so you now the server is ready
  }
}

