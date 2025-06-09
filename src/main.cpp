
#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <arduino_secrets.h>
#include <intervals.hpp>

void debugUsb();

//overload for using latch pin
void shiftOut(pin_size_t dataPin, pin_size_t clockPin, pin_size_t latchPin, BitOrder bitOrder, uint8_t val);

constexpr uint32_t usbBaud = 115200;

constexpr char broker [] = "192.168.0.2"; //ip mqtt broker
constexpr uint16_t port = 1883; //port mqtt
constexpr char topic [] = "/AlexaTimer/sekbisende"; //mqtt topic

constexpr pin_size_t dataPin = 5, clockPin = 6, blankPin = 7;

char ssid [] = SECRET_SSID; //SSID WiFi
char pass [] = SECRET_PASS; //Passwort WiFi
char user [] = SECRET_USER; //mqtt Username
char clientPass [] = SECRET_CLIENT_PASS; //mqtt passwort

uint32_t prevMillisNtpToVar = 0; //reserved
uint32_t prevMillisMqttPoll = 0; //reserved
uint32_t prevMillisDebug = 0; //reserved
uint32_t prevMillisCdwn = 0; //reserved
uint32_t prevMillisShiftOut = 0; //reserved

bool cdwnStart = false; //is countdown running?

uint16_t receivedSec = 0; //how many seconds are left on the timer (according to mqtt)
uint8_t timerHrs = 0; //whole remaining hours
uint8_t timerMins = 0; //whole remaining minutes
uint8_t timerSecs = 0; //whole remaining seconds

uint8_t hours = 0; //data from NTP
uint8_t mins = 0; //data from NTP

uint8_t firstByte = 0; //Zahl für die ersten zwei ziffern des 7-Segment
uint8_t secondByte = 0; //Zahl für die letzten zwei ziffern des 7-Segment

bool blinkVar = false; //flips once per loop() cycle

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 7200, INTERVAL1M); //ntpserver, timeoffset in s (2h), refreshinterval from server in ms (1min)

void setup(){
	char clientId [15]; //max length clientid for mqtt
	pinMode(LED_BUILTIN, 1);
	digitalWrite(LED_BUILTIN, blinkVar);
	pinMode(dataPin, 1);
	pinMode(clockPin, 1);
	pinMode(blankPin, 1);
	digitalWrite(dataPin, 0);
	digitalWrite(clockPin, 0);
	digitalWrite(blankPin, 1);
	Serial.begin(usbBaud);
	while(!Serial); //wait for native USB
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

	delay(random(100, 500)); //bring some randomness into the clientid

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

	Serial.println("Leaving Setup.");
	Serial.println();

	prevMillisCdwn = millis();
}

void loop(){
	// is this if statement still needed?
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

	if(millis() - prevMillisShiftOut > INTERVAL10HZ) {
		prevMillisShiftOut += INTERVAL10HZ;

		// insert output to shift regs here!
	}

	digitalWrite(LED_BUILTIN, !digitalRead(blinkVar));

	// Countdown incl stop at 0
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
	// recieve as well as starting the countdown

	//poll NTP and write to vars
	if(millis() - prevMillisNtpToVar > INTERVAL5S){
		prevMillisNtpToVar = millis();
		timeClient.update(); //Necessary every time??
		hours = timeClient.getHours();
		mins = timeClient.getMinutes();
	}

	//poll mqtt once a minute that connection doesnt fail
	if(millis() - prevMillisMqttPoll > INTERVAL1M){
		prevMillisMqttPoll = millis();
		mqttClient.poll();
	}

	//give out some debugging over usb.
	if(millis() - prevMillisDebug > INTERVAL1S){
		prevMillisDebug = millis();
		debugUsb();
	}
}

void debugUsb(){
	Serial.println();
	Serial.println("---Start of Debugging---");

	Serial.print("ReceivedSec: ");
	Serial.println(receivedSec);
	Serial.print("TimerRemainingHRS: ");
	Serial.println(timerHrs);
	Serial.print("TimerRemainingMIN: ");
	Serial.println(timerMins);
	Serial.print("TimerRemainingSEC: ");
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

void shiftOut(pin_size_t dataPin, pin_size_t clockPin, pin_size_t latchPin, BitOrder bitOrder, uint8_t val) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, bitOrder, val);
	digitalWrite(latchPin, HIGH);
}