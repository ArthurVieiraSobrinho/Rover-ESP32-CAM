/*
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4832
load:0x40078000,len:16460
load:0x40080400,len:4
load:0x40080404,len:3504
entry 0x400805cc
E (25) ledc: freq_hz=0 duty_resolution=1
E (25) camera_xclk: ledc_timer_config failed for freq 0, rc=102
E (25) camera_xclk: ledc_timer_config failed, rc=102
E (28) i2c: i2c_set_pin(941): scl and sda gpio numbers are the same
E (34) camera: sccb init err
E (37) camera: Camera probe failed with error 0x102(ESP_ERR_INVALID_ARG)
E (43) i2c: i2c_driver_delete(451): i2c driver install error
Erro ao inicializar a câmera: ESP_ERR_INVALID_ARG */



#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Defina suas credenciais de Wi-Fi
//const char* ssid = "Arthur";  // Substitua pelo seu SSID
//const char* password = "ESP32ROVER";  // Substitua pela sua senha

const char* ssid = "Arthur";  // Substitua pelo seu SSID
const char* password = "ESP32ROVER";  // Substitua pela sua senha

// Criação do servidor na porta 80
WebServer server(80);

// Configuração da câmera

/*camera_config_t config = {
    .pin_pwdn = 32,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sccb_sda = 26,
    .pin_sccb_scl = 27,
    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,
    .xclk_freq_hz = 20000000,
    .ledc_channel = LEDC_CHANNEL_0, // Defina um canal LEDC válido
    .ledc_timer = LEDC_TIMER_0,      // Defina um timer LEDC válido
    .pixel_format = PIXFORMAT_JPEG,   // Formato de pixel
    .frame_size = FRAMESIZE_SVGA,     // Tamanho do frame
    .jpeg_quality = 12,                // Qualidade JPEG (0-63)
    .fb_count = 2                      // Número de buffers de frame
};*/




#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

  #else
  //#error "Camera model not selected"
#endif

camera_config_t config; 


void startCameraServer() {
    server.on("/", HTTP_GET, []() {
        String html = "<html><body>";
        html += "<h1>ESP32-CAM Streaming</h1>";
        //String = stream;
        html += "<img src=\"/stream\" width=\"640\" height=\"480\">";
        html += "</body></html>";
        server.send(200, "text/html", html);
    });

    server.on("/stream", HTTP_GET, []() {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Falha ao capturar imagem");
            return;
        }

        server.sendHeader("Access-Control-Allow-Origin", "*");
//        server.send(200, "image/jpeg", (const char*)fb->buf,fb->len);
       // esp_camera_fb_return(fb);
        server.sendHeader("Content-Type", "image/jpeg");
        server.send(200, "image/jpeg", ""); // Envie um cabeçalho vazio primeiro
        server.sendContent((const char*)fb->buf, fb->len); // Envie o conteúdo da imagem
        esp_camera_fb_return(fb);



    });

    server.begin();
}

void setup() {
    Serial.begin(115200);

    delay(1000);
    // Conecta-se à rede Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        Serial.println("Conectando ao WiFi...");
    }

    Serial.println("Conectado ao WiFi");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    // Inicializa a câmera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Erro ao inicializar a câmera: %s\n", esp_err_to_name(err));
        return;
    }
     

    startCameraServer();


     // Configurações da câmera (ajuste conforme seu modelo de ESP32-CAM)
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 0;  // GPIO 0
    config.pin_d1 = 1;  // GPIO 1
    config.pin_d2 = 2;  // GPIO 2
    config.pin_d3 = 3;  // GPIO 3
    config.pin_d4 = 4;  // GPIO 4
    config.pin_d5 = 5;  // GPIO 5
    config.pin_d6 = 6;  // GPIO 6
    config.pin_d7 = 7;  // GPIO 7
    config.pin_xclk = 21; // GPIO 21
    config.pin_pclk = 22; // GPIO 22
    config.pin_vsync = 25; // GPIO 25
    config.pin_href = 23; // GPIO 23
    config.pin_sscb_sda = 26; // GPIO 26
    config.pin_sscb_scl = 27; // GPIO 27
    config.pin_pwdn = -1; // GPIO -1 (não usado)
    config.pin_reset = -1; // GPIO -1 (não usado)
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; 

    // Inicializa a câmera
    if (psramFound()) {
        config.frame_size = FRAMESIZE_SVGA; // Tamanho do quadro (SVGA)
        config.jpeg_quality = 12; // Qualidade JPEG (0-63)
        config.fb_count = 2; // Número de buffers de quadro
    } else {
        config.frame_size = FRAMESIZE_QVGA; // Tamanho do quadro (QVGA)
        config.jpeg_quality = 12; // Qualidade JPEG (0-63)
        config.fb_count = 1; // Número de buffers de quadro
    }

    
}

void loop() {
    server.handleClient();
}
