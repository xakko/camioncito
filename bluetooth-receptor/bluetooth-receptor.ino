/*primer programa para programar al bluetooth del arduino a traves de comandos AT por medio del monitor serie
en este ejemplo, se debe presionar el boton AT mientras se le de alimentacion, hasta que el led encienda y se apague

coche
https://es.aliexpress.com/item/32917152539.html?spm=a2g0o.productlist.0.0.6df270e1PWLxSk
conectado al arduino con un puente H L298N
https://es.aliexpress.com/item/32666061916.html?spm=a2g0s.9042311.0.0.274263c0ohlXQq
bluetooth hc-06
https://es.aliexpress.com/item/32342784842.html?spm=a2g0s.9042311.0.0.274263c0P0r4Z3
Arduino nano
https://es.aliexpress.com/item/32353404307.html?spm=a2g0s.9042311.0.0.274263c0zd2b4a
speaker-buzer para bocina
https://es.aliexpress.com/item/32814316897.html?spm=a2g0s.9042311.0.0.274263c078AnI6


patas del BT, de frente de izquierda a derecha del modulo BT-05
1. STATE - conectar al LED de estado
2. RXD - conectar al TXD del Arduino. Recibe los datos enviados desde la CPU
3. TXD - conectar al RXD del Arduino. Envia los datos que seran recibidos por la CPU
4. GND
5. VCC - 3.6 a 6V CC
6. KEY - Pull HIGH in AT/command mode

patas del BT, de frente de izquierda a derecha del modulo BT-06
1. RXD - conectar al TXD del Arduino. Recibe los datos enviados desde la CPU
2. TXD - conectar al RXD del Arduino. Envia los datos que seran recibidos por la CPU
3. GND
4. VCC - 3.6 a 6V CC

PWM: 3.5.6.9.10.11 (uno y nano)


recibira un valor de la forma
"+IV1512IH1512PDV1512DH1512DB-"
desglosado:
"IV1512....IH1512....P....DV1512....DH1512D....B"
izquierdo vertical un valor de 1000 a 2024
izquierdo horizontal un valor de 1000 a 2024
P boton izquierdo pulsado (envia N si no)
derecho vertical un valor de 1000 a 2024
derecho horizontal un valor de 1000 a 2024
B boton derecho pulsado (envia N si no)


joystick izquierdo
vertical
512 a 1024. ira para adelante
0 a 511. ira para atras
horizontal
512 a 1024. giro a la derecha. desde 768
0 a 511. giro a la izquierda. hasta 256


joystick derecho
adelante: dos luces
izquierda y derecha guiños (0 a 256 y 768 a 1024)
atras bocina

L298D:
              HHHHHH
  
Motor de  o  O      o Motor de 
giro      o       O o Ruedas
           ooo......
              123456  (Pines)


USB
    D13     - SENTIDO B DEL MOTOR 2 - Pin 3 de L298D
    D12     - SENTIDO A DEL MOTOR 2 - Pin 2 de L298D
    D11-PWM - VELOCIDAD DEL MOTOR 2 - Pin 1 de L298D
    D10-PWM - BUZZER
    D09-PWM - VELOCIDAD DEL MOTOR 1 - Pin 6 de L298D
    D08     - SENTIDO A DEL MOTOR 1 - Pin 5 de L298D
    D07     - SENTIDO B DEL MOTOR 1 - Pin 4 de L298D
    D06-PWM - LED IZQUIERDO
    D05-PWM - LED DERECHO
    D04     - TXD del BT
    D03-PWM - N/C
    D02     - RXD del BT 
    GND
    RST
    RXO
    TX1


*/

// en el ejemplo,
// el pin TXD (4) del BT va al pin 4 del arduino
// el pin RDX (2) del BT va al pin 2 del arduino


#include <SoftwareSerial.h>
#include "pitches.h"

// notas en la melodia:
int melodia[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// duraciones de las notas: 4 = un cuarto, 8 = un octavo, etc.:
int duraciones[] = {4, 8, 8, 4,4,4,4,4 };

int duracionNota;
int pausaNotas;

byte bai;
String ing; // valor ingresado del bluetooth en string
char let; // valor ingresado del bluetooth en caracter
//valores puros recibidos de cada potenciometro en String
String joyiv; // valor del ingreso proveniente del vertical del joystick izquierdo
String joyih; // valor del ingreso proveniente del horizontal del joystick izquierdo
String joydv; // valor del ingreso proveniente del vertical del joystick derecho
String joydh; // valor del ingreso proveniente del horizontal del joystick derecho
boolean lup = false;

//valores adaptados recibidos de cada potenciometro en entero
int iniv; //valores recibidos del joistick izquierdo vertical
int inih; //valores recibidos del joistick izquierdo horizontal
int indv; //valores recibidos del joistick derecho vertical
int indh; //valores recibidos del joistick derecho horizontal

//valores en las subrutinas de movimiento
int num;  //valor "puro" recibido desde el bluetooth
byte sld; //valor tranformado para la salida a los puertos (PWM)

byte joyiu;  //joistick izquierdo arriba
byte joyid;  //joistick izquierdo abajo
byte joyil;  //joistick izquierdo izquierda
byte joyir;  //joistick izquierdo derecha
byte joydu;  //joistick derecho arriba
byte joydd;  //joistick derecho abajo
byte joydl;  //joistick derecho izquierda
byte joydr;  //joistick derecho derecha

byte velDIA = 128; // velocidad de la rueda delantera izquierda hacia adelante
byte velDDA = 128; // velocidad de la rueda delantera derecha hacia adelante
byte velTIA = 128; // velocidad de la rueda traera izquierda hacia adelante
byte velTDA = 128; // velocidad de la rueda traera derecha hacia adelante
byte velDIB = 0; // velocidad de la rueda delantera izquierda hacia atras
byte velDDB = 0; // velocidad de la rueda delantera derecha hacia atras
byte velTIB = 0; // velocidad de la rueda traera izquierda hacia atras
byte velTDB = 0; // velocidad de la rueda traera derecha hacia atras

byte sal; //valor formado para la salida al control de los motores

// con control sobre las 4 ruedas solo modelos con 8 o mas PWM (no nano, no uno, no leonardo)
// en este caso esta hecho para un mega o un samd21
//ruedas delanteras (pwm en el conector superior y si es este solo, puede usarse con uno o nano)
//Asignacion de numeros de pines en el arduino para su conexion
byte m1E = 9; //PWM velocidad del motor 1
byte m1A = 8;  //sentido A del motor 1
byte m1B = 7;  //sentido B del motor 1
byte m2B = 13;  //sentido B del motor 2
byte m2A = 12;  //sentido A del motor 2
byte m2E = 11;  //PWM velocidad del motor 2

byte lzI = 6; //PWM del led izquierdo
byte lzD = 5;  //PWM del led derecho
byte buz = 10; //PWM buzzer - bocina

//D3 PWM libre

//2 y 4 oara el bluetooth:
SoftwareSerial miBT(2,4); //pines rx (2) & tx(4) del bluetooth

void setup() {

  pinMode(m1E, OUTPUT);
  pinMode(m1A, OUTPUT);
  pinMode(m1B, OUTPUT);
  pinMode(m2B, OUTPUT);
  pinMode(m2A, OUTPUT);
  pinMode(m2E, OUTPUT);
  pinMode(lzI, OUTPUT);
  pinMode(lzD, OUTPUT);
  pinMode(buz, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Listo");
  miBT.begin(38400);

  

}

void loop() {

  if (miBT.available()) {  // si esta ingresando algo por el bluetooth
    bai = miBT.read();     // guardo en ing el valor ingresado (character)
    Serial.println(bai);
    lup = true;     //desactivo bandera de salida
    if (bai == 43){ //si es el +, o sea el primer caracter de la secuencia
      ing = String(""); //inicializo el string que formare para la salida
      while(lup) {  //mientras lup este en true, armar el string
        if (miBT.available()) {  // si esta ingresando algo por el bluetooth
          bai = miBT.read();     // guardo en ing el valor ingresado (character)
          if (bai == 45){      // si se ha leido un "-", que es el ultimo caracter
            lup = false;        // activo bandera de salida para salir del bucle
          }else{                  //sino
            ing = ing + String(char(bai));    //voy armando el string de salida agregandole el nuevo caracter ingresado    
          }
        } // de BT disponible
      } //del while
      Serial.println(ing);
      //busco los 4 valores de las posiciones de los 2 joystick dentro del string
      joyiv = ing.substring(ing.indexOf("IV") + 2, ing.indexOf("IV") + 6);  //numero de cuatro cifras recibido detras de IV (izquierdo vertical)
      joyih = ing.substring(ing.indexOf("IH") + 2, ing.indexOf("IH") + 6);  //numero de cuatro cifras recibido detras de IH (izquierdo horizontal)
      joydv = ing.substring(ing.indexOf("DV") + 2, ing.indexOf("DV") + 6);  //numero  de cuatro cifras recibido detras de DV (derecho vertical)
      joydh = ing.substring(ing.indexOf("DH") + 2, ing.indexOf("DH") + 6);  //numero  de cuatro cifras recibido detras de DH (derecho horizontal)
      
      //les saco los 1000 agregados para que me mande un valor de 4 cifras
      iniv = joyiv.toInt() - 1000;
      inih = joyih.toInt() - 1000;
      indv = joydv.toInt() - 1000;
      indh = joydh.toInt() - 1000;

      //muestra de los valores de las 4 posiciones de los 2 joysticks
      Serial.println(iniv);
      Serial.println(inih);
      Serial.println(indv);
      Serial.println(indh);
      Serial.println("");

      //calculo los valores analogicos de las 8 salidas... en un rango intermedio (480-544) lo dejo "sin valor ingresado"
      if (iniv > 544)           //izquierdo arriba: avance
        adelante(iniv);
      if (iniv < 480)           //izquierdo abajo
        atras(iniv);
      if (iniv >= 480 && iniv <= 544)           //en el medio. frenado
        frenado();
      if (inih < 480)           //izquierdo izquierda
        izquierda(inih);
      if (inih > 544)           //izquierdo derecha
        derecha(inih);
      if (inih >= 480 && inih <= 544)           //en el medio. no gira. ruedas hacia adelante
        nogiro();
      if (indv > 544)           //derecho arriba - enciende las dos luces
        luces(indv);
      if (indv < 255)           //derecho abajo - suena la bocina
        bocina(indv);
      if (indv >= 480)           //no suena la bocina
        nobocina();
      if (indh < 480)           //derecho izquierda
        luzizq(indh);
      if (indh > 544)           //derecho derecha
        luzder(indh);
      if (indh >= 480 && indh <= 544 && indv <= 544)           //en el medio. no guiños
        noluces();
   
    
    }   // del "+"
    

  /*
    Serial.print(joyiv);
    Serial.print(" ... ");
    Serial.print(joyih);
    Serial.print(" ... ");
    Serial.print(joydv);
    Serial.print(" ... ");
    Serial.println(joydh);
  */
  
  }



}


void adelante(int num){
  sld = (num - 513) / 2;  //valor byte que saldra por el puerto PWM. a un valor entre 513 y 1024 le resto 513 y al resultado lo divido por 2 y tengo un valor de 0 a 255
  analogWrite(m1E, sld);  //salida en PWM para darle velocidad al avance
  //combinacion para que el puente haga el giro del motor "hacia adelante"
  digitalWrite(m1A, LOW);
  digitalWrite(m1B, HIGH);

  Serial.println("adelante");
}

void atras(int num){
  sld = (512 - num) / 2;  //valor byte que saldra por el puerto PWM. a 512 le resto un valor entre 0 y 512 y al resultado lo divido por 2 y tengo un valor de 0 a 255
  analogWrite(m1E, sld);  //salida en PWM para darle velocidad al retroceso
  //combinacion para que el puente haga el giro del motor "hacia atras"
  digitalWrite(m1A, HIGH);
  digitalWrite(m1B, LOW);
  Serial.println("atras");
}

void frenado(){
  analogWrite(m1E, 0);  // salida sin tension
  //combinacion para que el puente haga que el motor no gire
  digitalWrite(m1A, LOW);
  digitalWrite(m1B, LOW);
  Serial.println("frenado");  
}

void izquierda (int num){
  sld = (512 - num) / 2;
  analogWrite(m2E, 255);  //no PWM por mas que entre un valor. sale como un digital en 1 (analogico en 255)
  //combinacion para que el puente haga el giro del motor "hacia adelante"
  digitalWrite(m2A, HIGH);
  digitalWrite(m2B, LOW);
  Serial.println("izquierda");
}

void derecha (int num){
  sld = (num - 513) / 2;
  analogWrite(m2E, 255);  //no PWM por mas que entre un valor. sale como un digital en 1 (analogico en 255)
  //combinacion para que el puente haga el giro del motor "hacia atras"
  digitalWrite(m2A, LOW);
  digitalWrite(m2B, HIGH);
  Serial.println("derecha");
}

void nogiro(){
  analogWrite(m2E, 0);
  //combinacion para que el puente haga que el motor no gire
  digitalWrite(m2A, LOW);
  digitalWrite(m2B, LOW);
  Serial.println("nogiro");  
}

void luces (int num){
  sld = (num - 513) / 2;
  analogWrite(lzI, sld);
  analogWrite(lzD, sld);

}


//a partir de aca "jugar" con luces y bocina

void bocina (int num){  //bocina, ver jugar con tone
  //sld = (512 - num) / 2;
  //analogWrite(buz, sld);
  // bucle sobre las notas de la melodia:
  for (int unaNota = 0; unaNota < 8; unaNota++) {

    // para calcular la duracion de la nota, toma un segundo dividido por el tipo de nota 
    //ej. cuarto de nota = 1000 / 4, octavo de nota = 1000/8, etc.
    duracionNota = 1000/duraciones[unaNota];
    tone(buz, melodia[unaNota],duracionNota);
    tone(lzI, melodia[unaNota],duracionNota);
    tone(lzD, melodia[unaNota],duracionNota);
    
    // para separar las notas, ponemos un minimo de tiempo entre ellas.
    // la duracion de las notas + 30%:
    pausaNotas = duracionNota * 1.30;
    delay(pausaNotas);
    // parar la ejecucion de la nota:
    noTone(buz);
    noTone(lzI);
    noTone(lzD);
  }

}

void luzizq (int num){  //guiño led izquierdo
  sld = (512 - num) / 2;
  analogWrite(lzI, sld);
  delay(100);
  analogWrite(lzI, 0);
  delay(100);             //espero
}

void luzder (int num){  //guiño led derecho
  sld = (num - 513) / 2;
  analogWrite(lzD, sld);  //prendo el led
  delay(100);             //espero
  analogWrite(lzD, 0);    //lo apago
  delay(100);             //espero
}

void noluces(){
  analogWrite(lzI, 0);
  analogWrite(lzD, 0);
  Serial.println("luces apagadas");  
}

void nobocina(){
  analogWrite(buz, 0);
  noTone(buz);
  Serial.println("sin bocina");  
}
