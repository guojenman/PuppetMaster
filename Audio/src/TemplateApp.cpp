#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TemplateApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void TemplateApp::setup()
{
}

void TemplateApp::mouseDown( MouseEvent event )
{
}

void TemplateApp::update()
{
}

void TemplateApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP_BASIC( TemplateApp, RendererGl )
