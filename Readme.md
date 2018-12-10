### Links para IDE Arduino
http://arduino.esp8266.com/stable/package_esp8266com_index.json

https://github.com/bblanchon/ArduinoJson

Introducción

-------------


El proyecto trata sobre una nube de algodón que contendrá la electrónica necesaria para dictar por medio de una voz pregrabada la predicción meteorológica actual y a futuro realizando un show de luces relativo a la situación. Esta información será recibida a través de internet y podrá ser configurada por medio de un smartphone por medio de wifi o bluetooth.

Desarrollo
La información formateada se recibirá en un módulo NodeMCU a través de una petición a un servicio de OpenWeatherMap que devolverá un archivo JSON. Esta información será procesada y según las reglas establecidas la electrónica dictara la prediccion y realizará un show de luces además de dictarse en voz alta.


Nube Climatologica
https://lh6.googleusercontent.com/XJ0VMgwvNYQPqAptHYOXUW8JBV3aBWklKoBumIrQ9wY3uMBzvQlkGui6oQGEIZ-KVyWTQQLKPxXpMFzNkioIPip5NNzhZaU3AmGwUw9oA9ThvHpu0DPbx2R9ewLny6ZRztKyV3dL

Instituto Tecnológico de Mexicali


Nueva entrada
https://programacion.net/articulo/pronostico_del_tiempo_utilizando_openweathermap_mediante_php_2035


Nube Climatologica

Taller de investigación II

 








Lopez Meza Angel Armando

14490848

Ingeniería en sistemas computacionales

24/09/2018






 


 

































Nube climatológica



Introducción

El proyecto trata sobre una nube de algodón que contendrá la electrónica necesaria para dictar por medio de una voz pregrabada la predicción meteorológica actual y a futuro realizando un show de luces relativo a la situación. Esta información será recibida a través de internet y podrá ser configurada por medio de un smartphone por medio de wifi o bluetooth.

Desarrollo
La información formateada se recibirá en un módulo NodeMCU a través de una petición a un servicio de OpenWeatherMap que devolverá un archivo JSON. Esta información será procesada y según las reglas establecidas la electrónica dictara la prediccion y realizará un show de luces además de dictarse en voz alta.




Opciones de clima
Dia soleado; Se iluminara por completo la nube
Dia nublado; La iluminación será tenue
Chubascos: Se reproducirá un sonido de lluvia y se simularán algunos rayos.
Tormenta; Se reproducirá un sonido de tormenta y se simularán más rayos.

 :



Para dictar la voz se utilizará el módulo Catalex Serial MP3 Player, controlado por el NodeMCU por comandos hexadecimales. El módulo amplificador será Stereo y contará con una bocina de 3W.


Módulo MP3
El módulo Catalex MP3 Serial Player será controlado desde comandos hexadecimales que permitirán acceder a los archivos de manera indexada.

Para la iluminación se usarán leds con una interfaz electrónica controlada por el módulo NodeMCU.


Innovación

La innovación de este producto es mostrar de una manera diferente las predicciones climáticas. A diferencia de las predicciones por web, noticias o smartphone esta es más interactiva, vistosa y puede resultar hasta entretenida.


Materiales
Los requerimientos de los componentes necesarios para el desarrollo de la nube climatológica fueron cuidados para cumplir con los materiales ya existentes.


Lista de materiales:

NodeMCU
Catalex Serial MP3 Player
MircoSD 2GB
LM389
Bocina 3W
Leds blanco ultrabrillante
Leds azul ultrabrillante
Adaptador de corriente AC o circuito de carga de batería
Algodón
Tela blanca


