#pragma region global

#ifndef USB_Serial
#define USB_Serial _UART1_
#define UNDEFINE_USB_SERIAL
#endif

#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <shiftOut.hpp>

#include <arduino_secrets.h>
#include <intervals.hpp>
#include <toSevSeg.hpp>

void debugUsb();

constexpr uint32_t usbBaud = 115200;

constexpr char broker [] = "192.168.0.2"; //ip mqtt broker
constexpr uint16_t port = 1883; //port mqtt
constexpr char topic [] = "/AlexaTimer/sekbisende"; //mqtt topic

constexpr pin_size_t dataPin = 5, clockPin = 6, blankPin = 7;

char ssid [] = SECRET_SSID; //SSID WiFi
char pass [] = SECRET_PASS; //Passwort WiFi
char user [] = SECRET_USER; //mqtt Username
char clientPass [] = SECRET_CLIENT_PASS; //mqtt passwort

timer_t prevMillisNtpToVar = 0; //reserved
timer_t prevMillisMqttPoll = 0; //reserved
timer_t prevMillisDebug = 0; //reserved
timer_t prevMillisCdwn = 0; //reserved
timer_t prevMillisShiftOut = 0; //reserved

bool cdwnStart = false; //is countdown running?

uint16_t receivedSec = 0; //how many seconds are left on the timer (according to mqtt)
uint8_t timerHrs = 0; //whole remaining hours
uint8_t timerMins = 0; //whole remaining minutes
uint8_t timerSecs = 0; //whole remaining seconds

uint8_t hours = 0; //data from NTP
uint8_t mins = 0; //data from NTP

uint8_t firstNum = 0;   // Number for the first digit of the 7-segment display
uint8_t secondNum = 0;  // Number for the second digit of the 7-segment display
uint8_t thirdNum = 0;   // Number for the third digit of the 7-segment display
uint8_t fourthNum = 0;   // Number for the fourth digit of the 7-segment display

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ptbtime1.ptb.de", 7200, INTERVAL1M); //ntpserver, timeoffset in s (2h), refreshinterval from server in ms (1min)

#pragma endregion

void setup(){
	char clientId [15]; //max length clientid for mqtt
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(blankPin, OUTPUT);
	digitalWrite(LED_BUILTIN, 0);
	digitalWrite(dataPin, 0);
	digitalWrite(clockPin, 0);
	digitalWrite(blankPin, 1);
	USB_Serial.begin(usbBaud);
	while(!USB_Serial); //wait for native USB
	USB_Serial.print("USBSerial Initialised at ");
	USB_Serial.print(usbBaud);
	USB_Serial.println(" baud");
	USB_Serial.print("Trying to connect to ");
	USB_Serial.println(ssid);

	while(WiFi.begin(ssid, pass) != WL_CONNECTED){
		USB_Serial.println("WiFi connection failed!");
		delay(500);
	}

	USB_Serial.println("WiFi connection established!");

	delay(random(100, 500)); //bring some randomness into the clientid

	snprintf(clientId, 15, "UnoR4WiFi_%lu", millis() );

	delay(100);

	mqttClient.setId(clientId);
	USB_Serial.print("ClientID: ");
	USB_Serial.println(clientId);
	mqttClient.setUsernamePassword(user, clientPass);

	delay(100);

	while(!mqttClient.connect(broker, port)){
		USB_Serial.print("MQTT connection failed! Error code = ");
		USB_Serial.println(mqttClient.connectError());
		delay(INTERVAL1S);
	}
	USB_Serial.println("MQTT connection established!");
	mqttClient.subscribe(topic, 0);

	timeClient.begin();

	timeClient.update();

	USB_Serial.println("Leaving Setup.");
	USB_Serial.println();

	prevMillisCdwn = millis();
}

void loop(){
	if(cdwnStart){
		if(timerHrs == 0){
			firstNum = timerMins / 10;
			secondNum = timerMins % 10;
			thirdNum = timerSecs / 10;
			fourthNum = timerSecs % 10;
		}else{
			firstNum = timerHrs / 10;
			secondNum = timerHrs % 10;
			thirdNum = timerMins / 10;
			fourthNum = timerMins % 10;
		}
	}else{
		firstNum = hours / 10;
		secondNum = hours % 10;
		thirdNum = mins / 10;
		fourthNum = mins % 10;
	}

	if(millis() - prevMillisShiftOut > INTERVAL10HZ) {
		prevMillisShiftOut += INTERVAL10HZ;

		// Array for shiftOut overload from "RENoMafex/shiftOut" on GitHub.
		uint8_t vals[] = {
			toSevSeg(firstNum),
			toSevSeg(secondNum),
			toSevSeg(thirdNum),
			toSevSeg(fourthNum)
		};

		shiftOut(dataPin, clockPin, blankPin, MSBFIRST, vals, sizeof(vals)/sizeof(vals[0]));
	}

	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

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
		USB_Serial.println();
		USB_Serial.print("MQTTrx: ");
		uint8_t bitCount = messageSize + 1;
		char byteIn[bitCount] = {};
		bitCount = 0;
		while (mqttClient.available()) {
			byteIn[bitCount] = mqttClient.read();
			USB_Serial.print(byteIn[bitCount]);
			bitCount++;
		}

		USB_Serial.println();

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
	USB_Serial.println();
	USB_Serial.println("---Start of Debugging---");

	USB_Serial.print("ReceivedSec: ");
	USB_Serial.println(receivedSec);
	USB_Serial.print("TimerRemainingHRS: ");
	USB_Serial.println(timerHrs);
	USB_Serial.print("TimerRemainingMIN: ");
	USB_Serial.println(timerMins);
	USB_Serial.print("TimerRemainingSEC: ");
	USB_Serial.println(timerSecs);
	USB_Serial.print("NTPhrs: ");
	USB_Serial.println(hours);
	USB_Serial.print("NTPmins: ");
	USB_Serial.println(mins);
	USB_Serial.print("CdwnRun: ");
	USB_Serial.println(cdwnStart);


	USB_Serial.println("---End of Debugging---");
	USB_Serial.println();
}

#ifdef UNDEFINE_USB_SERIAL
#undef UNDEFINE_USB_SERIAL
#undef USB_Serial
#endif
