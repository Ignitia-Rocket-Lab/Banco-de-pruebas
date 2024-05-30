unsigned long startMillis;  // Tiempo inicial
unsigned long currentMillis; // Tiempo actual
const unsigned long period = 5000; // Tiempo de espera en milisegundos (5 segundos)
bool arduinos; //True = Pilotado aka Persona, False = banco de prueba
char input = '0';

void setup() {
  // Inicializar la comunicación serial
  Serial.begin(115200);
  Serial.println("Bienvenido al banco de pruebas de <Ignitia>. Presiona cualquier tecla en los siguientes 10 segundos para comenzar. Si no se ira a recepcion de datos.");
  startMillis = millis();  // Captura el tiempo inicial 
  char input = 0;
  arduinos = esperadoInput(); 
  menu();
}

void loop() {
  // Comprobar si hay datos disponibles en el puerto serial
  if(arduinos == true){
    while (!Serial.available()) {
    // wait for user input
    }
    // Leer el dato del puerto serial
    char input = Serial.read();
  }

    
  // Llamar a la función correspondiente según el input
  if (input != '\n'){
    switch (input) {
      case '0':
        recepcionDatos();
        break;
      case '1':
        conexionAntenas();
        break;
      case '2':
        testSistemas();
        break;
      case '3':
        swFisico();
        break;
      case '4' :
        prenderMotor();
        break;
      case '5' :
        leerSD();
        break;
      default:
        Serial.println("Opción no válida. Intente nuevamente.");
        break;
    }//Switch
    menu();
  
  }//If input
}//Loop

void menu(){
  Serial.println("Menu:");
  Serial.println("Por defecto se ira a recepcion de datos. ((0)");
  Serial.println("1. Checar la conexión de antenas");
  Serial.println("2. Test de sistemas");
  Serial.println("3. Switch fisico");
  Serial.println("4. Prender motor");
  Serial.println("5. Leer SD");

} //Menu

// Definición de las funciones

bool esperadoInput() {
  bool inputReceived = false;
  bool salida;

  while(currentMillis <= startMillis + period){
    currentMillis = millis();  // Actualiza el tiempo actual
  
    // Verifica si hay datos disponibles en el puerto serial
    if (Serial.available() > 0) {
      char input = Serial.read();
      Serial.println("Eres el piloto");
      inputReceived = true;
      break;
    }
        
  }
  return inputReceived;
}//Checar si es el arduino tripulado

void recepcionDatos() {

  Serial.println("Reciviendo datos");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputRD = 123;


  while (inputRD != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputRD = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputRD);

  }//While RD != 0
  arduinos = true;

}//Recepcion de datos 


void conexionAntenas() {

  Serial.println("Función conexion de antenas.");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputCA = 123;


  while (inputCA != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputCA = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputCA);

  }//While CA != 0

}//Conexion de antenas

void testSistemas() {
  Serial.println("Test de sistemas.");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputTS = 123;


  while (inputTS != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputTS = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputTS);

  }//While TS != 0

} //Test de sistemas

void swFisico() {
  Serial.println("Switch fisico.");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputSW = 123;


  while (inputSW != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputSW = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputSW);

  }//While SW != 0

}//Switch fisico

void prenderMotor() {
  Serial.println("Prender motor");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputPM = 123;


  while (inputPM != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputPM = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputPM);

  }//While PM != 0

}//Prender motor

void leerSD() {
  Serial.println("Lectura de tarjeta SD");
  Serial.println("0, regresar al menu. Esperando input. ");

  char inputSD = 123;


  while (inputSD != '0'){
    while (!Serial.available()) {
      // wait for user input
    }

    inputSD = Serial.read();

    Serial.print("Carácter ingresado: ");
    Serial.println(inputSD);

  }//While SD != 0

}//Leer sd