# Este repositorio corresponde al TP 2 - Asignación dinámica de memoria del grupo 2 de la materia RTOS II

## Los integrantes del grupo son Agustín Navari y Juan Pablo Touron

El repositorio contiene el proyecto para compilar en STM32Cube IDE. El proyecto cuenta con:

- Una tarea para el botón
- Un objeto activo de UI (que tiene una queue asociada y una tarea asociada que se crea y se destruye dinámicamente) 
- Un objeto activo para cada LED (que tiene un color asociado y una tarea asociada que se crea y se destruye dinámicamente)
- Una queue global para intercambio de mensajes entre la interfaz de usuario y los LEDs

El proyecto se realizó con una placa STM32F446RE. Se utilizaron el pulsador y el LED integrados en la placa por lo que no pueden verse distintos LEDs encendidos de
acuerdo al tiempo de presión del botón. Pero si se muestra por mensaje en consola, qué led está prendido y qué tipo de pulsación se realizó. El tiempo de encendido
del LED está hardcodeado en 1 segundo. 

Las tareas de la interfaz de usuario y del LED se crean y destruyen dinámicamente. Los mensajes de la UI a los LEDs se guardan con memory pool desde la UI, son
leídos y atendidos desde el LED, y luego la memoria es liberada desde la UI.

Link al video de evidencia de funcionamiento: https://drive.google.com/file/d/1ti4tutg6x8PYQv6Pq5-WsN7pTRh11Ve3/view?usp=drive_link
