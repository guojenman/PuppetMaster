#include "cinder/app/AppBasic.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

#define LEFT_ARM_UP "1"
#define LEFT_ARM_DOWN "2"
#define LEFT_ARM_CLEAR "3"

#define RIGHT_ARM_UP "4"
#define RIGHT_ARM_DOWN "5"
#define RIGHT_ARM_CLEAR "6

#define LEFT_LEG_UP "q"
#define LEFT_LEG_DOWN "w"
#define LEFT_LEG_CLEAR "e"

#define RIGHT_LEG_UP "r"
#define RIGHT_LEG_DOWN "t"
#define RIGHT_LEG_CLEAR "y"


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
