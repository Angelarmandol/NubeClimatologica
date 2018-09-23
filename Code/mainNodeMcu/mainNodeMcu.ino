
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
 
const char* ssid = "adg0"; 
const char* password = ""; 
 
const char* host = "api.openweathermap.org";
const char * cityId = "6942831";
const char* apiKey = "171f3d5eed4e72c670326a5f9ef80990";  
 StaticJsonBuffer<200> jsonBuffer;
String sunrise;
String line;
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  // Conectamos a la red WiFi
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}
 
void loop() {
 
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80; 
  if (!client.connect(host, httpPort)) {
    Serial.println("Ha fallado la conexión");
    return;
  }
 
  String url = "/data/2.5/weather?id=";
  url += cityId;
  url += "&APPID=";
  url += apiKey;
  
 
  Serial.print("URL de la petición: http://");
  Serial.print(host);
  Serial.print(":");
  Serial.print(httpPort);
  Serial.println(url);
 

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" + 
         "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Superado el tiempo de espera !");
      client.stop();
      return;
    }
  }
 

  Serial.printf("\nMemoria libre en el ESP8266: %d Bytes\n\n",ESP.getFreeHeap());
 

  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.print(line);

    
  }

 
String nombre = GetInfo( line, "name", ",") ;
Serial.println("nombre k  ");
Serial.print(nombre);

String temperatura= GetInfo( line, "temp", ",") ;
Serial.println("temperatura k  ");
Serial.print(temperatura);

String humedad= GetInfo( line, "humidity", ",") ;
Serial.println("humedad   ");
Serial.print(humedad);

String nubes = GetInfo( line, "clouds", ",") ;
Serial.println("nubes   ");
Serial.print(nubes);

 


  
 


  Serial.println("Cerrando la conexión");
 
  while(1){
    delay(0); 
  }


  
}

String GetInfo( String &line, String t1, String t2 ){   
  
  int n = line.indexOf( t1 );
       if (n)
          {    line = line.substring(n);         //Despreciamos la parte inicial de S
               int k = line.indexOf( t2 );    // Buscamos la siguiente coma
               String var = line.substring(t1.length() , k);
               line = line.substring( k+1);  // Recortamos S
               return(var);
           }
   }
