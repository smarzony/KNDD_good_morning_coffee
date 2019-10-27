#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
//#include <EEPROM.h>

#define BUTTON_DELAY 300
#define READ_SENSOR_PERIOS_S 120
#define READ_SENSOR_PERIOS_S_OTA 5


#define PUMP_PIN D6
#define HEATER_PIN D7
#define THERMISTOR_PIN A0

//ESP8266WebServer server(80);
WiFiServer server(80);

const char* ssid = "smarzony";
const char* password = "metalisallwhatineed";
/*
const char* host = "192.168.1.77";
const int port = 8080;
String idx_reed_switch = "30";
String idx_reed_switch_battery = "32";
*/

String request = "";
String status = "";
bool making_coffee, making_coffee_last;

unsigned long long making_coffee_start;
const char* PARAM_INPUT_1 = "input1";
int heating_time_s, pumping_time_s, pumping_delay_s;


void setup() {
	Serial.begin(115200);
	Serial.println("Booting");
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname("KNDD_V2.1");

	// No authentication by default
	// ArduinoOTA.setPassword("admin");

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
		});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
		});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
		});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
		});
	ArduinoOTA.begin();
	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	/*
	server.on("/", handleRootPath);		 // ustawienie przerwania
	server.on("/PUMP/On", PumpOn);
	server.on("/PUMP/Off", PumpOff);
	*/
	server.begin();
	//server.handleClient();

	pinMode(PUMP_PIN, OUTPUT);
	pinMode(HEATER_PIN, OUTPUT);
	pinMode(THERMISTOR_PIN, INPUT);

	//-------------------- EEPROM -------------------
	EEPROM.begin(32);
	delay(5);
	heating_time_s = EEPROM.read(0);
	pumping_time_s = EEPROM.read(1);
	pumping_delay_s = EEPROM.read(2);


}

void loop() {
	ArduinoOTA.handle();
	//server.handleClient();
	unsigned long now = millis();

	WiFiClient client = server.available();
	if (!client)
	{
		return;
	}

	request = client.readStringUntil('\r');

	if (request.indexOf("PUMP_On") > 0) {
		digitalWrite(PUMP_PIN, HIGH);
		Serial.println("1");
		making_coffee_start = 0;
		making_coffee = 0;
	}
	else if (request.indexOf("PUMP_Off") > 0) {
		digitalWrite(PUMP_PIN, LOW);
		Serial.println("2");
		making_coffee_start = 0;
		making_coffee = 0;
	}
	else if (request.indexOf("HEATER_On") > 0) {
		digitalWrite(HEATER_PIN, HIGH);
		Serial.println("3");
		making_coffee_start = 0;
		making_coffee = 0;
	}
	else if (request.indexOf("HEATER_Off") > 0) {
		digitalWrite(HEATER_PIN, LOW);
		Serial.println("4");
		making_coffee_start = 0;
		making_coffee = 0;
	}
	else if (request.indexOf("Make_coffee") > 0) {
		making_coffee = 1;
		//Serial.println("5");
	}
	else if (request.indexOf("input1=") > 0) {
		int start_param = request.indexOf("input1=") + 7;
		int end_param = request.indexOf(" HTTP/1.");
		String param_read = request.substring(start_param, end_param);
		status = param_read;
		heating_time_s = param_read.toInt();
		EEPROM.write(0, (byte)(heating_time_s));
		EEPROM.commit();
	}
	else if (request.indexOf("input2=") > 0) {
		int start_param = request.indexOf("=") + 1;
		int end_param = request.indexOf(" HTTP/1.");
		String param_read = request.substring(start_param, end_param);
		status = param_read;
		pumping_time_s = param_read.toInt();
		EEPROM.write(1, (byte)(pumping_time_s));
		EEPROM.commit();
	}
	else if (request.indexOf("input3=") > 0) {
		int start_param = request.indexOf("=") + 1;
		int end_param = request.indexOf(" HTTP/1.");
		String param_read = request.substring(start_param, end_param);
		status = param_read;
		pumping_delay_s = param_read.toInt();
		EEPROM.write(2, (byte)(pumping_delay_s));
		EEPROM.commit();
	}


	if (making_coffee == 1 && making_coffee_last == 0)
	{
		making_coffee_start = now;
	}

	if ((now - making_coffee_start < (heating_time_s * 1000)) && (making_coffee_start != 0))
	{
		digitalWrite(HEATER_PIN, HIGH);
		status = "1. Start Heater";
		Serial.println(status);
	}


	if ((now - making_coffee_start > (pumping_delay_s * 1000)) &&
		(now - making_coffee_start < ((pumping_time_s + pumping_delay_s) * 1000)) &&
		(making_coffee_start != 0))
	{
		digitalWrite(PUMP_PIN, HIGH);
		status = "2. Start Pump";
		Serial.println(status);
	}

	if ((now - making_coffee_start > (heating_time_s * 1000)) && (making_coffee_start != 0))
	{
		digitalWrite(HEATER_PIN, LOW);
		status = "3. Stop heater";
		Serial.println(status);
	}

	if ((now - making_coffee_start > ((pumping_delay_s + pumping_time_s) * 1000)) && (making_coffee_start != 0))
	{
		digitalWrite(PUMP_PIN, LOW);
		making_coffee_start = 0;
		making_coffee = 0;
		status = "4. Stop Pump";
		Serial.println(status);
	}



	client.flush();

	client.print(getStatusPage());
	delay(5);

	making_coffee_last = making_coffee;
}


