# Casa Domótica
Esto es mi proyecto integrador de la materia Técnicas Digitales 2 de 4to año de la carrera Ingeniería Electrónica de la UTN FRA.

[Informe Final del Proyecto](https://drive.google.com/file/d/1HTl4FN6zS-q5GI3TRCP4-OzRvRgcNZSA/view?usp=sharing)

![Persiana moviendose](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/persiana2.gif)

## Concepto

La idea es controlar los sensores y actuadores de una maqueta de una casa desde una página web.

![Diagrama](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/Diagrama.jpg)

![Casa](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/casa.jpg)

# Funcionalidades

## Control y automatización de las luces interiores

Desde la página web se pueden prender y apagar las luces que se soliciten. Además éstas luces pueden programarse para que estén prendidas en una franja horaria sin intervención humana. La programación se puede activar y desactivar según se desee.

## Control y automatización de las persianas

Cada habitación contiene una persiana, éstas últimas pueden abrirse o cerrarse desde la página web. Así como las luces, las persianas también pueden programarse para que estén abiertas en una franja horaria configurable.

![Persiana moviendose](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/persiana1.gif)

## Control automático de luces exteriores

En el exterior de la casa hay 2 luces LED, las cuales solo se encienden cuando la cantidad de luz baja de cierto umbral.

![Diagrama](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/exterior.jpg)

## Monitoreo con sensores de movimiento

Se puede activar el sistema de alarmas, lo que provoca que cuando se detecta algún movimiento dentro de la casa aparece una alerta en la página web.

![Sistema de alarma](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/alerta.gif)

# Hardware

## Placa de desarrollo

La placa de desarrollo fue elegida por los profesores de la materia, la cuál fue LPC825-BRK. Es un microcontrolador fabricado por NXP, de Cortex-M0+ ARM, tiene 38 pines, entre otros datos.

## Luces

Para las luces interiores y exteriores se utilizaron LEDs LH5CW que poseen una gran iluminación.

## Persianas

Para las persianas opté por utilizar motores paso a paso para ahorrarme el tener que utilizar sensores finales de carrera. Los motores elegidos fueron los 28BYJ-48, que a su vez eran comandados a través de los drivers ULN2003.

## Sensores de movimiento

Para los sensores de movimiento elegí el HC-SR501 PIR. Estos
sensores miden radiación infrarroja y envían una señal de 3,3V si detectan una variación significante (La sensibilidad se puede ajustar con un preset). Se alimentan con 5V.

Debido a que son 4 sensores, uno por cada
habitación, necesitaba usar 4 pines del microcontrolador.
Esto me fue imposible por la cantidad de pines
disponibles así que decidí utilizar solamente un pin para
enviar la señal al microcontrolador. Para lograr esto
utilicé el CD4072 que es un CI de 2 compuertas OR con
4 entradas cada una.

## LDR

Para detectar la cantidad de iluminación exterior utilicé un LDR de 10k Ohms. Disené un divisor de tensión simple y midiendo la tensión por ADC media la intensidad.

## Módulo WiFi

Para lograr la conectividad WiFi utilicé el módulo ESP01S. Desde el microcontrolador enviaba por comandos AT por UART al módulo y de esta forma envio y recibo los datos y solicitudes.

# Software

## Frontend

Para el frontend utilicé React.

![Página](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/web.jpg)

## Backend

Para el backend utilicé un servidor Node.js y express, el cual tuve que alojar en Glitch porque el Módulo WiFi ESP01S no soporta solicitudes HTTPS. La base de datos que terminé usando fue firebase ya que ya estaba familiarizado y no manejaba información sensible.

## Aplicación de escritorio

Diseñe una App de escritorio con Electron.js para enviar los datos de WiFi (SSID y contraseña) al microcontrolador a través de UART por USB para almacenar los datos en la memoria Flash.

![App](https://github.com/lucasigna/casa-domotica-td2/blob/main/media/electron.jpg)


