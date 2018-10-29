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
 
void setup() {

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



sendCommand(CMD_PLAY_W_INDEX, 0X0013);// condicion climatica actul
delay(3300);
sendCommand(CMD_PLAY_W_INDEX, 0X000E);// la temperatura es de
delay(1500);

Serial.println("pide"+temperatura);





 /////////////////////////separarNumero(temperatura);
delay(500);
sendCommand(CMD_PLAY_W_INDEX, 0X000B);// grados
sendCommand(CMD_PLAY_W_INDEX, 0X000C);// centigrados
delay(3200);
sendCommand(CMD_PLAY_W_INDEX, 0X0012);// nubes al
delay(3300);
 

float tempf = kelvinToCelcius(temperatura);
String tempSf = String(tempf);


char temperaturaChar [4];
tempSf.toCharArray(temperaturaChar, 4);

Serial.println("valor de tempSf: "+tempSf);

dictar(tempSf);

Serial.println("priimer valor d e la cadena"+temperaturaChar[0]);
delay(10);
Serial.println("segundo valor d e la cadena"+temperaturaChar[1]);
delay(100);

  Serial.println("Cerrando la conexión");
 
  while(1){
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

  char pd = cadena.charAt(0);
  char sd = cadena.charAt(1);

  String auxiliar1= String(pd);
  String auxiliar2= String(sd);
  float pdEntero1 = auxiliar1.toInt();
  float sdEntero1 =  auxiliar2.toInt();
  int pdEntero = auxiliar1.toInt();

  Serial.println("temperatura es:");
  Serial.println(cadena);
         Serial.println("Segunod igito entero:**********");
         Serial.println(sdEntero);    
         Serial.println("*******************");
    if(sdEntero==0)   //---MKSEDA0
      {
   
      //terminacion cero
         Serial.println("Primer igito entero:**********");
         Serial.println(pdEntero);    
         Serial.println("*******************");
          switch(pdEntero){  //-MKSTE0

            case 10:
            sendCommand(CMD_PLAY_W_INDEX, 0X0020);// dies
                Serial.println("****************************roger 110");
            break;
            
            case 20:
            Serial.println("****************************roger");
            sendCommand(CMD_PLAY_W_INDEX, 0X0027);// veinte
            
            break;

            case 30:
            sendCommand(CMD_PLAY_W_INDEX, 0X0029);// treinta
            break;
            
            case 40:
            sendCommand(CMD_PLAY_W_INDEX, 0X002B);// cuarenta
            break;
            
            case 50:
            sendCommand(CMD_PLAY_W_INDEX, 0X002D);// cincuenta
            break;
            
            case 60:
            sendCommand(CMD_PLAY_W_INDEX, 0X002F);// sesenta
            break;
        
            case 70:
            sendCommand(CMD_PLAY_W_INDEX, 0X0031);// setenta
            break;
            
            case 80:
            sendCommand(CMD_PLAY_W_INDEX, 0X0033);// ochenta
            break;
            
            case 90:
            sendCommand(CMD_PLAY_W_INDEX, 0X0035);// noventa 
            break;


          } // din de --MKSTE0



        
      }else{                    // -- MKSEDA0  MKSEDA0E
        //normal

///Serial.println("intneto de conversion segundo digito: ");
                

                                switch(pdEntero){     // MKSpdEntero
                                  
                                  case '1':
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0026);// diesi
                                  break;

                                  case '2':
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0028);// veinti
                                  break;
                                  
                                  case '3':
                                  sendCommand(CMD_PLAY_W_INDEX, 0X002A);// treintai
                                  Serial.println("Treintai***********");
                                  break;
 
                                  case 4:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X002C);// cuarentai
                                  break;

                                  case 5:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X002E);// cincuentai
                                  break;

                                  case 6:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0030);// sesentai
                                  break;

                                  case 7:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0032);// setentai
                                  break;

                                  case 8:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0034);// ochentai
                                  break;

                                  case 9:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0036);// noventai
                                  break;

                                }// FIN DEMKSpdEntero 

                                delay(500);
                                
                                switch(sdEntero){     // MKSpdEntero
                                  
                                  case 1:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0017);// uno
                                  break;

                                  case 2:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0018);// dos
                                  break;
                                  
                                  case 3:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X0019);// tres
                                  break;

                                  case 4:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001A);// cuatr
                                  break;

                                  case 5:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001B);// cinco
                                  break;

                                  case 6:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001C);// seis
                                  break;

                                  case 7:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001D);// siete
                                  break;

                                  case 8:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001E);// ocho
                                  break;

                                  case 9:
                                  sendCommand(CMD_PLAY_W_INDEX, 0X001F);// nueve
                                  break;
         

                                }// FIN DEMKSpdEntero 

                  }// fin de MKSEDA0E

}//fin dictar



 



  
