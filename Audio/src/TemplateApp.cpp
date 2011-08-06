#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "AudioClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TemplateApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void keyDown( KeyEvent event);
    AudioClient client;
};

void TemplateApp::setup()
{
    client = AudioClient::AudioClient();
    client.play("1085.mp3");
}

void TemplateApp::mouseDown( MouseEvent event )
{
        client.togglePause();
}

void TemplateApp::keyDown( KeyEvent event )
{
    if(event.getCode() == 273)
        client.increaseVolume(0.2f);
    if(event.getCode() == 274)
        client.decreaseVolume(0.2f);
    if(event.getCode() == 275)
        client.increaseSpeed(0.05f);
    if(event.getCode() == 276)
        client.decreaseSpeed(0.05f);
    if(event.getCode() == 32)
        client.reset();
}

void TemplateApp::update()
{
    client.update();
}

void TemplateApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
    
}


CINDER_APP_BASIC( TemplateApp, RendererGl )
