#include "cinder/app/AppBasic.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "RagDollController.h"
#include "RagDoll.h"
#include "cinder/Quaternion.h"
#include "cinder/MayaCamUI.h"

#include "WuCinderNITE.h"
#include <btBulletDynamicsCommon.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class PuppetMaster : public AppBasic {
public:
	void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void setupCamera();

	void		mouseDown( ci::app::MouseEvent event );
	void		mouseMove( ci::app::MouseEvent event );
	void		mouseDrag( ci::app::MouseEvent event );
	void		mouseUp( ci::app::MouseEvent event );

	void update();
	void draw();
	void shutdown();

	void keyUp(KeyEvent event);

	WuCinderNITE* ni;
	ci::MayaCamUI		_mayaCam;
	Vec2f				_mousePosition;
	bool 				_mouseIsDown;


	CameraPersp mCam;
	RagDollController *_ragdollController;


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

	_ragdollController = new RagDollController();
	_ragdollController->initPhysics();

//	ni = WuCinderNITE::getInstance();
//	ni->setup("Resources/Sample-User.xml", mapMode, true, true);
//	ni->startUpdating();

	mCamEye = Vec3f(0, 2, -5.0f);
	mCamLookAt = Vec3f::zero();
}

void PuppetMaster::setupCamera()
{
	// Camera perspective properties
	float cameraFOV			= 60.0f;
	float cameraNear		= 1.0f;
	float cameraFar			= 1000000.0;

	ci::Vec3f p = ci::Vec3f::one() * 2000.0f;// Start off this far away from the center
	ci::CameraPersp cam = ci::CameraPersp( getWindowWidth(), getWindowHeight(), cameraFOV );

	cam.setWorldUp( ci::Vec3f(0, 1, 0) );
	cam.setEyePoint( ci::Vec3f(0, 0, 0 ) );
	cam.setCenterOfInterestPoint( ci::Vec3f::zero() );
	cam.setPerspective( cameraFOV, getWindowAspectRatio(), cameraNear, cameraFar );
	cam.setViewDirection( ci::Vec3f(0, 0, 1 ) );

	// Set mayacamera
	_mayaCam.setCurrentCam( cam );
}


void PuppetMaster::update()
{
	_ragdollController->clientMoveAndDisplay( 16.0 );
	mCam.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 1000.0f);
	mCam.lookAt(mCamEye, mCamLookAt);
}

void PuppetMaster::draw()
{
	gl::clear(ColorA(0, 0, 0, 0), true);
	gl::pushMatrices();
	gl::setMatrices( _mayaCam.getCamera() );

	RagDoll* thedoll = _ragdollController->ragDoll;

	// Setup some damping on the m_bodies
	// TODO: HARD CODED BODYPART_COUNT
		for (int i = 0; i < RagDoll::BODYPART_COUNT; ++i)
		{
			btRigidBody* body = _ragdollController->ragDoll->m_bodies[i];

			btTransform trans;
			body->getMotionState()->getWorldTransform( trans );

			float mSize = 0.1;
			ci::Vec3f pos = ci::Vec3f( trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ() );
			btQuaternion btQuat = trans.getRotation();
			ci::Quatf rotation = ci::Quatf( btQuat.getX(), btQuat.getY(), btQuat.getZ(), btQuat.getW() );

			ci::gl::pushModelView();
				ci::gl::translate( pos );
				ci::gl::rotate( rotation );
				ci::gl::drawCube( ci::Vec3f::zero(), ci::Vec3f(mSize, mSize, mSize) * 2 );
			ci::gl::popMatrices();
		}
		gl::popMatrices();
}

void PuppetMaster::shutdown()
{
	console() << "quitting..." << std::endl;
	//	ni->shutdown();
}

void PuppetMaster::mouseDown( ci::app::MouseEvent event )
{
	_mouseIsDown = true;
	_mayaCam.mouseDown( event.getPos() );
}

void PuppetMaster::mouseDrag( ci::app::MouseEvent event )
{
	_mayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMetaDown(), event.isRightDown() );
	_mousePosition = event.getPos();
}

void PuppetMaster::mouseMove( ci::app::MouseEvent event )
{
	_mayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMetaDown(), event.isRightDown() );
	_mousePosition = event.getPos();
}

void PuppetMaster::mouseUp( ci::app::MouseEvent event )
{
	_mouseIsDown = false;
	_mayaCam.mouseDown( event.getPos() );
}

void PuppetMaster::keyUp(KeyEvent event)
{
	if (event.getChar() == KeyEvent::KEY_q) {
		quit();
	}
}

CINDER_APP_BASIC( PuppetMaster, RendererGl )
