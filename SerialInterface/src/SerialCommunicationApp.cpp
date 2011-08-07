#include "cinder/app/AppBasic.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "ArduinoCommandInterface.h"
#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;


// We'll create a new Cinder Application by deriving from the BasicApp class
class SerialCommunicationApp : public AppBasic {
public:
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void setup();
    void keyDown( KeyEvent event );
    void keyUp( KeyEvent event );
    void mouseDown(MouseEvent event);
    
    ArduinoCommandInterface puppet;
};


void SerialCommunicationApp::setup()
{
	puppet = ArduinoCommandInterface();
    puppet.setup("tty.usbserial-A700dYVr", true); //windows use: "COM6"
}


void SerialCommunicationApp::update()
{	
	puppet.update();
}

void SerialCommunicationApp::keyUp(KeyEvent event)
{
	puppet.resetCommand();
}

void SerialCommunicationApp::keyDown( KeyEvent event )
{
	string s;
	s = s + event.getChar();
	puppet.setCommand(s);
	puppet.update();
}

void SerialCommunicationApp::mouseDown(MouseEvent event){
	if(puppet.bSendSerialMessage)
        puppet.bSendSerialMessage = false;
    else
        puppet.bSendSerialMessage = true;        
}


void SerialCommunicationApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );		
	
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( SerialCommunicationApp, RendererGl )
