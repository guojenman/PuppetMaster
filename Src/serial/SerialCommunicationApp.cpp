#include "SerialCommunication.h"
using namespace ci;
using namespace ci::app;
using namespace std;

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
	if(puppet.sendSerialMessage)
        puppet.sendSerialMessage = false;
    else
        puppet.sendSerialMessage = true;        
}
