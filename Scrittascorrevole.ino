/*

This code has released under license CC BY- SA
last release created 13/06/2018
by Giuseppe Tamanini
*/

#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

#define DATA_PIN     9
#define NUM_LEDS    256
#define BRIGHTNESS  24
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

String tabcar;

int i; // variabili dei cicli for 
int j;
int k;
int l;
String car; // carattere 0 o 1 estratto di riga
int nc; // numero intero di car
File myFile;

String stringa = "CHIUSO";
int lunstr = stringa.length(); // numero dei caratteri di stringa
boolean scorrevole[8] [169];
boolean carattere[8] [7];
String riga; // riga del file di testo .csv contenente la mappa dei caratteri
int nl = 0; // numero del led del display

byte rp = 255; // intensità rosso di primo piano
byte gp = 255; // intensità verde di primo piano
byte bp = 0; // intensità blu di primo piano
byte rf = 0; // intensità rosso di sfondo
byte gf = 0; // intensità rosso di sfondo
byte bf = 32; // intensità rosso di sfondo

int asp = 100; // tempo di ritardo scroll

String nomefile;
char buffer[7];

char inChar; // caratteri in arrivo sulla Serial1
String inputString = "";         // Stringa in arrivo sulla Serial1
boolean stringComplete = false;  // Vero se la stringa in arrivo è completa

void setup() {
  
  Serial.begin(9600);
  Serial1.begin(9600);
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  
  inputString.reserve(200);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  tabcar = " !";
  tabcar = tabcar + "\"";
  tabcar = tabcar + "#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVXYZ[/]^_abcdefghijklmnopqrstuvxyz"; // stringa caratteri ASCII
  Serial.println(tabcar);

  creainizio();
  cercacaratteri();
  creafine();
  
}

void loop() {

  if (stringComplete) {
    Serial.println(stringComplete);
    if (inputString == "msg1") {
      stringa = "APERTO";
      lunstr = stringa.length();
      cercacaratteri();
      creafine();
    } else if (inputString == "msg2") {
      stringa = "TORNO SUBITO";
      lunstr = stringa.length();
      cercacaratteri();
      creafine();
    }
    inputString = "";
    stringComplete = false;
  }
  
  for (int x = 0; x < lunstr*7+32; x++) {
    for (j = 0; j < 8; j++){  // righe del display
      for (i= 0; i < 32; i++) {  // colonne del display
      if (i%2 == 0) { // se la colonna del display è pari
        nl = i*8+j;
      } else {
        nl = i*8+7-j; // inverte i valori in colonna da 1-7 a 7-1
      }
      if (scorrevole[j][x+i] == 1) {  // se la matrice contiene il valore 1 (pixel della lettera acceso)
        leds[nl].setRGB( rp, gp, bp); // imppsta il led alla posizione nl con il colore di primo piano
      } else {
        leds[nl].setRGB( rf, gf, bf); // imposta il led alla posizione nl con il colore di sfondo
      }
    }
  }
  FastLED.show();
  delay(asp); // attende asp ms     
  }
}

void creainizio() {
  for (int i = 0; i < 32; i++) { // colonne di scorrevole
    for (j = 0; j < 8; j++) {  // righe di scorrevole
      scorrevole[j][i] = 0;
    }
  }
}

void creafine() {
  for (int i = lunstr*7+32; i < lunstr*7+64; i++) { // colonne di scorrevole
    for (j = 0; j < 8; j++) {  // righe di scorrevole
      scorrevole[j][i] = 0;
    }
  }
}

void cercacaratteri()  {
  for (i = 0; i < lunstr; i++) { // scorre i caratteri in stringa 
    for (j = 0; j < tabcar.length(); j++) { // scorre i caratteri in tabcar
      if (stringa.substring(i, i+1) == tabcar.substring(j, j+1)) { // se il carattere in stringa corrisponde a quello in tabcar
        creascritta(i, j);
      }
    }
  }
}

void creascritta(int ii, int jj) {
  leggecarattere(jj);
  for (k = 0; k < 7; k++) { // Scorre le colonne delle matrici origine (matcar) e destinazione (scorrevole)
    for (l = 0; l < 8; l++) { // Scorre le righe delle matrici
      scorrevole[l][ii*7+32+k] = carattere[l][k]; // Memorizza i valori nella matrice destinazione nello stesso ordine di riga dell'origine 
    }
  }
}

void leggecarattere(int c) {
  sprintf(buffer,"%02d.CSV",c); // genera il nome del file da aprire in base al valore di c
  nomefile = buffer;
  Serial.println(nomefile);
  myFile = SD.open(nomefile); // apre il file
  int r = 0; // inizializza la riga a 0
  if (myFile) {  // se il file esiste
    while (myFile.available()) { // legge nel file fino alla fine
      riga = myFile.readStringUntil('\n'); // legge la riga
      for (int x = 0; x < 7; x++) { // scorre i da 0 a 7 caratteri contenuti in riga
        car = riga.substring(0+x,1+x); // estrae il carattere
        nc = car.toInt(); // lo trasforma in intero
        carattere[r] [x] = nc; // memorizza il dato come booleano (0 o 1) nella matrice carattere
      }
      r= r+1; // incrementa la riga
    }
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    if (inChar == '\r') {
      char inChar = (char)Serial1.read();
      if ( inChar == '\n') {
        stringComplete = true;
        break;
      }
    }
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
  }
}
