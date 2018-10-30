#include <SoftwareSerial.h> 
#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <SoftwareSerial.h> 

#define ARDUINO_RX 5//should connect to TX of the Serial MP3 Player module 
#define ARDUINO_TX 4//connect to RX of the module 
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);//init the serial protocol, tell to myserial wich pins are TX and RX 
// El software serial nm puede ser el  mismo que el rxÇ/tx de NOdemCu.ç

////////////////////////////////////////////////////////////////////////////////////////

static int8_t Send_buf[8] = {0} ;//The MP3 player undestands orders in a 8 int string 
                                //0X7E FF 06 command 00 00 00 EF;(if command =01 next song order)  
#define NEXT_SONG 0X01  
#define PREV_SONG 0X02  
#define CMD_PLAY_W_INDEX 0X03 //DATA IS REQUIRED (number of song) 
#define VOLUME_UP_ONE 0X04 
#define VOLUME_DOWN_ONE 0X05 
#define CMD_SET_VOLUME 0X06//DATA IS REQUIRED (number of volume from 0 up to 30(0x1E)) 
#define SET_DAC 0X17 
#define CMD_PLAY_WITHVOLUME 0X22 //data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song) 
#define CMD_SEL_DEV 0X09 //SELECT STORAGE DEVICE, DATA IS REQUIRED 
#define DEV_TF 0X02 //HELLO,IM THE DATA REQUIRED 
#define SLEEP_MODE_START 0X0A 
#define SLEEP_MODE_WAKEUP 0X0B 
#define CMD_RESET 0X0C//CHIP RESET 
#define CMD_PLAY 0X0D //RESUME PLAYBACK 
#define CMD_PAUSE 0X0E //PLAYBACK IS PAUSED 
#define CMD_PLAY_WITHFOLDER 0X0F//DATA IS NEEDED, 0x7E 06 0F 00 01 02 EF;(play the song with the directory \01\002xxxxxx.mp3 
#define STOP_PLAY 0X16 
#define PLAY_FOLDER 0X17// data is needed 0x7E 06 17 00 01 XX EF;(play the 01 folder)(value xx we dont care) 
#define SET_CYCLEPLAY 0X19//data is needed 00 start; 01 close 
#define SET_DAC 0X17//data is needed 00 start DAC OUTPUT;01 DAC no output 
//////////////////////////////////////////////////////////////////////////////////// 

const char* ssid = "adg0"; 
const char* password = "12341234"; 
const char* host = "api.openweathermap.org";
const char * cityId = "6942831";
const char* apiKey = "171f3d5eed4e72c670326a5f9ef80990";  
 StaticJsonBuffer<200> jsonBuffer;
String sunrise;
String line;
int start = 0;
void setup() {
 pinMode(start, OUTPUT);
Serial.begin(9600);
mySerial.begin(9600);//Start our Serial coms for our serial monitor! 
delay(500);//Wait chip initialization is complete 
sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card   
delay(200);//wait for 200ms 
 
  // Conectamos a la red WiFi
 
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
    sendCommand(CMD_PLAY_W_INDEX, 0X0016);// con internet no disp 
    delay(10000);
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

Serial.println("  - finjson");
String nombre = GetInfo( line, "name\":\"", "\",") ;
Serial.println("nombre "+nombre);

String temperatura= GetInfo( line, "temp\":", ",") ;
Serial.println("temperatura "+temperatura);

String humedad= GetInfo( line, "humidity\":", ",") ;
Serial.println("humedad "+humedad);

String nubes = GetInfo( line, "\"all\":", "},") ;
Serial.println("nubes "+nubes);

 

String climaMain = GetInfo( line, "main\":\"", "\",") ;
Serial.println("Clima Main "+climaMain);

String cielo = GetInfo( line, "description\":\"", "\",") ;
Serial.println("cielo "+cielo);




////////////////////////////////////////////////////////////////////////////////////////
digitalWrite(start, 0);

delay(500);
digitalWrite(start, 1);

delay(500);
digitalWrite(start, 0);

delay(500);
digitalWrite(start, 1);

delay(500);
digitalWrite(start, 0);

delay(500);
digitalWrite(start, 1);

delay(500);
sendCommand(CMD_PLAY_W_INDEX, 0X0013);// condicion climatica actul
delay(3300);
sendCommand(CMD_PLAY_W_INDEX, 0X000E);// la temperatura es de
delay(1500);

Serial.println("pide"+temperatura);
float tempf = kelvinToCelcius(temperatura);
String tempSf = String(tempf);
 
char temperaturaChar [4];
tempSf.toCharArray(temperaturaChar, 4);

Serial.println("valor de tempSf: "+tempSf);

dictar(tempSf);
 
delay(500);
sendCommand(CMD_PLAY_W_INDEX, 0X000B);// grados
sendCommand(CMD_PLAY_W_INDEX, 0X000C);// centigrados
delay(3200);
sendCommand(CMD_PLAY_W_INDEX, 0X0012);// nubes al
delay(900);
Serial.println("nubes al ");
dictar(nubes);
sendCommand(CMD_PLAY_W_INDEX, 0X000F);// porciento
delay(3200);
mainCondition(climaMain, cielo);
delay(2000);
sendCommand(CMD_PLAY_W_INDEX, 0X000D);// humedad esta al
delay(2000);
dictar(humedad);
delay(500);
sendCommand(CMD_PLAY_W_INDEX, 0X000F);// porciento
/////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println("Cerrando la conexión");
 
  while(1){


    digitalWrite(start, 1);

delay(2);
digitalWrite(start, 0);

delay(10);
digitalWrite(start, 1);

delay(68);
digitalWrite(start, 0);

delay(500);
digitalWrite(start, 1);




    delay(0); 
  }

  
}// fin de loop

String GetInfo( String line, String t1, String t2 ){   
  
  int n = line.indexOf( t1 );
       if (n){    line = line.substring(n);         //Despreciamos la parte inicial de S
               int k = line.indexOf( t2 );    // Buscamos la siguiente coma
               String var = line.substring(t1.length() , k);
               line = line.substring( k+1);  // Recortamos S
               return(var);
      }
}// fin get info



void sendCommand(int8_t command, int16_t dat) { 
delay(20); 
Send_buf[0] = 0x7e; //starting byte 
Send_buf[1] = 0xff; //version 
Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte 
Send_buf[3] = command; // 
Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback 
Send_buf[5] = (int8_t)(dat >> 8);//datah 
Send_buf[6] = (int8_t)(dat); //datal 
Send_buf[7] = 239; //ending byte 
  for(uint8_t i=0; i<8; i++)// 
  { 
    mySerial.write(Send_buf[i]) ; 
    Serial.println(Send_buf[i]);
  } 
}// fin send command 


 
float kelvinToCelcius(String numeroCadenak){
//Serial.println("combertir "+numeroCadenak);
float temperatura; 
 
temperatura = numeroCadenak.toFloat();
//Serial.println("antes de combertir, ya en entero ");
//Serial.print(temperatura);
temperatura = temperatura - 273.15;
//Serial.println("resultado "); 
//Serial.print(temperatura);
return temperatura;
}// fin kelvin to celcius




 


void dictar(String cadena){
 int sdEntero = 0;
int pdEntero2 =0;
 int sdEntero2 =0;
 int mq15 = cadena.toInt();
String auxiliar1="";
char pd ='0';
if(cadena.length() < 2){
  
    pd = cadena.charAt(0);
    auxiliar1= String(pd);
    pdEntero2 = auxiliar1.toInt();
    sdEntero2 = pdEntero2;
}else{
  pd = cadena.charAt(0);
  char sd = cadena.charAt(1);

  auxiliar1= String(pd);
  String auxiliar2= String(sd);
  
  float pdEntero1 = auxiliar1.toInt();
  float sdEntero1 =  auxiliar2.toInt();
  pdEntero2 = auxiliar1.toInt();
  sdEntero2 = auxiliar2.toInt();

}

    if(sdEntero2==0)   //---MKSEDA0
      {
   
      //terminacion cero
 
          switch(pdEntero2){  //-MKSTE0

            case 1:
            sendCommand(CMD_PLAY_W_INDEX, 0X0020);// dies
            delay(100);
            break;
            
            case 2:
            sendCommand(CMD_PLAY_W_INDEX, 0X0027);// veinte
            delay(100);
            break;

            case 3:
            Serial.println("Treinta''''''''''''''''''''''''''");
            sendCommand(CMD_PLAY_W_INDEX, 0X0029);// treinta
            delay(100);
            break;
            
            case 4:
            sendCommand(CMD_PLAY_W_INDEX, 0X002B);// cuarenta
            delay(100);
            break;
            
            case 5:
            sendCommand(CMD_PLAY_W_INDEX, 0X002D);// cincuenta
            delay(100);
            break;
            
            case 6:
            sendCommand(CMD_PLAY_W_INDEX, 0X002F);// sesenta
            delay(100);
            break;
        
            case 7:
            sendCommand(CMD_PLAY_W_INDEX, 0X0031);// setenta
            delay(100);
            break;
            
            case 8:
            sendCommand(CMD_PLAY_W_INDEX, 0X0033);// ochenta
            delay(100);
            break;
            
            case 9:
            sendCommand(CMD_PLAY_W_INDEX, 0X0035);// noventa 
            delay(100);
            break;


          } // din de --MKSTE0

      }else{                    // -- MKSEDA0  MKSEDA0E
        //normal
                                if(mq15<=15){
                                 unoA15(mq15);
                                }else{
                                      switch(pdEntero2){     // MKSpdEntero
                                        
                                        case 1:
      
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0026);// diesi
                                        delay(500);
                                        break;
      
                                        case 2:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0028);// veinti
                                        delay(500);
                                        break;
                                        
                                        case 3:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X002A);// treintai
                                        delay(500);
                                        break;
       
                                        case 4:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X002C);// cuarentai
                                        delay(500);
                                        break;
      
                                        case 5:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X002E);// cincuentai
                                        delay(500);
                                        break;
      
                                        case 6:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0030);// sesentai
                                        delay(500);
                                        break;
      
                                        case 7:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0032);// setentai
                                        delay(500);
                                        break;
      
                                        case 8:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0034);// ochentai
                                        delay(500);
                                        break;
      
                                        case 9:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0036);// noventai
                                        delay(500);
                                        break;
      
                                    
                                      }// FIN DEMKSpdEntero 
      
                                      delay(500);

                                      Serial.println("sdEntero2");
                                      Serial.println(sdEntero2);
                                      switch(sdEntero2){     // MKSpdEntero
                                        
                                        case 1:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0017);// uno
                                        delay(600);
                                        break;
      
                                        case 2:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0018);// dos
                                        delay(600);
                                        break;
                                        
                                        case 3:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X0019);// tres
                                        delay(600);
                                        break;
      
                                        case 4:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001A);// cuatr
                                        delay(600);
                                        break;
      
                                        case 5:
                                        Serial.println("cinco  ");
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001B);// cinco
                                        delay(600);
                                        break;
      
                                        case 6:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001C);// seis
                                        delay(600);
                                        break;
      
                                        case 7:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001D);// siete
                                        delay(600);
                                        break;
      
                                        case 8:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001E);// ocho
                                        delay(600);
                                        break;
      
                                        case 9:
                                        sendCommand(CMD_PLAY_W_INDEX, 0X001F);// nueve
                                        delay(600);
                                        break;
               
      
                                      }// FIN DEMKSpdEntero 
                                }
                  }// fin de MKSEDA0E

}//fin dictar


void unoA15(int numero){

Serial.println("unoa15entra: ");
      
      switch(numero){
        
      case 1:
      sendCommand(CMD_PLAY_W_INDEX, 0X0017);// uno
      delay(500);
      break;
      case 2:
      sendCommand(CMD_PLAY_W_INDEX, 0X0018);// dos
      delay(500);
      break;
      case 3:
      sendCommand(CMD_PLAY_W_INDEX, 0X0019);// tres
      delay(500);
      break;
      case 4:
      sendCommand(CMD_PLAY_W_INDEX, 0X001A);// cuatr
      delay(500);
      break;
      case 5:
      sendCommand(CMD_PLAY_W_INDEX, 0X001B);// cinco
      delay(500);
      break;
      case 6:
      sendCommand(CMD_PLAY_W_INDEX, 0X001C);// seis
      delay(500);
      break;
      case 7:
      sendCommand(CMD_PLAY_W_INDEX, 0X001D);// siete
      delay(500);
      break;
      case 8:
      sendCommand(CMD_PLAY_W_INDEX, 0X001E);// ocho
      delay(500);
      break;
      case 9:
      sendCommand(CMD_PLAY_W_INDEX, 0X001F);// nueve
      delay(500);
      break;
      case 10:
      sendCommand(CMD_PLAY_W_INDEX, 0X0020);// diez
      delay(500);
      break;
      case 11:
      sendCommand(CMD_PLAY_W_INDEX, 0X0021);// once
      delay(500);
      break;
      case 12:
      sendCommand(CMD_PLAY_W_INDEX, 0X0022);// doce
      delay(500);
      break;
      case 13:
      sendCommand(CMD_PLAY_W_INDEX, 0X0023);// trece
      delay(500);
      break;
      case 14:
      sendCommand(CMD_PLAY_W_INDEX, 0X0024);// catorce
      delay(500);
      break;
      case 15:
      sendCommand(CMD_PLAY_W_INDEX, 0X0025);// quince
      delay(500);
      break;
  }
      
}



 
void mainCondition(String main, String description){

 Serial.println("Entra mainCondition");
 Serial.println("main es: "+main);


if(main.equals("Clear")){

if(description.equals("Clear sky")){
sendCommand(CMD_PLAY_W_INDEX, 0X0003); 
}

}

 if(main.equals("Clouds")){

      if(description.equals("few clouds")){
      
        sendCommand(CMD_PLAY_W_INDEX, 0X0004); 
      }
      
      if(description.equals("scattered clouds")){
      
        sendCommand(CMD_PLAY_W_INDEX, 0X0004); 
      }
      
      if(description.equals("broken clouds")){
      
        sendCommand(CMD_PLAY_W_INDEX, 0X0005); 
      }
      
      if(description.equals("overcast clouds")){
      
        sendCommand(CMD_PLAY_W_INDEX, 0X0005); 
      }

}

 if(main.equals("Rain")){
  if(description.equals("light rain") || description.equals("moderate rain")){
      
        sendCommand(CMD_PLAY_W_INDEX, 0X0006); 
      }else{
        sendCommand(CMD_PLAY_W_INDEX, 0X0007); 
      }
  

  
}

 if(main.equals("Thunderstorm")){
  sendCommand(CMD_PLAY_W_INDEX, 0X0008); 
}
 
 
}


  
