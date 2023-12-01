#include <NewPing.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h> 

const char* ssid = "Regina Juniantika";
const char* password = "ginajaya";

// inisialisasi Bot Token
#define BOTtoken "6727051341:AAGDH9IxHRlVvKnz45qm1n_XXWQG9yx4oDU"  // Bot Token dari BotFather 

// chat id dari @myidbot 
#define CHAT_ID "5219799334" 

String chat_id;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//koding ultrasonic......................................>  
//pin Sensor ultrasonik
//S1 pendeteksi benda
#define TRIGGER_S1      22 //D22
#define ECHO_S1         23 //D23
//S2 pendeteksi Kapasitas
#define TRIGGER_S2      21 // D21
#define ECHO_S2         19 //D19

NewPing sonar1(TRIGGER_S1, ECHO_S1); 
NewPing sonar2(TRIGGER_S2, ECHO_S2); 

// Data pembacaan sensor
int DATA_S1 =0;
int DATA_S2 =0;
// data sensor di Konversi ke data String untuk dikirimkan ke pesan bot tele 
String pesan;
//data konversi S2 ke %
int level = 0;
// nama servo
Servo myservo;
  // koding untuk millis
  unsigned long currentMillis = millis(); // Mendapatkan waktu millis saat ini
  static unsigned long previousMillis = 0; // Waktu millis sebelumnya
  static const unsigned long interval = 500;

// koding untuk melihat pesan.
bool full = false;
void handleNewMessages(int numNewMessages) {
  Serial.println("menangani Pesan Baru");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
     chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "user tidak salah", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String control = "Selamat Datang, " + from_name + ".\n";
      control += "Implementation of the IoT system in smart trash cans and via telegram.\n\n";
      control += "Gunakan Commands Di Bawah Untuk Kontrol.\n";
      control += "/Cek_Isi Untuk Mengecek Isi Tempat sampah \n";
      control += "/Buka    untuk membuka tempat sampah \n";
      control += "/Tutup    untuk menutup tempat sampah \n";
     // control += "/Status Untuk Cek Status Led Saat Ini \n";
      bot.sendMessage(chat_id, control, "");
    }

    if (text == "/Cek_Isi") {
        level = map(DATA_S2, 0, 31, 100, 0);
        pesan = "kapasitas sampah = " + String(level) + "%";

  //    bot.sendMessage( chat_id, "sampah = ", "");
      bot.sendMessage( chat_id, pesan, "");
      Serial.println(pesan);
    }
    
    if (text == "/Buka") {
      myservo.write(90);
      bot.sendMessage(chat_id, "Tempat sampah terbuka", "");
      
    }

   if (text == "/Tutup") {
      myservo.write(0);
      bot.sendMessage(chat_id, "Tempat sampah tertutup", "");
      
     
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Pin servo
  myservo.attach(4);// D4
  // Jalankan wifi
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void ULTRA(){
  DATA_S1 = sonar1.ping_cm();// pendeteksi barang
  DATA_S2 = sonar2.ping_cm();// pendeteksi kapasitas
  Serial.println(String() + "sensor_2 = " + DATA_S2 + " Cm ");

 if (full == false) {
  if (DATA_S1 <= 10) { // jika ada benda terdeteksi maka 
      myservo.write(90); // buka tutup sampah
      bot.sendMessage(chat_id, "Tempat sampah terbuka", ""); // kirim pesan ke bot
      Serial.println("terbuka");
  }
 }
  if (level >= 70) { // jika kapasitas lebih dari 70% maka
      myservo.write(0); // kunci tempat sampah
      bot.sendMessage(chat_id, "Tempat sampah full", "");// kirim pesan
      full = true;
  }

  else { /// jika tidak dari itu maka 
     delay(3000); // waktu jeda
     myservo.write(0); // tutup tempat sampah
     Serial.println("tertutup");
     full = false;
  }

  
}
void loop() {

   ULTRA();/// jalankan  program pembacaan sensor ultrasonik

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
     Serial.print(String() + "S1_barang = " + DATA_S1 + "CM");
     Serial.println(String() + "S2_isi = " + DATA_S2 + "CM");
    lastTimeBotRan = millis();
  }

 if (currentMillis - previousMillis >= interval) {
 
 
    previousMillis = currentMillis; 

    Serial.println(currentMillis); 
  }
}