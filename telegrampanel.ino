  /*******************************************************************
 *  this is a basic example how to program a Telegram Bot          *
 *  using TelegramBOT library on ESP8266                           *
 *                                                                 *
 *  Open a conversation with the bot, you can command via Telegram *
 *  a led from ESP8266 GPIO                                        *
 *  https://web.telegram.org/#/im?p=@FlashledBot_bot               *
 *                                                                 *
 *  written by Giancarlo Bacchio                                   *
 *  modify by Giuseppe tamanini                                   *
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>


// Initialize Wifi connection to the router
char ssid[] = "SSID";              // SSID nome della tua rete wifi
char password[] = "password";         // password della tua rete wifi


// Initialize Telegram BOT

#define BOTtoken "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  //token of FlashledBOT
#define BOTname "xxxxBOT"
#define BOTusername "xxxxBOT"

TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 1000; // millisecondi da aspettare per un nuovo messaggio
long Bot_lasttime;   // aggiorna il tempo dopo aver verificato se è arrivato un nuvo messaggio 
bool Start = false;

/********************************************
 * EchoMessages - function to Echo messages *
 ********************************************/
void Bot_ExecMessages() {
  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length()); // esclude il primo carattere "/ledon" diventerà "ledon"
    Serial.println(bot.message[i][5]); // invia il comando sulla seriale
    if (bot.message[i][5] == "msg1") { //in base al messaggio ricevuto "msg1" "msg2" "start"
      bot.sendMessage(bot.message[i][4], "Scritta APERTO", "");
    }
    if (bot.message[i][5] == "msg2") {
      bot.sendMessage(bot.message[i][4], "Scritta TORNO SUBITO", "");
    }
    if (bot.message[i][5] == "start") {
      String wellcome = "Benvenuto su ArdulogioBOT, il tuo Bot personale con ESP8266"; //manda un messaggio di benvenuto
      String wellcome1 = "/msg1 : per scrivere 'APERTO'";
      String wellcome2 = "/msg2 : per scrivere 'TORNO SUBITO'";
      bot.sendMessage(bot.message[i][4], wellcome, "");
      bot.sendMessage(bot.message[i][4], wellcome1, "");
      bot.sendMessage(bot.message[i][4], wellcome2, "");
      Start = true;
    }
  }
  bot.message[0][0] = "";  //Svuota la stringa contenente il messaggio
}

void setup() {
  
  Serial.begin(9600);
  // configura la connessione
  //Serial.print("Connessione Wifi: ");
  //Serial.println(ssid);
  IPAddress ip(192,168,1,69); //ip da assegnare all'ESP-12E
  IPAddress gateway(192,168,1,1); //gateway del router
  IPAddress subnet(255,255,255,0); //subnet mask del router
  WiFi.config(ip, gateway, subnet);
  delay(3000);

  WiFi.begin(ssid, password);

  //Serial.println("");
  //Serial.println("WiFi connessa");
  //Serial.println("Indirizzo IP: ");
  //Serial.println(WiFi.localIP());
  bot.begin();      // avvia le funzionalità di Bot
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  { //Se è passato il tempo Bot_mtbs in millisecondi
    bot.getUpdates(bot.message[0][1]);   // Controlla se c'è un nuovo messaggio
    Bot_ExecMessages();   //esegue la void Bot_ExecMessages()
    Bot_lasttime = millis(); //fa ripartire il tempo di attesa
    // if mode greater than numMode reset
  }
}
