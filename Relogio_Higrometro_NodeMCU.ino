#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ==== CONFIGURAÇÕES ====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHTPIN D5
#define DHTTYPE DHT11
#define TEXT_SIZE 2   // Fonte 2x maior

// ==== SUA REDE WI-FI ====
const char* ssid     = "Rede";
const char* password = "Senha";

// ==== LISTA DE IPs POSSÍVEIS (máximo 5) ====
const char* ipList[] = {
  "http://192.168.0.200",
  "http://192.168.0.201",
  "http://192.168.0.202",
  "http://192.168.0.203",
  "http://192.168.0.204"
};
const int ipCount = 5;

// ==== OBJETOS ====
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000); // UTC-3 (Brasil)

// ==== VARIÁVEIS ====
String horaAtual = "--:--:--";

// Leituras internas
float tempInterna = NAN, umidInterna = NAN;
float ultimaTempInterna = NAN, ultimaUmidInterna = NAN;
bool tempInternaDefasada = false;
bool umidInternaDefasada = false;

// Leituras externas
float tempExterna = NAN, umidExterna = NAN;
float ultimaTempExterna = NAN, ultimaUmidExterna = NAN;
bool tempExternaDefasada = false;
bool umidExternaDefasada = false;

bool mostraTemp = true;
unsigned long ultimoToggle = 0;

// ==== FUNÇÃO PARA BUSCAR JSON EM VÁRIOS IPs ====
bool buscaDadosExternos() {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClient client;
  HTTPClient http;

  for (int i = 0; i < ipCount; i++) {
    http.begin(client, ipList[i]);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();

      int tIndex = payload.indexOf("\"temperatura\":");
      int uIndex = payload.indexOf("\"umidade\":");
      if (tIndex >= 0 && uIndex >= 0) {
        float novaTemp = payload.substring(tIndex + 14, payload.indexOf(',', tIndex)).toFloat();
        float novaUmid = payload.substring(uIndex + 10, payload.indexOf('}', uIndex)).toFloat();

        // Testes separados
        if (!isnan(novaTemp)) {
          tempExterna = novaTemp;
          tempExternaDefasada = false;
        } else {
          tempExterna = ultimaTempExterna;
          tempExternaDefasada = true;
        }

        if (!isnan(novaUmid)) {
          umidExterna = novaUmid;
          umidExternaDefasada = false;
        } else {
          umidExterna = ultimaUmidExterna;
          umidExternaDefasada = true;
        }

        http.end();
        return true;
      }
    }
    http.end();
  }

  // Se nenhum IP respondeu, usa valores anteriores e marca como defasado
  tempExterna = ultimaTempExterna;
  umidExterna = ultimaUmidExterna;
  tempExternaDefasada = true;
  umidExternaDefasada = true;
  return false;
}

void garanteWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    unsigned long inicio = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - inicio < 5000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi reconectado!" : "\nFalha ao reconectar.");
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(D2, D1);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao inicializar display!");
    while (true);
  }
  display.clearDisplay();
  display.display();

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
}

void loop() {
  garanteWiFi();

  timeClient.update();
  horaAtual = timeClient.getFormattedTime();

  // --- Lê sensor interno com testes separados ---
  float t = dht.readTemperature();
  if (!isnan(t)) {
    tempInterna = t;
    tempInternaDefasada = false;
  } else {
    tempInterna = ultimaTempInterna;
    tempInternaDefasada = true;
  }

  float h = dht.readHumidity();
  if (!isnan(h)) {
    umidInterna = h;
    umidInternaDefasada = false;
  } else {
    umidInterna = ultimaUmidInterna;
    umidInternaDefasada = true;
  }

  // Guarda valores anteriores
  ultimaTempInterna = tempInterna;
  ultimaUmidInterna = umidInterna;
  ultimaTempExterna = tempExterna;
  ultimaUmidExterna = umidExterna;

  buscaDadosExternos();

  if (millis() - ultimoToggle > 5000) {
    mostraTemp = !mostraTemp;
    ultimoToggle = millis();
  }

  // --- Atualiza display ---
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(horaAtual);

  display.setTextSize(2);
  if (mostraTemp) {
    display.setCursor(0, 30);
    display.print("In:");
    display.print(tempInterna, 1);
    display.print("C");
    if (tempInternaDefasada) display.print("#");

    display.setCursor(0, 50);
    display.print("Out:");
    display.print(tempExterna, 1);
    display.print("C");
    if (tempExternaDefasada) display.print("#");

  } else {
    display.setCursor(0, 30);
    display.print("In:");
    display.print(umidInterna, 0);
    display.print("%");
    if (umidInternaDefasada) display.print("#");

    display.setCursor(0, 50);
    display.print("Out:");
    display.print(umidExterna, 0);
    display.print("%");
    if (umidExternaDefasada) display.print("#");
  }

  display.display();
  delay(500);
}
