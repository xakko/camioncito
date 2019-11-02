/*

https://naylampmechatronics.com/blog/45_Tutorial-MPU6050-Aceler%C3%B3metro-y-Giroscopio.html

Calculando el ángulo de inclinación con el acelerómetro del MPU6050
Si tenemos en cuenta que la única fuerza que actúa sobre el sensor es la fuerza de la gravedad.
Entonces los valores que obtenemos en las componentes del acelerómetro corresponden a la gravedad
y los ángulos de la resultante serán la inclinación del plano del sensor,
puesto que la gravedad siempre es vertical.

Para entenderlo mejor, asumiremos que estamos en un plano X-Z e inclinamos el MPU6050 un ángulo θ,
dicho ángulo se calcula de la siguiente forma:

    /
 gx/|\
  / |θ\gz
 /  g
/) θ
--------------

θ = atan(gx/gz)

angulo de inclinacion 2D

θx = tan (exp,-1) (ax / (raiz(ay2 + az2))
θy = tan (exp,-1) (ay / (raiz(ax2 + az2))

Lo anterior nos sirve para calcular el ángulo en un plano 2D,
pero para calcular los ángulos de inclinación en un espacio 3D tanto en X como en Y
usamos las siguientes formulas:



 ecuacion angulo inclinacion 3D

 

Tener en cuenta que estamos calculando el ángulo de inclinación, si deseáramos el ángulo de rotación es decir por ejemplo el ángulo que rota el eje x en su mismo eje, entonces en las formulas necesitamos cambiar el ay por el ax y viceversa.
 */

// Librerias I2C para controlar el mpu6050
// la libreria MPU6050.h necesita I2Cdev.h, I2Cdev.h necesita Wire.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// librerias del bluetooth
#include <SoftwareSerial.h>
SoftwareSerial miBT(2,3); //pines rx (2) & tx(3) del bluetooth


// La dirección del MPU6050 puede ser 0x68 o 0x69, dependiendo 
// del estado de AD0. Si no se especifica, 0x68 estará implicito
MPU6050 sensor;

// Valores RAW (sin procesar) del acelerometro  en los ejes x,y,z
int ax, ay, az;

// inicializar estabilizacion.
// ver en el monitoreo los valores cuando esta en posicion "0" y ponerlos aqui

//inicializacion de los estados 0 en posicion horizontal del mpu6050
//depende de cada mpu.
// poner los dos valores en 0. ver en pantalla cuanto da estando horizontal. y ponerles luego ese valor
float cerox = -5;
float ceroy = 0;

//valor de la posicion en angulo (en grados) obtenodo por formula desde el valor obtenido por la libreria
int accel_ang_x;
int accel_ang_y;
// radianes a angulo
float rad_ang = 180 / 3.14;

//maximma inclinacion de los angulos. de -90 a +90 pasa de un vertical al opuesto
//para un brazo puede ir de -60 a +60 . En posicion -60 seria 0 y en posicion +60 seria 1024 en valores analogicos. el 0 seria 512
int max_x = 60;
int max_y = 60;
//y sus multiplicadores para llevarlos a 1024
float mul_x;
float mul_y;

String let0;
String let1;

//variables de salida del BT
String paln;
String palv = "NNNNNN";
String uno;


void setup() {
  Serial.begin(57600);    //Iniciando puerto serial
  Wire.begin();           //Iniciando I2C  
  sensor.initialize();    //Iniciando el sensor

  if (sensor.testConnection()) Serial.println("Sensor iniciado correctamente");
  else Serial.println("Error al iniciar el sensor");

  Serial.begin(9600);
  Serial.println("Listo");
  miBT.begin(38400);

  mul_x = 512 / max_x;
  mul_y = 512 / max_y;
  
}

void loop() {
  // Leer las aceleraciones
  // y las pasa a las variables ax, ay, az
  sensor.getAcceleration(&ax, &ay, &az);
  //Calcular los angulos de inclinacion: de -max a +max
  //atan arcotengente
  //sqrt raiz cuadrada
  //pow elevar el primer numero a la potencia del segundo
  // 
  accel_ang_x=(atan(ax/sqrt(pow(ay,2) + pow(az,2))) * rad_ang - cerox);
  accel_ang_y=(atan(ay/sqrt(pow(ax,2) + pow(az,2))) * rad_ang - ceroy);


  //preparo para la salida analogica
  // lo adapto al angulo maximo y lo ubico sobre 1000
  accel_ang_y=(accel_ang_y) * mul_y + 1512;
  accel_ang_x=(accel_ang_x) * mul_x + 1512;

  //control de limites inferiores y superiores
  if (accel_ang_x <= 1000)
    accel_ang_x = 1000;
  if (accel_ang_x >= 2024)
    accel_ang_x = 2024;
  if (accel_ang_y <= 1000)
    accel_ang_y = 1000;
  if (accel_ang_y >= 2024)
    accel_ang_y = 1024;


  //armado de strins de salida al BT
  let0 = "IV" + String(accel_ang_y);
  let1 = "IH" + String(accel_ang_x);

//"+IV1512IH1512PDV1512DH1512B-"
// no usa el control derecho, asi que sale con valores reseteados
 paln = String("+") + String(let0) +  String(let1) + String("NDV1512DH1512N-");

  
  //Mostrar los angulos
  Serial.print(ax);
  Serial.print(" ... ");
  Serial.print(ay);
  Serial.print(" ... ");
  Serial.print(az);
  Serial.print(" ... ");
  Serial.print("   Inclinacion en X: ");
  Serial.print(accel_ang_x); 
  Serial.print("   Inclinacion en Y:");
  Serial.println(accel_ang_y);

  //monitoreo y envio
//  if (!(palv==paln)){ //me fijo si es igaual al anterior para hacer el envio
    
    //envio del string por Bluetooth
    for(int i = 0; i <= 30; i++){
      uno = paln.substring(i, i + 1);
      miBT.print(String(uno));
      //Serial.println(String(uno));
      delay(5);
    }
    Serial.println(String(paln)); //envio el valor al monitor
    palv=paln; //almaceno el valor actual para la comparacion con el proximo
    delay(200);
//  }

}
