# camioncito
Sketchs de Arduino para manejar un camioncito con puente H a traves de bluetooth

Aqui hay un proyecto de un camion controlado por bluetooth.
La idea es mostrar el codigo de control de envio y recepcion de string bluetooth con arduino,
control de puente H-
El sketch bluetooth-receptor esta en un Arduino Nano conectado a un puente H L298N
y a un modulo HC-06 de bluetooth.
Ademas de a un par de leds y un speaker-boozer para "jugar" un poco mas.
LOS DATOS DE LA BOCINA NO SON MIOS. Tanto codigo de reproduccion como notas estan tomadas de internet.

A traves del bluetooth recibo la señal de un string enviado de otro bluetooth con un Arduino
con algun dispositivo de envio. En este "enviador", dependiendo de la forma de entrada,
"armo" el strig y lo envio por el bluetooth byte a byte.

TODO lo realizado esta comentado, asi los puedo guiar en cada accion que realizo en los programas.
CUALQUIER duda consulten, me gusta que la gente sepa y despejar CUALQUIER TIPO de duda.

camioncito:
https://es.aliexpress.com/item/32917152539.html?spm=a2g0s.9042311.0.0.274263c0QWyFeR
solo tiene dos motores: uno para hacer andar las ruedas, el otro para girar las ruedas delanteras
nada mas trae. todo hay que armarlo. el puente H lo puse con el circuito hacia abajo justo en el espacio
que queda sobre las ruedas traceras. calza justo. el arduino y el bluetooth quedan algo ajustados en el capot
del camion. esta hecho con un Nano. tal vez con un mini entr mas cómodo.

modulos bluetooth
https://es.aliexpress.com/item/32523867394.html?spm=a2g0o.productlist.0.0.27786ce2dZghPl
en particular los HC-06 ya que son configurables con "botoncito" y puede ser configurado como master o
como slave sin problemas. 
como configurar y "aparear" dos modulos no esta en el proyecto ya que hay varios video que explican muy bien como hacerlo

puente H
https://es.aliexpress.com/item/32666061916.html?spm=a2g0s.9042311.0.0.274263c0b5rDaV
solo conectar a sus entradas las 6 salidas del arduino y los dos motores del camioncito a sus salidas

los emisores
2 modulos de joystick (uno del lado izquierdo y otro del lado derecho):
https://es.aliexpress.com/item/32898205502.html?spm=a2g0o.productlist.0.0.54631a3bqfvRnw
conectados a un arduino y el arduino a un bluetooth

estabilizador-acelerometro con mpu6050. con este armo como el "guante controlador" que tiene un video en youtube
https://es.aliexpress.com/item/2035920870.html?spm=a2g0o.productlist.0.0.7f256b2ebB94n7
hay mucha info para saber como trabaja 
en particular con este solo hago el avance, marcha atras y los dos giros (izquierda y derecha) para
controlar al camioncito. utilizo las librerias del mpu y transformo directamente los valores raw obtenidos
y trabajo con ellos directamente


la idea basica es armar un string en los enviadores, luego enviarlo a traves del bluetooth y que su compañoero lo reciba
ya recibido, el camion lo decodifique y actue en base a loe recibido

el string tiene la forma
"+IV1512IH1512PDV1512DH1512B-"
"+" para indicar inicio del string
IVxxxx datos del joystick izquierdo en movimiento vertical
IHxxxx datos del joystick izquierdo en movimiento horizontal
P pusado del boton del joystick izquierdo
DVxxxx datos del joystick derecho en movimiento vertical
DHxxxx datos del joystick derecho en movimiento horizontal
P pusado del boton del joystick derecho
"-" fin del string

el valor xxxx es el formado por las distintas entradas. la idea es mandar el valor analogico recibido de cada joystick
a estos valores les agrego 1000 para que me de un numero de 4 cifras siempre. en cuensecuencia, en la decodificacion del string
en el receptor, se los resto
otra cosa que hago es que los valores del joystick con los pines hacia arriba me dan de 0 a 1023 de arriba hacia abajo
y de derecha hacia izquierda, y no me gusta (a mi). Por esto hago 1023 -  el valor leido, asi me dan los inversos
y tengo 0 a 1023 de izquierda hacia derecha y de abajo hacia arriba.
junto los strigs armados en base a las lecturas y los concateno en un string largo (30 caracteres)
luego recorro el string byte a byte y lo voy enviando por el bluetooth (byte a byte)


recepcion: recibo el string "encontrando" el caracter "+" de inicio de string
una vez localizado, armo todo lo recibido hasta encontrar el caracter "-" de final de string
decodifico el string ubicando los 4 caracteres siguientes a "IV", "IH", "DV" y "DH" con "indexOf"
con los valores obtenidos, actuo en cosecuencia (avanzo, retrocedo, giro izquierda, giro derecha, luces, bocina...)



por CUALQUIER CONSULTA mi correo: xakko@yahoo.com.ar
