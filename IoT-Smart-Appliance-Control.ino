#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =========================================================
// WIFI SETTINGS
// =========================================================

const char* ssid     = "Risha";
const char* password = "risha123";

// =========================================================
// PIN DEFINITIONS
// =========================================================

// DS18B20
#define DS18B20_PIN D5

// Relay outputs
#define FAN_RELAY   D6
#define LIGHT_RELAY D7

// LDR
#define LDR_PIN A0

// =========================================================
// RELAY LOGIC
// =========================================================

// Most relay modules are ACTIVE LOW.
// If your relay works opposite, change these values.

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// =========================================================
// LDR SETTINGS
// =========================================================

// Adjust this value according to your LDR module.
//
// Example:
// ADC value LOW  = dark
// ADC value HIGH = bright
//
// If your LDR gives opposite readings, change
// LDR_DARK_THRESHOLD accordingly.

#define LDR_DARK_THRESHOLD 400

// =========================================================
// TEMPERATURE THRESHOLD
// =========================================================

#define FAN_TEMPERATURE 40.0

// =========================================================
// OBJECTS
// =========================================================

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);

ESP8266WebServer server(80);

// =========================================================
// VARIABLES
// =========================================================

float temperature = 0.0;
int lightValue = 0;

bool fanState = false;
bool lightState = false;

// AUTO mode = true
// MANUAL mode = false

bool autoMode = true;

// Manual control variables

bool manualFanState = false;
bool manualLightState = false;

// Timing

unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 2000;

// =========================================================
// FUNCTION DECLARATIONS
// =========================================================

void readSensors();
void automaticControl();
void updateLCD();

void handleRoot();
void handleData();
void handleAuto();

void handleFanOn();
void handleFanOff();

void handleLightOn();
void handleLightOff();

void handleNotFound();

// =========================================================
// SETUP
// =========================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" SMART APPLIANCE CONTROL");
  Serial.println(" ESP8266 IoT PROJECT 39");
  Serial.println("================================");

  // =======================================================
  // RELAY PIN SETUP
  // =======================================================

  pinMode(FAN_RELAY, OUTPUT);
  pinMode(LIGHT_RELAY, OUTPUT);

  // Initially OFF

  digitalWrite(FAN_RELAY, RELAY_OFF);
  digitalWrite(LIGHT_RELAY, RELAY_OFF);

  // =======================================================
  // LDR
  // =======================================================

  pinMode(LDR_PIN, INPUT);

  // =======================================================
  // DS18B20
  // =======================================================

  sensors.begin();

  // =======================================================
  // I2C LCD
  // =======================================================

  Wire.begin(D2, D1);

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Smart Appliance");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(2000);

  // =======================================================
  // WIFI
  // =======================================================

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");

    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
  }

  Serial.println();

  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // =======================================================
  // LCD IP ADDRESS
  // =======================================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(3000);

  // =======================================================
  // WEB SERVER ROUTES
  // =======================================================

  server.on("/", handleRoot);

  server.on("/data", handleData);

  server.on("/auto", handleAuto);

  server.on("/fan/on", handleFanOn);
  server.on("/fan/off", handleFanOff);

  server.on("/light/on", handleLightOn);
  server.on("/light/off", handleLightOff);

  server.onNotFound(handleNotFound);

  // =======================================================
  // START SERVER
  // =======================================================

  server.begin();

  Serial.println("Web Server Started");

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("IP:");

  lcd.setCursor(3, 0);
  lcd.print(WiFi.localIP());

  delay(2000);
}

// =========================================================
// MAIN LOOP
// =========================================================

void loop()
{
  // Handle web browser requests

  server.handleClient();

  // Read sensors every 2 seconds

  if (millis() - lastSensorRead >= sensorInterval)
  {
    lastSensorRead = millis();

    readSensors();

    // Automatic control

    if (autoMode == true)
    {
      automaticControl();
    }

    updateLCD();

    // =====================================================
    // SERIAL MONITOR
    // =====================================================

    Serial.println("--------------------------------");

    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("LDR Value   : ");
    Serial.println(lightValue);

    Serial.print("Fan         : ");

    if (fanState)
    {
      Serial.println("ON");
    }
    else
    {
      Serial.println("OFF");
    }

    Serial.print("Light       : ");

    if (lightState)
    {
      Serial.println("ON");
    }
    else
    {
      Serial.println("OFF");
    }

    Serial.print("Mode        : ");

    if (autoMode)
    {
      Serial.println("AUTO");
    }
    else
    {
      Serial.println("MANUAL");
    }
  }
}

// =========================================================
// READ SENSORS
// =========================================================

void readSensors()
{
  // =======================================================
  // DS18B20
  // =======================================================

  sensors.requestTemperatures();

  float temp = sensors.getTempCByIndex(0);

  // Check sensor validity

  if (temp != DEVICE_DISCONNECTED_C)
  {
    temperature = temp;
  }
  else
  {
    Serial.println("DS18B20 ERROR!");

    temperature = 0;
  }

  // =======================================================
  // LDR
  // =======================================================

  lightValue = analogRead(LDR_PIN);
}

// =========================================================
// AUTOMATIC CONTROL
// =========================================================

void automaticControl()
{
  // =======================================================
  // FAN CONTROL
  // =======================================================

  if (temperature > FAN_TEMPERATURE)
  {
    fanState = true;

    digitalWrite(FAN_RELAY, RELAY_ON);
  }
  else
  {
    fanState = false;

    digitalWrite(FAN_RELAY, RELAY_OFF);
  }

  // =======================================================
  // LIGHT CONTROL
  // =======================================================

  if (lightValue < LDR_DARK_THRESHOLD)
  {
    lightState = true;

    digitalWrite(LIGHT_RELAY, RELAY_ON);
  }
  else
  {
    lightState = false;

    digitalWrite(LIGHT_RELAY, RELAY_OFF);
  }
}

// =========================================================
// UPDATE LCD
// =========================================================

void updateLCD()
{
  lcd.clear();

  // =======================================================
  // LINE 1
  // =======================================================

  lcd.setCursor(0, 0);

  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.print("L:");
  lcd.print(lightValue);

  // =======================================================
  // LINE 2
  // =======================================================

  lcd.setCursor(0, 1);

  lcd.print("F:");

  if (fanState)
  {
    lcd.print("ON ");
  }
  else
  {
    lcd.print("OFF");
  }

  lcd.print(" Li:");

  if (lightState)
  {
    lcd.print("ON");
  }
  else
  {
    lcd.print("OFF");
  }
}

// =========================================================
// WEB DASHBOARD
// =========================================================

void handleRoot()
{
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";

  html += "<head>";

  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";

  html += "<title>Smart Appliance Control</title>";

  // =======================================================
  // CSS
  // =======================================================

  html += "<style>";

  html += "*{box-sizing:border-box;}";

  html += "body{";
  html += "font-family:Arial;";
  html += "margin:0;";
  html += "background:#f1f5f9;";
  html += "color:#111827;";
  html += "}";

  html += ".header{";
  html += "background:#2563eb;";
  html += "color:white;";
  html += "padding:20px;";
  html += "text-align:center;";
  html += "}";

  html += ".container{";
  html += "max-width:900px;";
  html += "margin:auto;";
  html += "padding:20px;";
  html += "}";

  html += ".grid{";
  html += "display:grid;";
  html += "grid-template-columns:repeat(auto-fit,minmax(220px,1fr));";
  html += "gap:15px;";
  html += "}";

  html += ".card{";
  html += "background:white;";
  html += "padding:20px;";
  html += "border-radius:15px;";
  html += "box-shadow:0 3px 10px rgba(0,0,0,0.12);";
  html += "text-align:center;";
  html += "}";

  html += ".value{";
  html += "font-size:32px;";
  html += "font-weight:bold;";
  html += "margin:10px;";
  html += "}";

  html += ".status{";
  html += "font-size:20px;";
  html += "font-weight:bold;";
  html += "}";

  html += "button{";
  html += "border:0;";
  html += "border-radius:8px;";
  html += "padding:12px 18px;";
  html += "margin:5px;";
  html += "font-size:16px;";
  html += "cursor:pointer;";
  html += "}";

  html += ".on{background:#16a34a;color:white;}";

  html += ".off{background:#dc2626;color:white;}";

  html += ".auto{background:#2563eb;color:white;}";

  html += ".manual{background:#9333ea;color:white;}";

  html += ".info{";
  html += "background:white;";
  html += "padding:15px;";
  html += "border-radius:10px;";
  html += "margin-top:15px;";
  html += "}";

  html += "</style>";

  // =======================================================
  // JAVASCRIPT
  // =======================================================

  html += "<script>";

  html += "function updateData(){";

  html += "fetch('/data')";

  html += ".then(response=>response.json())";

  html += ".then(data=>{";

  html += "document.getElementById('temp').innerHTML=data.temperature.toFixed(1)+' &deg;C';";

  html += "document.getElementById('ldr').innerHTML=data.light;";

  html += "document.getElementById('fan').innerHTML=data.fan?'ON':'OFF';";

  html += "document.getElementById('light').innerHTML=data.lightState?'ON':'OFF';";

  html += "document.getElementById('mode').innerHTML=data.auto?'AUTO':'MANUAL';";

  html += "});";

  html += "}";

  html += "setInterval(updateData,2000);";

  html += "window.onload=updateData;";

  html += "</script>";

  html += "</head>";

  // =======================================================
  // BODY
  // =======================================================

  html += "<body>";

  html += "<div class='header'>";

  html += "<h1>Smart Appliance Control</h1>";

  html += "<p>ESP8266 IoT Dashboard</p>";

  html += "</div>";

  html += "<div class='container'>";

  // =======================================================
  // SENSOR CARDS
  // =======================================================

  html += "<div class='grid'>";

  html += "<div class='card'>";

  html += "<h2>Temperature</h2>";

  html += "<div class='value' id='temp'>--</div>";

  html += "<p>DS18B20 Sensor</p>";

  html += "</div>";

  html += "<div class='card'>";

  html += "<h2>Light Intensity</h2>";

  html += "<div class='value' id='ldr'>--</div>";

  html += "<p>LDR ADC Value</p>";

  html += "</div>";

  html += "<div class='card'>";

  html += "<h2>Fan</h2>";

  html += "<div class='status' id='fan'>--</div>";

  html += "<br>";

  html += "<a href='/fan/on'><button class='on'>FAN ON</button></a>";

  html += "<a href='/fan/off'><button class='off'>FAN OFF</button></a>";

  html += "</div>";

  html += "<div class='card'>";

  html += "<h2>Light</h2>";

  html += "<div class='status' id='light'>--</div>";

  html += "<br>";

  html += "<a href='/light/on'><button class='on'>LIGHT ON</button></a>";

  html += "<a href='/light/off'><button class='off'>LIGHT OFF</button></a>";

  html += "</div>";

  html += "</div>";

  // =======================================================
  // MODE CONTROL
  // =======================================================

  html += "<div class='info'>";

  html += "<h2>Control Mode</h2>";

  html += "<p>Current Mode: <b id='mode'>--</b></p>";

  html += "<a href='/auto'>";

  html += "<button class='auto'>AUTOMATIC MODE</button>";

  html += "</a>";

  html += "<p>";

  html += "Automatic mode controls the fan according to temperature ";

  html += "and the light according to LDR intensity.";

  html += "</p>";

  html += "</div>";

  // =======================================================
  // AUTOMATIC CONDITIONS
  // =======================================================

  html += "<div class='info'>";

  html += "<h2>Automatic Conditions</h2>";

  html += "<p>Fan ON when temperature &gt; 40 &deg;C</p>";

  html += "<p>Fan OFF when temperature &le; 40 &deg;C</p>";

  html += "<p>Light ON when light intensity is low</p>";

  html += "<p>Light OFF when sufficient light is available</p>";

  html += "</div>";

  html += "</div>";

  html += "</body>";

  html += "</html>";

  server.send(200, "text/html", html);
}

// =========================================================
// SEND SENSOR DATA
// =========================================================

void handleData()
{
  String json = "{";

  json += "\"temperature\":";
  json += String(temperature, 1);

  json += ",";

  json += "\"light\":";
  json += String(lightValue);

  json += ",";

  json += "\"fan\":";
  json += fanState ? "true" : "false";

  json += ",";

  json += "\"lightState\":";
  json += lightState ? "true" : "false";

  json += ",";

  json += "\"auto\":";
  json += autoMode ? "true" : "false";

  json += "}";

  server.send(200, "application/json", json);
}

// =========================================================
// AUTOMATIC MODE
// =========================================================

void handleAuto()
{
  autoMode = true;

  automaticControl();

  updateLCD();

  server.sendHeader("Location", "/");

  server.send(303);
}

// =========================================================
// FAN ON
// =========================================================

void handleFanOn()
{
  autoMode = false;

  manualFanState = true;

  fanState = true;

  digitalWrite(FAN_RELAY, RELAY_ON);

  updateLCD();

  server.sendHeader("Location", "/");

  server.send(303);
}

// =========================================================
// FAN OFF
// =========================================================

void handleFanOff()
{
  autoMode = false;

  manualFanState = false;

  fanState = false;

  digitalWrite(FAN_RELAY, RELAY_OFF);

  updateLCD();

  server.sendHeader("Location", "/");

  server.send(303);
}

// =========================================================
// LIGHT ON
// =========================================================

void handleLightOn()
{
  autoMode = false;

  manualLightState = true;

  lightState = true;

  digitalWrite(LIGHT_RELAY, RELAY_ON);

  updateLCD();

  server.sendHeader("Location", "/");

  server.send(303);
}

// =========================================================
// LIGHT OFF
// =========================================================

void handleLightOff()
{
  autoMode = false;

  manualLightState = false;

  lightState = false;

  digitalWrite(LIGHT_RELAY, RELAY_OFF);

  updateLCD();

  server.sendHeader("Location", "/");

  server.send(303);
}

// =========================================================
// PAGE NOT FOUND
// =========================================================

void handleNotFound()
{
  server.send(404, "text/plain", "Page Not Found");
}
