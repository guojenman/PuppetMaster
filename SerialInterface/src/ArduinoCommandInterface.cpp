
#include "ArduinoCommandInterface.h"

void ArduinoCommandInterface::setup()
{
    commandCount = 0;
	lastString = "";	
	bSendSerialMessage = true;
	printDevices();
	bindDevice("tty.usbserial-A700dYVr");		
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
    //serial.writeString(message);
    serial.writeByte(m_Test[0]);
    console() << serialCommand;
    //lastString = message;
    //serial.flush();
}


void ArduinoCommandInterface::setLimbCommands(int rightArm, int leftArm, int rightLeg, int leftLeg, int rightEar, int leftEar)
{
    std::stringstream Num;
    Num << rightArm << "," << leftArm << "," << rightLeg << "," << leftLeg << "," << rightEar << "," << leftEar << "|";
    //serialCommand = Num.str();
}

void ArduinoCommandInterface::setCommand(string c)
{
    serialCommand = c;
}

void ArduinoCommandInterface::update()
{	
	if (bSendSerialMessage)
	{
        //if(commandCount < 100 && (serialCommand == "3" || serialCommand == "6") ){
        sendMessage(serialCommand);
        sendMessage(serialCommand);
        sendMessage(serialCommand);
        sendMessage(serialCommand);
        sendMessage(serialCommand);
        //commandCount++;
        //serialCommand = "0";
          //  if(serialCommand == "2" || serialCommand == "1"){
            //    serialCommand = "3";
              //  serialCommand = "3";
                //serialCommand = "3";
               // serialCommand = "3";
                //serialCommand = "3";
            //}
            //if(serialCommand == "4" || serialCommand == "5"){
              //  serialCommand = "6";
                //serialCommand = "6";
                //serialCommand = "6";
                //serialCommand = "6";
                //serialCommand = "6";
            //}
           // if(serialCommand == "6" || serialCommand == "3")
             //   serialCommand = "0";
        
        //}
        //else{
            
          //  commandCount = 0;
        //}
	}
}

void ArduinoCommandInterface::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	//printf("click to test serial:\nnBytes read %i\nnTimes read %i\nread: %s\n(at time %0.3f)", nBytesRead, nTimesRead, lastString, readTime);
	
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0, 0, 0, 0 );		
	
}