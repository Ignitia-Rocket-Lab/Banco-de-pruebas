# Banco-de-pruebas

Repositorio para los codiogs/recursos para el proyecto del banco de pruebas que incluye pero no esta limitado a: Mechero, Antenas, SD, Celda de carga.

Product Owner : Fer
Scrum Master : Ari
Equipo de desarollo : Stefano H.A. (stefano.herrejon@hotmail.com & A00571720@tec.mx)

Product Vision Statement:Un codigo/programa que permita la comunicación entre el operador y el banco de pruebas. La comunicación tiene que ser a una distancia considerable (+500 metros) para la seguridad del operador. El banco de pruebas tiene el objetivo de probar nuevos motores, combustibles para medir la potencia del equipo del grupo de IGNITIA. La comunicación tiene el objetivo de poder activar el motor/combustible a distancia, guardar los datos de potencia del cohete y transmitirlos. La seguridad consta de 2 switch, 1 fisico y 1 logico para evitar fallos. 

Requisitos : 
  Menu para opder operarar por monitor serial
  Transmisión
  Switch físico
  Switch logico(Voltaje)
  Cargar a memoria SD
  Lectura de la celda de carga
  
  Test : 
    Conexión de antenas/arduino
    ADC : Dar una carga de voltaje
    SD : Almacenamiento de datos

Historias de usuario (Prioridad -> Puntos) & Acceptance criteria:
  
  Como operador quiero tener un menu que me despliegue las opciones para controlar el banco de pruebas. (1, 3)
      Tener un menu con todas las opciones posibles
      Poder navegar entrar a una opciones y volver al menu, repetir*
    
  Como operador quiero poder comunicarme de un arduino a otro. (2, 13)
      Poder enviar mensajes de un arduiono a otro (Mensaje = Strings, datos,etc)
      Hacer acciones en base al mensaje enviado.
      Que la transmisión alcance +500 metros.
      
  Como operador quiero recibir acknowledge cuando envio mensaje. (3, 21)
      Recibir un mensaje de acknowledge por cada mensaje enviado.
      El mensaje de acknowledge tiene que estar personalizado dependiendo del tipo de mensaje enviado.
      
  Como operador quiero poder activar/desactivar un switch fisico. (4, 13)
      Por medio de un mensaje/transmisión quiero controlar un servomotor.
      Poder activar/desactivar un switch fisico con un servomotor.
      Poder girar 90* un servomotor al sentido del reloj para activar.
      Poder girar 90* un servomotor al sentido contrario del reloj para desactivar.

  Como operador quiero poder prender el combustible a distancia. (5,8)
      Con un mensaje/transmisión quiero poder dar un voltaje que prenda el combustible.
      Poder desactivar el voltaje por medio de un mensaje/transmisión.
      
  Como operador quiero que la fuerza del motor se mida. (6, 34)
      Recibir señal de una load cell.
      Poder interpretar los datos obtenidos e interpretarlos a fuerza.
      
  Como operador quiero que los datos del experimento se guarden en una tarjeta sd. (7, 5)
      Todos los datos recibidos por la load cell que se guarden con exito en una tarjeta sd.
      Que los datos de la load cell se puedan poner en un excell para hacer graficas de fuerza * tiempo.
      Poder quitar la sd sin miedo a la perdida de datos.
  
  Como operador quiero que los datos del experimento se transmitan. (8, 13)
      Que los datos de la load cell se transmitan (En vivo ?) del banco de pruebas al operador.
  
  Como operador quiero hacer un test a distancia para comprobar que los sistemas esten en funcionamiento. (9, 13)
      Que con un comando se corra un test del load cell, se guarden los datos y los datos se transmitan al operador.
       
PUNTOS = 0,1,2,3,5,8,13,21,34,55,89,?.

  Tasks : 
    1, Menu : 
      Opciones
      Movimientos del menu
      Funciones basicas
      Interface
      
