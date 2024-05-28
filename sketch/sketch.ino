void setup() {
  // Inicializar la comunicación serial
  Serial.begin(115200);
}

void loop() {
  // Comprobar si hay datos disponibles en el puerto serial
  while (!Serial.available()) {
    // wait for user input
  }
  // Leer el dato del puerto serial
  char input = Serial.read();
    
  // Llamar a la función correspondiente según el input
  if (input != '\n'){
    switch (input) {
      case '1':
        funcion1();
        break;
      case '2':
        funcion2();
        break;
      case '3':
        funcion3();
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
  Serial.println("1. Checar la ");
  Serial.println("2. Switch físico");
  Serial.println("3. Prender motor");
}

// Definición de las funciones
void funcion1() {
  Serial.println("Función 1 ejecutada.");
  // Añade aquí el código de la función 1
}

void funcion2() {
  Serial.println("Función 2 ejecutada.");
  // Añade aquí el código de la función 2
}

void funcion3() {
  Serial.println("Función 3 ejecutada.");
  // Añade aquí el código de la función 3
}
