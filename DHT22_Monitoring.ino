#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN D1     // Pin data DHT kita
#define DHTTYPE DHT22 // Tipe sensor DHT yang kita gunakan

const char* ssid = "SSID";          //Masukkan nama WiFi kita
const char* password = "12345678";  //Masukkan kata sandi/password WiFi kita
const char* host = "script.google.com";  //Host yang kita gunakan
const int httpsPort = 443;               //Httpsport untuk spreadsheet tidak perlu diubah

const char* serverName = "Masukkan id penerapan Web app url "; // ID Penerapan aplikasi web url di app script

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  dht.begin();
  
  // Menghubungkan ke WiFi yang kita buat
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); //Opsional digunakan untuk menonaktifkan verifikasi sertifikat
    
    Serial.print("Connecting to ");
    Serial.println(host);
    
    if (!client.connect(host, httpsPort)) {
      Serial.println("Connection failed");
      return;
    }

//    String url = "/macros/s/1uNALsrsxgQz9gCfVeoPdhARMbyzFlCJ3HAeavqpDBKw/exec?";
    String url = "/macros/s/AKfycbw3z5d7sU9SP9wLGj7r-2n0TVpScvX_chDw-1szvo5HhPQ1Ptp0iMhUTkv3rcoQqpbcKw/exec?"; // Replace with your actual script path

    //Membaca sensor DHT22
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    //Mempersiapkan data untuk dikirim
    String data = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
    url += data;
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP8266\r\n" +
                 "Connection: close\r\n\r\n");
    
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers received");
        break;
      }
    }
    
    String line = client.readStringUntil('\n');
    if (line.startsWith("{\"result\":\"success\"")) {
      Serial.println("Data sent successfully.");
    } else {
      Serial.println("Failed to send data.");
    }
    
    client.stop();
  } else {
    Serial.println("WiFi Disconnected");
  }
  
  delay(10000); //Dapat diatur untuk waktu pengiriman data yang kalian inginkan setiap beberapa waktu
}
