#include "esp_camera.h" // estudar essa biblioteca
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// Configurações da rede Wi-Fi
const char* ssid = "Arthur";
const char* password = "ESP32ROVER";

// Cria uma instância do servidor na porta 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // ver o que é

// Definições dos pinos da câmera AI THINKER
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM      5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

// Definições dos pinos dos motores
#define Pino_IN1  12
#define Pino_IN2  13
#define Pino_IN3  14
#define Pino_IN4  15

//#define LED 4 // led grande
#define LED 4
#define LED_Interno 33

// Função para capturar e enviar imagem da câmera via WebSocket
void captureAndSendFrame() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Falha ao capturar imagem");
    return;
  }
  ws.binaryAll(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

// Funções para controlar os movimentos do robô
void Mover_Frente() {
  digitalWrite(Pino_IN1, LOW);
  digitalWrite(Pino_IN2, HIGH);
  digitalWrite(Pino_IN3, LOW);
  digitalWrite(Pino_IN4, HIGH);

}

void Mover_Atras() {
  digitalWrite(Pino_IN1, HIGH);
  digitalWrite(Pino_IN2, LOW);
  digitalWrite(Pino_IN3, HIGH);
  digitalWrite(Pino_IN4, LOW);

}

void Mover_Esquerda() {
  digitalWrite(Pino_IN1, HIGH);
  digitalWrite(Pino_IN2, LOW);
  digitalWrite(Pino_IN3, LOW);
  digitalWrite(Pino_IN4, HIGH);

}

void Mover_Direita() {
  digitalWrite(Pino_IN1, LOW);
  digitalWrite(Pino_IN2, HIGH);
  digitalWrite(Pino_IN3, HIGH);
  digitalWrite(Pino_IN4, LOW);

}

void Parar() {
  digitalWrite(Pino_IN1, LOW);
  digitalWrite(Pino_IN2, LOW);
  digitalWrite(Pino_IN3, LOW);
  digitalWrite(Pino_IN4, LOW);

}

// Configurações iniciais
void setup() {
  Serial.begin(115200);

  //pinMode(LED, OUTPUT);
  pinMode(LED_Interno, OUTPUT);

   // Inicializa o Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(LED_Interno, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(LED_Interno, HIGH);
  }
  Serial.println("\nConectado ao Wi-Fi");

  

  // Configuração dos pinos da câmera
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12; 
  config.fb_count = 1;

  // Inicializa a câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Falha ao inicializar a câmera: 0x%x", err);
    return;
  }
  Serial.println("Câmera inicializada com sucesso");

  // Configuração dos pinos dos motores
  pinMode(Pino_IN1, OUTPUT);
  pinMode(Pino_IN2, OUTPUT);
  pinMode(Pino_IN3, OUTPUT);
  pinMode(Pino_IN4, OUTPUT);
  
  Parar(); // Garante que o robô não ande quando for ligado.


  // Configura o WebSocket
  ws.onEvent([](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){
      Serial.println("WebSocket cliente conectado");
    } else if(type == WS_EVT_DISCONNECT){
      Serial.println("WebSocket cliente desconectado");
    }
  });
  server.addHandler(&ws);

  // Configura as rotas do servidor
  server.on("/Parar", HTTP_GET, [](AsyncWebServerRequest *request){ Parar(); request->send(200, "text/plain", "P"); });
  server.on("/Frente", HTTP_GET, [](AsyncWebServerRequest *request){ Mover_Frente(); request->send(200, "text/plain", "F"); });
  server.on("/Atras", HTTP_GET, [](AsyncWebServerRequest *request){ Mover_Atras(); request->send(200, "text/plain", "A"); });
  server.on("/Esquerda", HTTP_GET, [](AsyncWebServerRequest *request){ Mover_Esquerda(); request->send(200, "text/plain", "E"); });
  server.on("/Direita", HTTP_GET, [](AsyncWebServerRequest *request){ Mover_Direita(); request->send(200, "text/plain", "D"); });


  // Página HTML
      //<img id="stream" src="" width="320" height="240" />
      //<img id="stream" src="" width="640" height="480" />   // VIDEO MAIOR NA PAGINA HTML
      //<button onclick="fetch('/Frente')">Frente</button>
      //   <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('frente');" ontouchstart="toggleCheckbox('frente');" onmouseup="toggleCheckbox('parar');" ontouchend="toggleCheckbox('parar');">Frente</button></td></tr>
      //<tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('frente');" ontouchstart="toggleCheckbox('frente');" onmouseup="toggleCheckbox('parar');" ontouchend="toggleCheckbox('parar');">Frente</button></td></tr>
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", R"rawliteral(
    <html>
    <head>
      <style>
        body {
          display: flex;
          flex-direction: column;
          align-items: center;
          margin: 0;
          background-color: #5f5f5f;
        }
        .video-container {
          margin-top: 15px;
        }
        .joystick {
          display: grid;
          grid-template-columns: repeat(3, 150px);
          grid-template-rows: repeat(3, 150px);
          gap: 140px;
          place-items: center;
          margin-top: 80px;
        }
        .joystick button {
          width: 270px;
          height: 270px;
          font-size: 35px;
          border: none;
          background-color: #424242;
          color: white;
          border-radius: 50%;
          cursor: pointer;
        }
        .joystick button:active {
          background-color: #222222;
        }
      </style>
    </head>
    <body>
      <div class="video-container">
        <img id="stream" src="" width="800" height="600" style="transform: rotate(180deg);" />
      </div>
      <div class="joystick">
        <div></div>
        <button onmousedown="startMoving('Frente')" onmouseup="stopMoving()" 
                ontouchstart="startMoving('Frente')" ontouchend="stopMoving()">Frente</button>
        <div></div>
        <button onmousedown="startMoving('Esquerda')" onmouseup="stopMoving()" 
                ontouchstart="startMoving('Esquerda')" ontouchend="stopMoving()">Esquerda</button>
        <button onclick="stopMoving()">Parar</button>
        <button onmousedown="startMoving('Direita')" onmouseup="stopMoving()" 
                ontouchstart="startMoving('Direita')" ontouchend="stopMoving()">Direita</button>
        <div></div>
        <button onmousedown="startMoving('Atras')" onmouseup="stopMoving()" 
                ontouchstart="startMoving('Atras')" ontouchend="stopMoving()">Trás</button>
        <div></div>
      </div>
      <script>
        const stream = document.getElementById('stream');
        const ws = new WebSocket(`ws://${location.host}/ws`);
        ws.onmessage = (event) => {
          stream.src = URL.createObjectURL(event.data);
        };

        function startMoving(direction) {
          fetch(`/${direction}`)
            .then(response => {
              if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
              }
              return response;
            })
            .catch(error => console.error('There was a problem with the fetch operation:', error));
        }

        function stopMoving() {
          fetch('/Parar')
            .then(response => {
              if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
              }
              return response;
            })
            .catch(error => console.error('There was a problem with the fetch operation:', error));
        }
      </script>
    </body>
    </html>
  )rawliteral");
});



  server.begin();
}

void loop() {
  ws.cleanupClients();
  captureAndSendFrame();
  delay(5); // Intervalo de tempo para capturar e enviar frames
}
