
#include "ArduinoCommandInterface.h"


void ArduinoCommandInterface::setup(string device, bool debug = false)
{
	isDebug = debug;
	sendSerialMessage = true;
	if(isDebug) printDevices();	
	bindDevice(device);
}

void ArduinoCommandInterface::bindDevice(string device)
{
    try {
		Serial::Device dev = Serial::findDeviceByNameContains(device);
		serial = Serial( dev, 19200);
	}
	catch( ... ) {
		console() << "There was an error initializing the serial device!" << std::endl;
		exit( -1 );
	}
}

void ArduinoCommandInterface::printDevices()
{
    // print the devices
	const vector<Serial::Device> &devices( Serial::getDevices() );
	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		console() << "Device: " << deviceIt->getName() << endl;
	}
}

void ArduinoCommandInterface::sendMessage(string message)
{
    unsigned char m_Test[20];    
    std::strcpy( (char*) m_Test, message.c_str() );
    serial.writeByte(m_Test[0]);
	serial.writeByte(m_Test[0]);
	serial.writeByte(m_Test[0]);
	serial.writeByte(m_Test[0]);
    
//	if(isDebug) console() << serialCommand;
}

void ArduinoCommandInterface::setCommand(string c)
{
    serialCommand = c;

}

void ArduinoCommandInterface::resetCommand()
{

	if(serialCommand == "1" || serialCommand == "2") serialCommand = "3";
	if(serialCommand == "4" || serialCommand == "5") serialCommand = "6";
	if(serialCommand == "q" || serialCommand == "w") serialCommand = "e";
	if(serialCommand == "r" || serialCommand == "t") serialCommand = "y";
}

bool ArduinoCommandInterface::isValidCommand(string c)
{
	return (c == "1" || c == "2" || c == "3" || c == "4" || c == "5" || c == "6" || c == "q" || c == "w" || c =="e" || c =="r" || c == "t" || c == "y");
}

void ArduinoCommandInterface::update()
{	
	if (sendSerialMessage && isValidCommand(serialCommand)) sendMessage(serialCommand);
}
