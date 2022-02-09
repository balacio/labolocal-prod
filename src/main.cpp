#include <Arduino.h>

const int ledMeasure = 16;
const int ledOK = 17;

void readSensors();
void sendReadings();

// Setup switch
const int pushButton = 4;
const int ledSetup = 9;
int buttonState = 0;
bool switchSetup = false;
const long intervalSetup = 10000;

// SPIFFS
#include "SPIFFS.h"
String ssid;
String ssidKey;
String timezone;
String ulogin;
String upass;
String apiKey;
String rtdbURL;
const char* ssidPath		= "/ssid.txt";
const char* ssidKeyPath		= "/ssidkey.txt";
const char* timezonePath	= "/timezone.txt";
const char* uloginPath		= "/ulogin.txt";
const char* upassPath		= "/upass.txt";
const char* apiKeyPath		= "/apikey.txt";
const char* rtdbURLPath		= "/rtdburl.txt";
String readFile(fs::FS &fs, const char * path);
void initSPIFFS();
void readSPIFFS();
void writeFile(fs::FS &fs, const char * path, const char * message, int isWifi);

// WiFi
#include <WiFi.h>
unsigned long previousMillis = 0;
const long intervalWiFi = 30000;
bool initWiFi();

//Access point
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
AsyncWebServer server(80);
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "ssidKey";
const char* PARAM_INPUT_3 = "timezone";
const char* PARAM_INPUT_4 = "ulogin";
const char* PARAM_INPUT_5 = "upass";
const char* PARAM_INPUT_6 = "apiKey";
const char* PARAM_INPUT_7 = "rtdbURL";
void serveWifiPortal();

// OTA
#include <AsyncElegantOTA.h>
void serveOTA();

// DNS http://labo.local
#include <ESPmDNS.h>
void laboDNS();

// // NTP
// #include "time.h"
// int timestamp;
// const char* ntpServer = "pool.ntp.org";
// unsigned long getTime();
// int year;
// int month;
// int day;


// Firebase
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
String uid;
String readerPath;
String databasePath;
String parentPath;
String timePath		= "/timestamp";
String lastreadPath	= "/lastread";
String tempPath		= "/temperature";
String humPath		= "/humidity";
String presPath		= "/pressure";
String waterPath	= "/water";
String phPath		= "/ph";
String ecPath		= "/ec";
String levelPath	= "/level";
void initFirebase();
void sendFloat(String path, float value);
void sendInt(String path, float value);
// void sendString(String path, String value);
void sendTimestamp();
void sendBME();
void sendDS18B20();
void sendEC();
void sendPH();
void sendLevel();

// BME280 sensor
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>		// BME280 and EZO pH i2c connections
#define bme_address 0x76
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
float temperature;
float humidity;
float pressure;
void initBME();
void readBME();

// DS18B20 sensor
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 13;
OneWire oneWire(oneWireBus);
DallasTemperature ds18b20(&oneWire);
float water;
void readDS18B20();

// pH sensor
#define ph_address 99
byte code = 0;	//used to hold the I2C response code.
char ph_data[20]; //we make a 20-byte character array to hold incoming data from the pH circuit.
byte in_char = 0; //used as a 1 byte buffer to store inbound bytes from the pH Circuit.
byte i = 0;	   //counter used for ph_data array.
int time_ = 900;  //used to change the delay needed depending on the command sent to the EZO Class pH Circuit.
float ph;   //float var used to hold the float value of the pH.
void readPH();

// EC sensor
float ec;
void readEC();

// Level sensor
float level;
const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034
long duration;
float distanceCm;
void initLevel();
void readLevel();

// Timer
unsigned long sendDataPrevMillis	= 0;
unsigned long intervalMeasure		= 180000;
int count = 1;
int sec;


void setup() {
	Serial.begin(115200);

	Serial.println("Device booting.");

	pinMode(ledMeasure, OUTPUT);
	pinMode(ledOK, OUTPUT);
	pinMode(ledSetup, OUTPUT);
	pinMode(pushButton, INPUT);
	
	buttonState = digitalRead(pushButton);

	delay(intervalSetup);

	// buttonState = digitalRead(pushButton);

	if (buttonState == HIGH) {
		switchSetup = true;
		Serial.printf("Setup switch activated.\n");
	}

	initSPIFFS();
	readSPIFFS();
	initBME();
	initLevel();
	
	if(switchSetup != true && initWiFi()) {
		digitalWrite(ledOK, HIGH);
			
		// configTime(0, 0, ntpServer);
		// setenv("TZ", timezone.c_str(), 1);

		initFirebase();
	} else {
		count = 0;
		digitalWrite(ledSetup, HIGH);
		serveWifiPortal();
		serveOTA();
		laboDNS();
		Serial.println("Device mode: setup");
	}
}

void loop(){
	if (Firebase.ready() && (millis() - sendDataPrevMillis > intervalMeasure 
	|| sendDataPrevMillis == 0)) {
		sendDataPrevMillis = millis();
		digitalWrite(ledMeasure, HIGH);
		digitalWrite(ledOK, LOW);

		// timestamp = getTime();
		// Serial.printf("\nTimestamp loop = %i\n", timestamp);
		// parentPath= databasePath + "/" + String(timestamp);
		// parentPath = databasePath + "/" + String(year) + "/" + String(month) 
		// 			+ "/" + String(day) + "/" + String(timestamp);
		// readerPath = "/UsersData/" + uid + "/reader";

		readSensors();

		if (count > 2) {
			sendReadings();
		}
		Serial.printf("count = %i\n", count);
		count++;
		
		sec = intervalMeasure/1000;
		digitalWrite(ledMeasure, LOW);
		digitalWrite(ledOK, HIGH);
	}

	if(count != 0) {
		Serial.printf("%i... ", sec--);
		delay(999);
	}
}

// unsigned long getTime() {
// 	time_t now;
// 	struct tm timeinfo;
// 	if (!getLocalTime(&timeinfo)) {
// 		Serial.println("Failed to obtain time");
// 		return(0);
// 	}
// 	time(&now);
// 	return now;
// }

void initFirebase() {
	config.api_key		= apiKey.c_str();
	auth.user.email		= ulogin.c_str();
	auth.user.password	= upass.c_str();
	config.database_url	= rtdbURL.c_str();

	Firebase.reconnectWiFi(true);
	fbdo.setResponseSize(4096);

	config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
	config.max_token_generation_retry = 5;
	Firebase.begin(&config, &auth);
	Serial.println("Getting User UID");
	while ((auth.token.uid) == "") {
		Serial.print('.');
		delay(1000);
	}
	uid = auth.token.uid.c_str();
	Serial.print("User UID: ");
	Serial.println(uid);
	databasePath	= "/UsersData/" + uid + "/data";
	readerPath		= "/UsersData/" + uid + "/reader";
}

bool initWiFi() {
	if(ssid=="") {
		Serial.println("Undefined SSID");
		return false;
	}
	WiFi.begin(ssid.c_str(), ssidKey.c_str());
	Serial.print("Connecting to WiFi ..");
	unsigned long currentMillis = millis();
	previousMillis = currentMillis;
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(1000);
		if (currentMillis - previousMillis >= intervalWiFi) {
			Serial.println("Failed to connect.");
			return false;
		}
	}
	Serial.println(WiFi.localIP());
	Serial.println();
	return true;
}

void initBME() {
	if (!bme.begin(bme_address)) {
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		while (1); // Makes an infinite loop
	}
}

void initSPIFFS() {
	if (!SPIFFS.begin(true)) {
		Serial.println("An error has occurred while mounting SPIFFS");
	}
	Serial.println("SPIFFS mounted successfully");
}

String readFile(fs::FS &fs, const char * path) {
	Serial.printf("Reading file: %s\r\n", path);

	File file = fs.open(path);
	if(!file || file.isDirectory()) {
		Serial.println("Failed to open file for reading");
		return String();
	}
	
	String fileContent;
	while(file.available()) {
		fileContent = file.readStringUntil('\n');
		break;		 
	}
	return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message, int isWifi) {
	Serial.printf("Writing file: %s\r\n", path);

	File file;
	
	if (isWifi == 1) {
		file = fs.open(path, FILE_APPEND);
	} else {
		file = fs.open(path, FILE_WRITE);
	}
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
	if(file.print(message)) {
		Serial.println("File written");
	} else {
		Serial.println("Failed to write");
	}
}

void readSPIFFS() {
	ssid = readFile(SPIFFS, ssidPath);
	ssidKey = readFile(SPIFFS, ssidKeyPath);
	timezone = readFile(SPIFFS, timezonePath);
	ulogin = readFile(SPIFFS, uloginPath);
	upass = readFile(SPIFFS,upassPath);
	apiKey = readFile(SPIFFS, apiKeyPath);
	rtdbURL = readFile(SPIFFS, rtdbURLPath);
	Serial.print("SSID: ");
	Serial.println(ssid);
	Serial.print("Key: ");
	Serial.println(ssidKey);
	Serial.print("Timezone: ");
	Serial.println(timezone);
	Serial.print("User email: ");
	Serial.println(ulogin);
	Serial.print("User password: ");
	Serial.println(upass);
	Serial.print("API key: ");
	Serial.println(apiKey);
	Serial.print("RTDB URL: ");
	Serial.println(rtdbURL);
}

void serveWifiPortal() {
	WiFi.softAP("labolocal", NULL);
	// WiFi.softAP("labolocal", "labolocalW1F1");
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);
	// Web Server Root URL
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/wifimanager.html", "text/html");
	});
		
	server.serveStatic("/", SPIFFS, "/");
	
	server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
		int params = request->params();
		for(int i=0;i<params;i++){
			AsyncWebParameter* p = request->getParam(i);
			if(p->isPost()){
				// HTTP POST ssid value
				if (p->name() == PARAM_INPUT_1) {
					ssid = p->value().c_str();
					Serial.print("SSID set to: ");
					Serial.println(ssid);
					writeFile(SPIFFS, ssidPath, ssid.c_str(), 0);
				}
				// HTTP POST ssidKey value
				if (p->name() == PARAM_INPUT_2) {
					ssidKey = p->value().c_str();
					Serial.print("Password set to: ");
					Serial.println(ssidKey);
					writeFile(SPIFFS, ssidKeyPath, ssidKey.c_str(), 0);
				}
				// HTTP POST timezone value
				if (p->name() == PARAM_INPUT_3) {
					timezone = p->value().c_str();
					Serial.print("Timezone set to: ");
					Serial.println(timezone);
					writeFile(SPIFFS, timezonePath, timezone.c_str(), 0);
				}
				// HTTP POST user email value
				if (p->name() == PARAM_INPUT_4) {
					ulogin = p->value().c_str();
					Serial.print("Login set to: ");
					Serial.println(ulogin);
					writeFile(SPIFFS, uloginPath, ulogin.c_str(), 0);
				}
				// HTTP POST user password value
				if (p->name() == PARAM_INPUT_5) {
					upass = p->value().c_str();
					Serial.print("Password saved.");
					Serial.println(upass);
					writeFile(SPIFFS, upassPath, upass.c_str(), 0);
				}
				// HTTP POST API key value
				if (p->name() == PARAM_INPUT_6) {
					apiKey = p->value().c_str();
					Serial.print("API key saved.");
					Serial.println(apiKey);
					writeFile(SPIFFS, apiKeyPath, apiKey.c_str(), 0);
				}
				// HTTP POST RTDB URL value
				if (p->name() == PARAM_INPUT_7) {
					rtdbURL = p->value().c_str();
					Serial.print("RTDB URL saved");
					Serial.println(rtdbURL);
					writeFile(SPIFFS, rtdbURLPath, rtdbURL.c_str(), 0);
				}
			}
		}
		request->send(200, "text/plain", "Setup done. Econologist will restart.");
		delay(3000);
		ESP.restart();
	});
	// Make a link button
	server.on("/visitus", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->redirect("https://www.labolocal.com/");
	});
	server.begin();
}

void serveOTA() {
	AsyncElegantOTA.begin(&server);		// Start ElegantOTA
	server.begin();
	Serial.println("HTTP server started");
}

void laboDNS() {
	MDNS.begin("labo");
	delay(1000);
	Serial.println("MDNS started - Go to http://labo.local");
}

void readBME() {
	temperature = bme.readTemperature();
	humidity = bme.readHumidity();
	pressure = bme.readPressure()/100.0F;
	Serial.printf("Temperature: %2.2f\n", temperature);
	Serial.printf("Humidity: %2.2f%%\n", humidity);
	Serial.printf("Pressure: %2.2f\n", pressure);
}

void sendFloat(String path, float value) {
	if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
		// Serial.printf("PASSED %2.2f on %s\n", value, path.c_str());
	}
	else {
		Serial.println("FAILED");
		Serial.println("REASON: " + fbdo.errorReason());
	}
}

void sendInt(String path, float value) {
	if (Firebase.RTDB.setInt(&fbdo, path.c_str(), value)){
		// Serial.printf("PASSED %2.2f on %s\n", value, path.c_str());
	}
	else {
		Serial.println("FAILED");
		Serial.println("REASON: " + fbdo.errorReason());
	}
}

void sendBME() {
	sendFloat(parentPath + tempPath, temperature);
	sendFloat(parentPath + humPath, humidity);
	sendFloat(parentPath + presPath, pressure);
	sendFloat(readerPath + tempPath, temperature);
	sendFloat(readerPath + humPath, humidity);
	sendFloat(readerPath + presPath, pressure);
}

// void sendTimestamp() {
// 	Serial.printf("\ntimestamp sendTimestamp1 = %i\n", timestamp);
// 	sendInt(parentPath + timePath, timestamp);
// 	sendFloat(readerPath + timePath, timestamp);
// 	Serial.printf("\ntimestamp sendTimestamp2 = %i\n", timestamp);
// }

void sendDS18B20() {
	sendFloat(parentPath + waterPath, water);
	sendFloat(readerPath + waterPath, water);
}

void readDS18B20() {
	ds18b20.requestTemperatures();
	delay(1000);
	water = ds18b20.getTempCByIndex(0);
	Serial.printf("Water: %2.2f\n", water);
}

void sendEC() {
	sendFloat(parentPath + ecPath, ec);
	sendFloat(readerPath + ecPath, ec);
}

void sendPH() {
	sendFloat(parentPath + phPath, ph);
	sendFloat(readerPath + phPath, ph);
}

void sendLevel() {
	sendFloat(parentPath + levelPath, level);
	sendFloat(readerPath + levelPath, level);
}

void readPH() {
	Wire.beginTransmission(ph_address); 	//call the circuit by its ID number.
	Wire.write('r');						//hard code r to read continually
	Wire.endTransmission();					//end the I2C data transmission.
	delay(time_);							//wait the correct amount of time for the circuit to complete its instruction.

	Wire.requestFrom(ph_address, 20, 1);	//call the circuit and request 20 bytes (this may be more than we need)
	code = Wire.read();						//the first byte is the response code, we read this separately.

	switch (code)
	{ //switch case based on what the response code is.
	case 1:
		Serial.println("Success");
		break;

	case 2:
		Serial.println("Failed");
		break;

	case 254:
		Serial.println("Pending");
		break;

	case 255:
		Serial.println("No Data");
		break;
	}

	while (Wire.available())
	{								//are there bytes to receive.
		in_char = Wire.read();		//receive a byte.
		ph_data[i] = in_char;		//load this byte into our array.
		i += 1;						//incur the counter for the array element.
		if (in_char == 0)
		{							//if we see that we have been sent a null command.
			i = 0;					//reset the counter i to 0.
			Wire.endTransmission();	//end the I2C data transmission.
			break;					//exit the while loop.
		}
	}
	ph = atof(ph_data);
}

void readEC() {
	ec = random(100, 500);
	Serial.printf("EC: %2.2f\n", ec);
}

void readLevel() {
	// Clears the trigPin
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	// Sets the trigPin on HIGH state for 10 micro seconds
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	
	// Reads the echoPin, returns the sound wave travel time in microseconds
	duration = pulseIn(echoPin, HIGH);
	
	// Calculate the distance
	distanceCm = duration * SOUND_SPEED/2;

	Serial.printf("Distance (cm): %2.2f\n", distanceCm);
	// Serial.println(distanceCm);
	
	// Converts distance to actual level in liter
	level = distanceCm * 10;
	Serial.printf("Level: %2.2fl\n", level);

	delay(1000);
}

void readSensors() {
	readBME();
	readDS18B20();
	readEC();
	readPH();
	readLevel();
}

void sendReadings() {
	// sendTimestamp();
	sendBME();
	sendDS18B20();
	sendEC();
	sendPH();
	sendLevel();
}

void initLevel() {
	pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
	pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

// void sendString(String path, String value) {
// 	if (Firebase.RTDB.setString(&fbdo, path.c_str(), value)){
// 		Serial.printf("PASSED %2.2f on %s\n", value, path.c_str());
// 	}
// 	else {
// 		Serial.println("FAILED");
// 		Serial.println("REASON: " + fbdo.errorReason());
// 	}
// }