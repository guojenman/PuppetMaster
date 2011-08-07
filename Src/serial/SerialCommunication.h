#include "cinder/app/AppBasic.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "ArduinoCommandInterface.h"
#include <sstream>




// We'll create a new Cinder Application by deriving from the BasicApp class
class SerialCommunicationApp
{
public:
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void setup();
    void keyDown( ci::app::KeyEvent event );
    void keyUp( ci::app::KeyEvent event );
    void mouseDown( ci::app::MouseEvent event);

    ArduinoCommandInterface puppet;
};
