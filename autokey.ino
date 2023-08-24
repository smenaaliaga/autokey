#include "Keyboard.h"
#include <string.h>
#include <SPI.h>
#include <SdFat.h>

SdFat sd;
SdFile file;

const int pinLed = 3;
const int chipSelect = 2;
// Definición de buffer
#define MAX_MESSAGE_LENGTH 128
char message[MAX_MESSAGE_LENGTH];
int messageIndex = 0;
unsigned long lastReceivedTime = 0;

// Definición de variable actual
#define MAX_PATH_LENGTH 64
char currentDir[64] = "/";
SdFile sdCurrentDir;

// Variables de comandos/funciones
#define NUM_PARAMS 4
typedef void (*FuncCmd)(char* splitCommand[NUM_PARAMS]);

/*
 Funciones de comandos básicos del File System S.O. Linux
 y comandos propios del Automatic Keystroke (Autokey)

 Lista de comandos:
  cat - Imprime el contenido de un archivo
  pwd - Imprime directorio de trabajo
  cd - Cambia el directorio local de trabajo
  cp - Copia un archivo o directorio
  ls - Lista los nombres de archivo de un directorio
  mkdir - Crea un directorio
  mv - Renombra un archivo
  rm - Remueve un archivo o directorio
  touch - Toca un archivo
  pwd - Imprime la ruta
  key - Escribe texto proveniente de texto (-t) o archivo (-f)
*/

void pwd(char* splitCommand[NUM_PARAMS]) {
  Serial1.println(currentDir);
}

void ls(char* splitCommand[NUM_PARAMS]) {
  Serial1.println(currentDir);
  sdCurrentDir.rewind();
  SdFile entry;
  while (entry.openNext(&sdCurrentDir, O_RDONLY)) {
    if (entry.isSubDir()) {
      Serial1.print("[DIR]\t");
    } else {
      Serial1.print("[FILE]\t");
    }
    entry.printName(&Serial1);
    Serial1.println();
    entry.close();
  }
}

bool cd(char* splitCommand[NUM_PARAMS]) {
    char newPath[64];   
    strcpy(newPath, currentDir); // Copia el directorio actual a newPath
    if (strcmp(splitCommand[1], "..") == 0) {  // Si es "..", retrocede un directorio
      // Encuentra la última ocurrencia de '/' y termina la cadena allí
      char* lastSlash = strrchr(newPath, '/');
      if (lastSlash != newPath) {
        *lastSlash = '\0';
      } else {
        *(lastSlash + 1) = '\0';
      }
    } else {
      // Concatena el directorio actual y el nuevo subdirectorio
      if (newPath[strlen(newPath) - 1] != '/') {
        strcat(newPath, "/");
      }
      strcat(newPath, splitCommand[1]);
    }
    // Verifica si el nuevo directorio es válido
    SdFile dirFile;
    if (dirFile.open(newPath, O_READ)) {
      dirFile.close();    
      // Cierra sdCurrentDir antes de abrirlo con una nueva ruta (si ya estaba abierto)
      if (sdCurrentDir.isOpen()) {
        sdCurrentDir.close();
      }
      // Actualiza sdCurrentDir para apuntar al nuevo directorio
      if (!sdCurrentDir.open(newPath, O_READ)) {
        Serial1.println("Failed to update the directory.");
        return false;
      }
      strcpy(currentDir, newPath);  // Si es válido, actualiza pwd
      Serial1.println(currentDir);
      return true;  // Retorna true indicando que el cambio de directorio fue exitoso
    } else {
      return false;  // Retorna false si no se pudo cambiar el directorio
    }
}


char* getAbsolutePath(const char* filename, char* outputPath, size_t outputSize) {
  // Check if the filename starts with a /
  if (filename[0] == '/') {
    strncpy(outputPath, filename, outputSize);
  } else {
    // Construct the full path of the file by combining currentDir and filename
    snprintf(outputPath, outputSize, "%s/%s", currentDir, filename);
  }
  return outputPath;
}

void cat(char* splitCommand[NUM_PARAMS]) {
  char filename[32];
  char fullPath[128];  // To store the full path of the file
  
  strncpy(filename, splitCommand[1], sizeof(filename) - 1); // Make sure it's null-terminated.+
  getAbsolutePath(splitCommand[1], fullPath, sizeof(fullPath));

  if (!file.open(fullPath, O_READ)) {
    Serial1.println("File not found.");
    return;
  }
  int data;
  while ((data = file.read()) >= 0) {
    Serial1.write(data);
  }
  file.close();
}

void key(char* splitCommand[NUM_PARAMS]) {
  char fullPath[192];  // To store the full path of the file

  if (strcmp(splitCommand[1], "-f") == 0) {
    // Resolve the absolute path of the file
    getAbsolutePath(splitCommand[2], fullPath, sizeof(fullPath));
    if (!file.open(fullPath, O_READ)) {
      Serial1.println("File not found.");
      return;
    }
    Serial1.print("Writing...");
    while (file.available()) {
      Keyboard.print((char) file.read());
      delay(10);
    }
    file.close();
    Serial1.println("OK");
  } else if (strcmp(splitCommand[1], "-t") == 0) {
    Serial1.print("Writing...");
    String text = splitCommand[2];
    for (int i = 0; i < text.length(); i++) {
      Keyboard.print((char) text[i]);
      delay(10);
    }
    Serial1.println("OK");
  } else {
    Serial1.print("Writing...");
    String text = splitCommand[1];
    for (int i = 0; i < text.length(); i++) {
      Keyboard.print((char) text[i]);
      delay(10);
    }
    Serial1.println("OK");
  }
}

FuncCmd funCmds[] = {pwd, ls, cd, cat, key};
String strCmds[] = {"pwd", "ls", "cd", "cat", "key"};

/*
  Divide el mensaje por espaciado y verifica si el primer resultado es un comando conocido
  luego, ejecuta la función del comando o devuelve un error

  :param command: String con comando deseado a ejecutar

  :return: Ejecuta, si existe, función del comando solicitado, e.o.c., imprime error
*/
void fileSystem(char message[MAX_MESSAGE_LENGTH]) {
  const char delimiter[] = " ";
  char* splitCommand[NUM_PARAMS];
  int index = 0;
  // Inicialización de strtok para dividir el mensaje por espacios
  char* token = strtok(message, delimiter);
  while (token != NULL && index < NUM_PARAMS) {
    // Si el token comienza con una comilla doble
    if (token[0] == '"') {
      // Busca el token de cierre (otra comilla doble)
      char* endQuote = strstr(token + 1, "\"");
      if (endQuote) {
        *endQuote = '\0';  // Termino la cadena en la comilla final
        splitCommand[index] = token + 1;  // +1 para saltar la comilla inicial
        token = strtok(endQuote + 1, delimiter);  // +1 para saltar la comilla final
      } else {
        splitCommand[index] = token; // Si no hay comilla de cierre, solo guarda el token
        token = strtok(NULL, delimiter);
      }
    } else {
      splitCommand[index] = token;
      token = strtok(NULL, delimiter);
    }
    index++;
  }
  // Verifica existencia de splitCommand[0] en FuncCmd y ejecuta el comando
  for (int i = 0; i < sizeof(funCmds) / sizeof(FuncCmd); i++) {
    if (strcmp(strCmds[i].c_str(), splitCommand[0]) == 0) {
      funCmds[i](splitCommand);
      break;
    }
  }
}

/*
  Configuraciones iniciales
*/
void setup() {  
  pinMode(pinLed, OUTPUT);

  //Serial.begin(9600);
  Serial1.begin(9600); // BLE
  
  //while (!Serial);

  //Serial.print("Init SD...");
  pinMode(chipSelect, OUTPUT);
  while (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    //Serial.println("Card failed, or not present.");
    delay(3000);
  }
  if (!sdCurrentDir.open(currentDir, O_RDONLY)) { // abrir el directorio raíz
    //Serial.println("Error opening root directory.");
    return;
  }
  //Serial.println("OK!");
  digitalWrite(pinLed, 1);
}

/*
  Escucha continua de mensajes del Serial1 (BLE)
  Cada mensaje recibido se envía por parametro a la función fileSystem
*/
void loop() {
  if (Serial1.available()) {
    char incomingChar = Serial1.read();
    message[messageIndex++] = incomingChar;
    if (messageIndex >= (MAX_MESSAGE_LENGTH - 1)) {
      messageIndex = MAX_MESSAGE_LENGTH - 1;  // Evitar desbordamiento
    }
    lastReceivedTime = millis();  // Actualiza el tiempo de la última recepción.
  } 
  else {
    // Si ha pasado 1 segundo desde la última vez que se recibió un carácter y hay datos en el buffer...
    if (millis() - lastReceivedTime > 1000 && messageIndex != 0) {
      message[messageIndex] = '\0';  // Termina la cadena
      //Serial.println(message);
      fileSystem(message);
      messageIndex = 0;
    }
  }
}
