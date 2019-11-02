/* este programa leera los datos ingresados en los pines analogicos A0-A3 y sus botones en pines 2 y 4
 * y los enviara a traves del bluetooth conectado a los pines 7 y 8 
patas del BT, de frente de izquierda a derecha del modulo BT-06

el formato del texto final enviado sera:
"+IV1512IH1512PDV1512DH1512B-"

izquierda vartical con valor, izquierda horizontal con valor, P es boton izquierdo pulzado (sino, seria N)
derecha vartical con valor, derecha horizontal con valor, B es boton dercho pulzado (sino, seria N)
los valores de salida van de 1000 a 2024. se les suma 1000 para que tengan un valor de cuatro cifras
en el receptor habria que restarselos a cada resultado

conexiones del bluetooth
1. ESTADO - conecta al led de estado
2. RXD - conectar al TXD del Arduino. Recibe los datos enviados desde la CPU
3. TXD - conectar al RXD del Arduino. Envia los datos que seran recibidos por la CPU
4. GND
5. VCC - 3.6 a 6V CC
6. KEY - habilita al modulo para la programacion por comandos AT

 el pin 1 - ESTADO ... no se usa en este proyecto
 el pin 2 - RDX del BT va al pin 2 del arduino
 el pin 3 - TXD del BT va al pin 3 del arduino
 el pin 4 - va a GND
 el pin 5 - va a 5V
 el pin 6 - KEY ... no se usa en este proyecto. se supone q los dos modulos ya estan acoplados de antemano

conexiones de los joysticks

joystick izquierdo

-SW a pin digital 8
-URY a pin analogico A0
-URX a pin analogico A1
+5V
-GND

joystick derecho

-SW a pin digital 7
-URY a pin analogico A2
-URX a pin analogico A3
+5V
-GND

*/

#include <SoftwareSerial.h>

SoftwareSerial miBT(2,3); //pines rx (2) & tx(3) del bluetooth

//joysticks
byte potpin0 = 0; //Definir el joystick izquierdo de entrada dirección horizontal VRX A0 final.
byte potpin1 = 1; //Definir el joystick izquierdo de entrada dirección vertical VRY A1 final
byte potpin2 = 2; //Definir el joystick derecho de entrada dirección horizontal VRX A2 final
byte potpin3 = 3; //Definir el joystick derecho de entrada dirección vertical VRY A3 final.
int val0;  //valor leido del joystick izquierdo horizontal
int val1;  //valor leido del joystick izquierdo vertical
int val2;  //valor leido del joystick derecho horizontal
int val3;  //valor leido del joystick derecho vertical

//strings
String let0;  //string del valor del joystick izquierdo en el movimiento vertical
String let1;  //string del valor del joystick izquierdo en el movimiento horizontal
String let2;  //string del valor del joystick derecho en el movimiento vertical
String let3;  //string del valor del joystick derecho en el movimiento horizontal
String let7;  //boton izquierdo
String let8;  //boton derecho
String paln;  //string final formado
String palv = "NNNNNN"; //string "anterior" formado para comparacion con el nuevo ingresado
String uno; //caracter que saldra por el bluetooth tomado del string formado

void setup() {
  pinMode(8, INPUT); //configuro al pin 8 como pulsador (trigger del joystick 1)
  pinMode(7, INPUT); //configuro al pin 7 como pulsador (trigger del joystick 2)
  Serial.begin(9600);
  Serial.println("Listo");
  miBT.begin(38400);

}

void loop() {

  //lectura de los puertos analogicos A0 a A3
  //valores INVERSOS leidos de los joysicks (1024 - el valor). de abajo e izquierda (0-0) hacia arriba y derecha (1023-1023)
  //les sumo 1000 para que tengan 4 (cuatro) cifras/caracteres si o si. luego habria que restarselos en el receptor
  val0 = 2024 - analogRead(potpin0);
  val1 = 2024 - analogRead(potpin1);
  val2 = 2024 - analogRead(potpin2);
  val3 = 2024 - analogRead(potpin3);

  //armo las cuatro variables para las salidas
  let0 = "IV" + String(val0);
  let1 = "IH" + String(val1); 
  let2 = "DV" + String(val2);
  let3 = "DH" + String(val3);

  //pulsado de botones izquierdo y derecho
  let7 = "N";
  let8 = "N";
  if (digitalRead(7) == HIGH)
  let7 = "P";
  if (digitalRead(8) == HIGH)
  let8 = "B";

  //concatenacion para el string final a ser enviado con todos los mini-strings formados
  
  paln = String("+") + String(let0) +  String(let1) + String(let8) + String(let2) + String(let3) + String(let7) + String("-");

//monitoreo y envio
//  if (!(palv==paln)){ //me fijo si no es igual al anterior para hacer el envio
    Serial.println(String(paln)); //envio el valor al monitor
    //envio byte a byte del string de salida a traves del bluetooth
    for(int i = 0; i <= 30; i++){ // recorrido de las posiciones del string de 0 a 30, la longitud del string, 1 por 1
      uno = paln.substring(i, i + 1); //cada caracter tomado de la posicion actual del string recorrido
      miBT.print(String(uno));  //mando al blutooth el caracter
      Serial.println(String(uno));  //monitoreo 
      delay(5); //retardito
    }
    
    palv=paln; //almaceno el valor actual para la comparacion con el proximo
    delay(100); //retardo
//  } // fin del if de no igual al anterior

}
