#include <Node.h>
#include <WiFi.h>
#include <HTTPClient.h>
HTTPClient http;
/*
  const char* wifi_ssid = "LiWiFi 2.4Ghz";
  const char* wifi_pass = "Lindsey7047";
*/
const char* wifi_ssid = "NAP--v3jXfzPYCJ7hPtW1e2hL5AlH/M";
const char* wifi_pass = "";


#include <DHT.h> // DHT for humidity/temperature sensor
DHT DHT_INTERFACE(22, DHT22); // Init sensor on PIN 22 with sensor type set to DHT22

#include <NewPing.h> // NewPing for HC-SR04 ultrasonic distance sensor
// Echo pin is reused, trigger is on PIN 16
int HCSR04_PIN_ECHO    = 17;
// NewPing sonar(trigger,echo,max_cm)
NewPing sonar(16, HCSR04_PIN_ECHO, 250);

#include <ArduinoJson.h>
StaticJsonBuffer<512> JSON_Buffer;
String JSON_Temporary_Output;
JsonObject& JSON_Root         =   JSON_Buffer.createObject();
JsonObject& air_data_json     =   JSON_Root.createNestedObject("air");
JsonObject& depth_data_json   =   JSON_Root.createNestedObject("depth");
JsonObject& light_data_json   =   JSON_Root.createNestedObject("light");

int i = 0;
int LED_PIN = 36;
int PHR_PIN = A14;
int HCSR04_OUTPUT = 0;

int API_POLLING_INTERVAL = 3000;

void setup() {
  Serial.begin(115200);
  DHT_INTERFACE.begin();
  wifi_setup();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  Serial.print("#");
  Serial.print(++i);
  read_depth();
  read_air();
  read_light();
  wifi_poll();
  delay(API_POLLING_INTERVAL);
}

void wifi_setup() {

  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
  }

  WiFi.begin(wifi_ssid);
  while (WiFi.status() != WL_CONNECTED) { // Check for the connection
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}
void wifi_poll() {
  // compile json
  compile_json();

  http.begin("http://192.168.4.1");

  Serial.println("Sending JSON to server: ");
  Serial.println(JSON_Temporary_Output);
  int http_status = http.POST(JSON_Temporary_Output);
  String http_output = ( http_status ) ? http.getString() : "HTTP failed";

  Serial.print("Global node says: ");
  Serial.println(http_output);
  //Serial.print(" Set LED to ");
  //Serial.println(http_output.toInt());
  //digitalWrite(LED_PIN, http_output.toInt());

  http.end();
}


void read_depth() {
  HCSR04_OUTPUT = sonar.ping(); // divide by US_ROUNDTRIP_CM or US_ROUNDTRIP_IN
  depth_data_json["cm"] = HCSR04_OUTPUT / US_ROUNDTRIP_CM;
  depth_data_json["in"] = HCSR04_OUTPUT / US_ROUNDTRIP_IN;
  // Echo again for the next reading
  pinMode(HCSR04_PIN_ECHO , OUTPUT);
  digitalWrite(HCSR04_PIN_ECHO , LOW);
  pinMode(HCSR04_PIN_ECHO , INPUT);
  Serial.print("\nDistance: ");
  Serial.print(HCSR04_OUTPUT / US_ROUNDTRIP_CM);
  Serial.println("cm");
}


void read_light() {

  light_data_json["phr"] = analogRead(PHR_PIN);
  Serial.print("PHR = "); Serial.println(light_data_json.get<int>("phr"));
  //light_data_json["led"] = (light_data_json["phr"] >= 2500) ? HIGH : LOW;
  //digitalWrite(LED_PIN, light_data_json["led"]);
  //Serial.print("LED = "); Serial.println(digitalRead(LED_PIN));

}
void read_air() {

  air_data_json["humidity"]   = DHT_INTERFACE.readHumidity();
  air_data_json["centigrade"] = DHT_INTERFACE.readTemperature();
  air_data_json["farenheit"]  = air_data_json.get<float>("centigrade") * 9 / 5 + 32;

  //Print temp and humidity values to serial monitor
  Serial.print(air_data_json.get<float>("farenheit")); Serial.print("F / "); Serial.print(air_data_json.get<float>("centigrade")); Serial.print("C @ ");
  Serial.print(air_data_json.get<float>("humidity"));
  Serial.println("% humidity");

}


void compile_json() {
  JSON_Temporary_Output = "";
  JSON_Root.printTo(JSON_Temporary_Output);
}


