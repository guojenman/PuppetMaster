#include "cinder/app/AppBasic.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the BasicApp class
class ArduinoCommandInterface {
public:
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void setup();
	void mouseDown(MouseEvent event);
    void setLimbCommands(int rightArm, int leftArm, int rightLeg, int leftLeg, int rightEar, int leftEar);
    void setCommand(string c);
    void firstContact();
    void printDevices();
    void bindDevice(string device);
	void sendMessage(string message);
	bool bSendSerialMessage, bTextureComplete;			// a flag for sending serial
    Serial serial;
    int commandCount;
    
    string serialCommand;
    
	std::string lastString;	
};