#include "cinder/app/AppBasic.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"

#include "WuCinderNITE.h"
#include <btBulletDynamicsCommon.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class PuppetMaster : public AppBasic {
public:
	void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void update();
	void draw();
	void shutdown();

	void keyUp(KeyEvent event);

	WuCinderNITE* ni;

	CameraPersp mCam;
	Vec3f mCamEye;
	Vec3f mCamLookAt;
	Vec4f lightPosition;
};

void PuppetMaster::prepareSettings( AppBasic::Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void PuppetMaster::setup()
{
	XnMapOutputMode mapMode;
	mapMode.nFPS = 30;
	mapMode.nXRes = 640;
	mapMode.nYRes = 480;

//	ni = WuCinderNITE::getInstance();
//	ni->setup("Resources/Sample-User.xml", mapMode, true, true);
//	ni->startUpdating();

	mCamEye = Vec3f(0, 0, -500.0f);
	mCamLookAt = Vec3f::zero();

}


void PuppetMaster::update()
{
	mCam.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 1000.0f);
	mCam.lookAt(mCamEye, mCamLookAt);

}

void PuppetMaster::draw()
{
	gl::clear(ColorA(0, 0, 0, 0), true);
}

void PuppetMaster::shutdown()
{
	console() << "quitting..." << std::endl;
	//	ni->shutdown();
}

void PuppetMaster::keyUp(KeyEvent event)
{
	if (event.getChar() == KeyEvent::KEY_q) {
		quit();
	}
}

CINDER_APP_BASIC( PuppetMaster, RendererGl )
