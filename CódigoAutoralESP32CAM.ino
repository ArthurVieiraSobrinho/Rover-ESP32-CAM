/*-------------------------------- DADOS GERAIS ----------------------------------------------------/

Esse código tem o objetivo de controlar um rover através de um web server que o próprio esp32-cam gerou
no web server o esp irá conferir, a cada instante de tempo, se recebeu alguma requisição sea para controle de locomoção, led ou servo garra. Caso ele não receba nenhuma requisição irá conferir novamente em um loop infinito
IP PADRÃO DO ESP COMO ROTEADOR 192.168.4.1 
quando o ESP se comporta como um servidor web nós conseguimos colocar uma página dentro dele 
me inspirei nesses materiais para o código:
https://www.youtube.com/watch?v=iV_a5Y_8_3c&list=PLynFNITd4K9Ee0LO7JUVS7xTJJmhiFRHv&index=7
https://www.facebook.com/100057676321788/videos/controle-um-carrinho-pelo-wi-fi-com-esp32/908429986248108/
https://www.youtube.com/watch?v=ZSyqNFGAF8o&list=PLynFNITd4K9Ee0LO7JUVS7xTJJmhiFRHv&index=15


*/

//inclusão de bibliotecas de conexão  ESP-CAM ---> WiFi
#include <WiFi.h>       // O ESP liga em alguma rede WiFi. Ele não cria uma, apenas se conecta
#include <WiFiAP.h>     // faz com que o esp crie o seu próprio WiFi para que possamos nos conectar AP = Aess Point
#include <WiFiClient.h>


//Declarando os pinos dos motores
#define MOTOR1_IN1    15
#define MOTOR1_IN2    14
#define MOTOR2_IN3    13
#define MOTOR2_IN4    12

#define LED    33

//-------------Definição da Rede WiFi do ESP-CAM--------------//
//const char *ssid      = "Robô-CÂMERA";      // SENHA DA REDE CRIADA PELO ESP
//const char *password  = "Arthur";           // NOME DA REDE CRIADA PELO ESP

//------------Rede no qual o ESP irá se conectar-------------//
const char *ssid = "Robô ESP-CAMERA"; // REDE WIFI GERADA PELO MEU CELULAR
const char *password = "ESP32ROVER";

WiFiServer server(80); // Declaração que define um servidor com a porta 80 do esp com wifi


void setup() {
   pinMode(LED, OUTPUT);
  /* ----------------WIFI GERADO PELO ESP------------//
  Serial.begin(115200);                       // inicia o comunicador serial
  WiFi.softAP(ssid,password);                 // Função que gera a rede WiFi
  IPAddress IPESP = WiFi.softAPIP();          // função que mostra o ip do esp e quarda em uma variável
  Serial.println("Endereço IP Da Rede :  ");  //
  Serial.println(IPESP);                      // mostra no monitor serial o número IP
  server.begin();                             // função que inicia o servidor
  Serial.println("Servidor Iniciado!");       // Avisa

*/
  //--------CÓDIGO PARA USAR O MEU WIFI OU WIFI OU O DE CASA-----//
  Serial.begin(115200);                       // inicia o comunicador serial
  Serial.println();
  Serial.println();
  Serial.print("Conectando na Rede:  ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // inicia a conexão como o WiFi escolhido

  while (WiFi.status() != WL_CONNECTED) {  // Enquanto o ESP não se conectar ao WiFi digitar pontos no monitor serial. ponto de "!" significa o oposto,não, diferente
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Conectado.");
  Serial.println("Endereço IP : ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  server.begin();   // inicia o servidor
}

void loop() {

//NetworkClient client = server.accept();  // fica em ritimo constante de checagem se há algum cliente conectado a rede
WiFiClient client = server.available(); // fica em ritimo constante de checagem se há algum cliente conectado a rede

  if (client) {                       //se alguém se conectar
    Serial.println("Novo Cliente.");  
    String RequisiçãoAtual = "";          // faz um string para manter os dados recebidos do cliente(nao entendi essa parte)
    while (client.connected()) {      // fica aqui enquanto o cliente estiver conectado
      if (client.available()) {       // caso haja dados do cliente para ler (bytes) 
        char c = client.read();       // leia um byte do cliente
        Serial.write(c);              // print it out the serial monitor
        if (c == '\n') {              // NÃO ENTENDI ESSA PARTE. quebra a linha para receber uma nova instrução?

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (RequisiçãoAtual.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)  NAO ENTENDI
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");  // "SUA AÇÃO FOI  CONCLUIDA COM SUCESSO"? ESSA É A RESPOSTA?
            client.println("Content-type:text/html");  // RESPOSTA DO TIPO TEXTO HTML?
            client.println();

            // Esse é a resposta http no qual iremos usar a letra enviada para que o ESP dê comandos
            //ele envia uma requisição http para o esp
            client.println("<html>");
            client.println("<head><meta content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { margin: Opx auto; text-align: center;}");
            client.println(".botao_liga { background-colod: #00FF00; color: white; padding: 15px 40px; border-radius: 25px;}");
            client.println(".botao_liga { background-colod: #FF0000; color: white; padding: 15px 40px; border-radius: 25px;}<style><>");
            client.println("<body><hl>ROBÔ CÂMERA DO ARTHUR</hl>");
            client.print("<p><a href=\"/H\"><button class=\"botao liga\">LIGA</button></a></p>");
            client.print("<p><a href=\"/L\"><button class=\"botao desliga\">DESLIGA</button></a></p>");

            client.println("</body></html>");
            client.println();


           //client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
           // client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            RequisiçãoAtual = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          RequisiçãoAtual += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (RequisiçãoAtual.endsWith("GET /H")) {
          digitalWrite(LED, HIGH);  // GET /H turns the LED on
        }
        if (RequisiçãoAtual.endsWith("GET /L")) {
          digitalWrite(LED, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  



















//WiFiClient client = server.available(); // fica em ritimo constante de checagem se há algum cliente conectado a rede

   /* if(client) {                        //caso alguém se conecte
      Serial.println("Você ou alguém entrou.");
      String currentLine = "";         // faz um string para manter os dados recebidos do cliente(nao entendi essa parte)
      while(client.connected()) {      // fica aqui enquanto o cliente estiver conectado
           if (client.available()) {    // aso haja dados do cliente para ler (bytes)   
             char c = client.read();  // leia um byte 
             Serial.write(c);
             if(c == '\n') {          // NÃO ENTENDI ESSA PARTE
        
     // }
    //}


    if(currentLine.length() == 0 ) {
      client.println("HTTP//1.1 200 OK");
      client.println("Content-type:text/html");
      client.println();
       

      //configuração visual/estilística do site
       client.print("<html>");
       client.print("<head>><title> ""Painel de Controle do Rover"" </title></head>");
       client.print("<body>");

     
      client.print("<style type=\"text/css\"> a{margin:600px 600px; background: #228B22; color: #ffffff; text-decoration: none padding:0.54 54 0.54;))

      //----------------Botão Frente na página Web-----------------
      client.print("<a href=\"/FRENTE)\"> Frente </a> </br></br></br>");
      




      client.print("</html>");
      client.print("</body>");

      client.println(); // a resposta htpp termina em uma mensagem em branco
      break; //sair do loop while
    }
    else {
      currentLine = "";
    }
    else if(c !='\r') {
      currentLine += c;
    }  

    if(currentLine.endsWith("GET\FRENTE")){
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
      delay(100);

    }





*/


    }
