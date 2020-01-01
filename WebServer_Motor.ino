#include <WiFi.h>

/* char* ssid     = "Luiz-Marcelo";
const char* password = "carreira";*/
const char* ssid     = "Lapes";
const char* password = "sepaL123";

WiFiServer server(80);

String header;
String valueString = String(0);
String webPage[] = {
  "<!DOCTYPE html>"
  ,"<html lang='pt'>" //HTML
  ,"  <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Lato\">"    
  ,"  <style>"
  ,"    body { text-align: center; background: #06860352; width: 95%; max-width: 800px; margin: auto; font-family: 'Lato'; line-height: 1.5; color: white;}"
  ,"    main { background: #00000054; padding: 5px 40px; margin-top: 20px; border-radius: 10px;}"
  ,"    .concerts { margin: 30px 0; text-align: center; width: 100%;}"
  ,"    .concerts table { background: #1d7d2dd9; border-radius: 10px; padding: 10px; margin: 0 auto;}"
  ,"    .slider { -webkit-appearance: none; width: 100%; height: 15px; border-radius: 5px; background: white; outline: none; opacity: 1; -webkit-transition: .2s;   transition: opacity .2s;}"
  ,"    .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 50%; background: #1d7d2d; cursor: pointer;}"
  ,"    .slider::-moz-range-thumb { width: 25px; height: 25px; border-radius: 50%; background: #1d7d2d; cursor: pointer;}"
  ,"  </style>"  
  ,"  <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>"
  ,"  <head>" //HEAD
  ,"    <title>Sistemas Embarcados</title>"  
  ,"  </head>" //FIM_HEAD
  ,"  <body><main>" //BODY
  ,"    <div>"
  ,"      <h1>Controlador de Motor CC</h1>"
  ,"      <h2>Via Web utilizando ESP32</h2>"
  ,"      <h3>Motor de Corrente Continua</h3>  "
  ,"      <p>Sentido & Velocidade = <span id=\"servoPos\"></span></p>"
  ,"      <input type=\"range\" min=\"-100\" max=\"100\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+valueString+"\"/>"  
  ,"    </div>"
  ,"    <hr>"
  ,"    <div class=\"concerts\">"
  ,"      <h2>Membros</h2>"
  ,"      <table>"
  ,"        <thead><tr><th>Docente:</th></tr></thead>"
  ,"        <tbody><tr><td>Giuliano Robledo Zucoloto Moreira</td></tr></tbody>"
  ,"        <thead><tr><th>Discentes:</th></tr></thead>"
  ,"        <tbody><tr><td>Luis Alexandre Ferreira Bueno</td></tr><tr><td>Yuri Roque Benvenutti</td></tr></tbody>"
  ,"      </table>"
  ,"    </div>"
  ,"  </body></main>" //FIM_BODY
  ,"  <script>var slider = document.getElementById(\"servoSlider\");"
  ,"    var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;"
  ,"    slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }"
  ,"    $.ajaxSetup({timeout:1000}); function servo(pos) { "
  ,"    $.get(\"/?value=\" + pos + \"&\"); {Connection: close};}"
  ,"  </script>"  
  ,"</html>" //FIM_HTML
};

void setup() {
  Serial.begin(9600);

  pinMode(12, OUTPUT);
  ledcAttachPin(12, 0);
  ledcSetup(0, 1000, 8);
  
  pinMode(13, OUTPUT);
  ledcAttachPin(13, 1);
  ledcSetup(1, 1000, 8);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();
  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            for(int i=0; i< (sizeof(webPage)/sizeof(webPage[0])); i++)
              client.println(webPage[i]);
            valueString = velocidadeMotor(header);
            MotorCC(valueString.toInt());
            Serial.println(valueString); 
                     
            break;
          }else{
            currentLine = "";
          }
        }else if (c != '\r'){
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

int constPartida = 70;
int velocidade = 0;

void MotorCC (int valor){
  if(valor == 0){
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    delay(2);
  }else if(valor > 0){
    velocidade = (int) (valor*185)/100;
    ledcWrite(1, 0);
    ledcWrite(0, velocidade+constPartida);
    delay(2);
  }else{
    valor = valor*(-1);
    velocidade = (int) (valor*185)/100;
    ledcWrite(0, 0);
    ledcWrite(1, velocidade+constPartida);
    delay(2);
  }
}

String velocidadeMotor(String header){
  int pos1, pos2;
  if(header.indexOf("GET /?value=")>=0) {       //GET /?value=100& HTTP/1.1
    pos1 = header.indexOf('=');
    pos2 = header.indexOf('&');
    return header.substring(pos1+1, pos2);
  }
  return "";
}
