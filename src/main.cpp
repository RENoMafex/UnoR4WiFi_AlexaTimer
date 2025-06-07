/*
 *
 * Sorry for German Comments
 *
*/
#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <arduino_secrets.h>
#include <intervals.hpp>

void debugUsb();

typedef uint32_t msv;

const uint32_t usbBaud = 115200;

constexpr char broker [] = "192.168.0.2"; //ip mqtt broker
const uint16_t port = 1883; //port mqtt
constexpr char topic [] = "/AlexaTimer/sekbisende"; //mqtt topic

char ssid [] = SECRET_SSID; //SSID WiFi
char pass [] = SECRET_PASS; //Passwort WiFi
char user [] = SECRET_USER; //mqtt Username
char clientPass [] = SECRET_CLIENT_PASS; //mqtt passwort

msv prevMillisNtpToVar = 0; //reserved
msv prevMillisMqttPoll = 0; //reserved
msv prevMillisDebug = 0; //reserved
msv prevMillisCdwn = 0; //reserved
msv prevMillisUARTtx = 0; //reserved

bool cdwnStart = false; //is countdown running?

uint16_t receivedSec = 0; //how many seconds are left on the timer (according to mqtt)
uint8_t timerHrs = 0; //whole remaining hours
uint8_t timerMins = 0; //whole remaining minutes
uint8_t timerSecs = 0; //whole remaining seconds

uint8_t hours = 0; //data from NTP
uint8_t mins = 0; //data from NTP

uint8_t firstByte = 0; //number for the first two digits of the 7seg display
uint8_t secondByte = 0; //number for the second two digits of the 7seg display

bool blinkVar = false; //flips once per loop() cycle

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 7200, INTERVAL1M); //ntpserver, timeoffset in s (2h), refreshinterval from server in ms (1min)

void setup(){
	char clientId [15]; //max length clientid mqtt

	pinMode(LED_BUILTIN, 1);
	digitalWrite(LED_BUILTIN, 1);
	Serial.begin(usbBaud);
	while(!Serial); // wait for native usb
	Serial.print("USBSerial Initialised at ");
	Serial.print(usbBaud);
	Serial.println(" baud");
	Serial.print("Trying to connect to ");
	Serial.println(ssid);

	while(WiFi.begin(ssid, pass) != WL_CONNECTED){
		Serial.println("WiFi connection failed!");
		delay(500);
	}

	Serial.println("WiFi connection established!");

	delay(random(100, 500)); //randomization for clientid (if the arduino restarts/resets)

	snprintf(clientId, 15, "UnoR4WiFi_%lu", millis() );

	delay(100);

	mqttClient.setId(clientId);
	Serial.print("ClientID: ");
	Serial.println(clientId);
	mqttClient.setUsernamePassword(user, clientPass);

	delay(100);

	while(!mqttClient.connect(broker, port)){
		Serial.print("MQTT connection failed! Error code = ");
		Serial.println(mqttClient.connectError());
		delay(INTERVAL1S);
	}
	Serial.println("MQTT connection established!");
	mqttClient.subscribe(topic, 0);

	timeClient.begin();

	timeClient.update();


	//UART start
	Serial1.begin(9600);

	Serial.println("Leaving Setup.");
	Serial.println();
	delay(500);
	prevMillisCdwn = millis();
}

void loop(){
	if(cdwnStart){
		if(timerHrs == 0){
			firstByte = timerMins;
			secondByte = timerSecs;
		}else{
			firstByte = timerHrs;
			secondByte = timerMins;
		}
	}else{
		firstByte = hours;
		secondByte = mins;
	}

	if(millis() - prevMillisUARTtx > INTERVAL10HZ){
		prevMillisUARTtx = millis();
		Serial1.write(firstByte);
		Serial1.write(secondByte);
		Serial1.write(255);
	} //Daten über UART Ausgeben

	digitalWrite(LED_BUILTIN, blinkVar);
	blinkVar =! blinkVar;
	if(millis() - prevMillisCdwn > INTERVAL1S){
		prevMillisCdwn = prevMillisCdwn + INTERVAL1S;
		if(cdwnStart){
			timerSecs--;
			if(timerSecs == 255){
				timerSecs = 59;
				timerMins--;
			}
			if(timerMins == 255){
				timerMins = 59;
				timerHrs--;
			}
			if(timerHrs == 255){
				timerSecs = 0;
				timerMins = 0;
				timerHrs = 0;
				cdwnStart = 0;
			}
		}
	}
	// Countdown incl Stoppen des Countdowns bei 0
	if(uint8_t messageSize = mqttClient.parseMessage()){
		Serial.println();
		Serial.print("MQTTrx: ");
		uint8_t bitCount = messageSize + 1;
		char byteIn[bitCount];
		bitCount = 0;
		while (mqttClient.available()) {
			byteIn[bitCount] = mqttClient.read();
			Serial.print(byteIn[bitCount]);
			bitCount++;
		}

		Serial.println();

		receivedSec = atoi(byteIn);

		timerHrs = receivedSec/3600;
		timerMins = (receivedSec%3600)/60;
		timerSecs = receivedSec%60;

		if(receivedSec){
			cdwnStart = 1;
		}else{
			cdwnStart = 0;
		}
	}

	//poll ntp and write to vars
	if(millis() - prevMillisNtpToVar > INTERVAL5S){
		prevMillisNtpToVar = millis();
		timeClient.update(); // really needed every time??
		hours = timeClient.getHours();
		mins = timeClient.getMinutes();
	}

	//hold connection to mqtt server
	if(millis() - prevMillisMqttPoll > INTERVAL1M){
		prevMillisMqttPoll = millis();
		mqttClient.poll();
	}

	//output some vars via usb
	if(millis() - prevMillisDebug > INTERVAL1S){
		prevMillisDebug = millis();
		debugUsb();
	}
}

void debugUsb(){
	Serial.println();
	Serial.println("---Start of Debugging---");

	Serial.print("receivedSec: ");
	Serial.println(receivedSec);
	Serial.print("TimerRestHRS: ");
	Serial.println(timerHrs);
	Serial.print("TimerRestMIN: ");
	Serial.println(timerMins);
	Serial.print("TimerRestSEC: ");
	Serial.println(timerSecs);
	Serial.print("NTPhrs: ");
	Serial.println(hours);
	Serial.print("NTPmins: ");
	Serial.println(mins);
	Serial.print("CdwnRun: ");
	Serial.println(cdwnStart);


	Serial.println("---End of Debugging---");
	Serial.println();
}