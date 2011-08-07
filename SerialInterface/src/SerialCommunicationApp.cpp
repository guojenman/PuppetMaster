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
    puppet.setup();
}


void SerialCommunicationApp::update()
{	
	puppet.update();
}

void SerialCommunicationApp::keyUp(KeyEvent event)
{
    if(puppet.serialCommand == "1" || puppet.serialCommand == "2")
        puppet.serialCommand = "3";
    if(puppet.serialCommand == "4" || puppet.serialCommand == "5")
        puppet.serialCommand = "6";
}

void SerialCommunicationApp::keyDown( KeyEvent event )
{
    if(event.getCode() == 49){
        puppet.setCommand("1");
        puppet.update();}
    if(event.getCode() == 50){
        puppet.setCommand("2");
        puppet.update();}
    if(event.getCode() == 51){
        puppet.setCommand("3");
        puppet.update();}
    if(event.getCode() == 52){
        puppet.setCommand("4");
        puppet.update();}
    if(event.getCode() == 53){
        puppet.setCommand("5");
        puppet.update();}
    if(event.getCode() == 54){
        puppet.setCommand("6");
        puppet.update();}
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
	//printf("click to test serial:\nnBytes read %i\nnTimes read %i\nread: %s\n(at time %0.3f)", nBytesRead, nTimesRead, lastString, readTime);
	
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );		
	
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( SerialCommunicationApp, RendererGl )
