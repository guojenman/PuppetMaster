#include "cinder/app/AppBasic.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

class ArduinoCommandInterface {
public:
	void update();
	void setup(string device, bool debug);
    void setCommand(string c);
    void firstContact();
    void printDevices();
    void bindDevice(string device);
	void resetCommand();
	bool isValidCommand(string c);
	void sendMessage(string message);
	bool sendSerialMessage, isDebug;
    Serial serial;
    string serialCommand;
};