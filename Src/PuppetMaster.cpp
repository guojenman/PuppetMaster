#include "Resources.h"

#include "cinder/Cinder.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/DisplayList.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Light.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/ip/Hdr.h"
#include "ciDeferred/ciDeferred.h"
#include "cinder/Perlin.h"
#include <iostream>
#include <math.h>



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

#include "GLDebugDrawer.h"
#include "WuCinderNITE.h"
#include <btBulletDynamicsCommon.h>
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ciDeferred;

static const int SHADOW_MAP_RESOLUTION = 1024;


#define DEBUG_DRAW_BULLET 0
#define APP_WIDTH 800
#define APP_HEIGHT 600

class PuppetMaster : public AppBasic {
public:
	void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void resize(ResizeEvent resize);
	void setupCamera();
	void initShadowMap();
	void updateShadowMap();
	void render();

	void		mouseDown( ci::app::MouseEvent event );
	void		mouseMove( ci::app::MouseEvent event );
	void		mouseDrag( ci::app::MouseEvent event );
	void		mouseUp( ci::app::MouseEvent event );
	void 		transformBulletJointsWithNIJoint(XnSkeletonJoint niJoint, int bulletJoint, float yOffset, float zOffset);
	void		dropBulletJointToFloor(int bulletJoint);
	void		pickupBulletJointFromFloor(int bulletJoint);
	void		drawFloorPlane( float floorSize );

	void update();
	void draw();
	void shutdown();

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);

	WuCinderNITE* ni;
	ci::MayaCamUI		_mayaCam;
	Vec2f				_mousePosition;
	bool 				_mouseIsDown;
	bool				_isTracking;
	bool				_seeThroughLightCamera;

	CameraPersp mCam;
	RagDollController *_ragdollController;
	GLDebugDrawer _bulletDebugDraw;


	//ciDeferred
	DisplayList			mDisplayList;
	gl::GlslProg		mShader;
	gl::Fbo				mDepthFbo;
	gl::Light			*mLight0;
	ciGBuffer			mGBuffer;
	ciPostProd			mPostProd;
	float sampleRadius, intensity, scale, bias, jitter, selfOcclusion, blurStrength;

};

void PuppetMaster::prepareSettings( AppBasic::Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
}

void PuppetMaster::setup()
{
	XnMapOutputMode mapMode;
	mapMode.nFPS = 30;
	mapMode.nXRes = 640;
	mapMode.nYRes = 480;
	ni = WuCinderNITE::getInstance();
	ni->setup("Resources/SkeletonRec.oni");
	ni->startUpdating();

	_ragdollController = new RagDollController();
	_ragdollController->initPhysics();
	_isTracking = false;
	_seeThroughLightCamera = false;

	glClearColor(1, 1, 1, 1);
	glPolygonOffset( 1.0f, 1.0f );
	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );

	sampleRadius		= 0.02f;
	intensity			= 2.5f;
	scale				= 0.73f;
	bias				= 0.02f;
	jitter				= 64.0f;
	selfOcclusion		= 0.02f;
	blurStrength		= 4.0f;

	// Setup Camera UI
	//------------------------------------------
	setupCamera();

	// Setup GBuffer and PostProd Layer
	//-------------------------------------------------------------------------------------
	mGBuffer	= ciGBuffer( APP_WIDTH, APP_HEIGHT );
	mPostProd	= ciPostProd( APP_WIDTH, APP_HEIGHT );

	// Setup Light
	//------------------------------------------------------
	mLight0 = new gl::Light( gl::Light::SPOTLIGHT, 0 );
	//mLight0->lookAt( Vec3f( 1.0f, 8.0f, -7.0f ), Vec3f( 0.0f, 2.0f, -5.0f ) );
	mLight0->lookAt( Vec3f( 1.0f, 5.0f, -7.0f ), Vec3f( 0.0f, -3.0f, 10.0f ) );
	mLight0->setAmbient( Color( 0.2f, 0.2f, 0.2f ) );
	mLight0->setDiffuse( Color( 0.5f, 0.5f, 0.7f ) );
	mLight0->setSpecular( Color( 0.5f, 0.5f, 0.7f ) );
	mLight0->setShadowParams( 60.0f, 0.1f, 50.0f );
	mLight0->update(  mCam );
	mLight0->enable();


	// Init shadows
	//------------------------------------------------------
	initShadowMap();

	mShader = gl::GlslProg( loadResource( RES_SHADOWMAP_VERT ), loadResource( RES_SHADOWMAP_FRAG ) );
	mShader.bind();
	mShader.uniform( "depthTexture", 0 );
	mShader.uniform( "aoTexture", 1 );
	mShader.uniform( "albedoTexture", 2 );
	mShader.uniform( "screenSize", Vec2f(getWindowWidth(), getWindowHeight()) );
	mShader.unbind();

	// floor
	mDisplayList = DisplayList( GL_COMPILE );
	mDisplayList.newList();
	drawFloorPlane(20);
	mDisplayList.endList();

	gl::Material material;
	material.setSpecular( ColorA::white() );
	material.setDiffuse( ColorA::white() );
	material.setAmbient( Color( 0.5f, 0.5f, 0.5f ) );
	material.setShininess( 5.0f );
	mDisplayList.setMaterial( material );

	//setFullScreen(true);
}


void PuppetMaster::setupCamera()
{
	mCam.setPerspective( 60, APP_WIDTH / APP_HEIGHT, 0.1f, ni->maxDepth);
	mCam.lookAt(ci::Vec3f(0, 1, -3.0f), ci::Vec3f(0, 2, 3));
}


void PuppetMaster::update()
{
	setupCamera();
	_ragdollController->clientMoveAndDisplay( 16.0 );
}
void PuppetMaster::draw()
{
	gl::clear();
	gl::enableDepthWrite();

	// Update Shadows
	//------------------------------------------------------
	glEnable( GL_LIGHTING );
	updateShadowMap();


	// First Render Everything in our "GBuffer"
	//------------------------------------------------------
	mGBuffer.bindBuffers();

	gl::clear();
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mLight0->update( mCam );
	glPushMatrix();
	gl::setMatrices(  _seeThroughLightCamera ? mLight0->getShadowCamera() : mCam );
		render();
	glPopMatrix();

	mGBuffer.unbindBuffers();


	// Then apply ambient occlusion
	//-------------------------------------------------------------------------------
	mPostProd.ambientOcclusion( &mGBuffer, sampleRadius, intensity, scale, bias, jitter, selfOcclusion  );
	mPostProd.blurDepthDependant( mPostProd.getTexture( POSTPROD_SSAO ), mGBuffer.getTexture( GBUFFER_NORMAL_DEPTH ), POSTPROD_SLOT0, blurStrength );


	// Do the last pass with the shadows
	//------------------------------------------------------
	gl::setViewport(getWindowBounds());
	gl::enableDepthWrite();
	gl::enableDepthRead();

	glEnable( GL_TEXTURE_2D );
	mDepthFbo.bindDepthTexture( 0 );
	mPostProd.bindTexture( POSTPROD_SLOT0, 1 );
	mGBuffer.bindTexture( GBUFFER_ALBEDO, 2 );
	mShader.bind();
	mShader.uniform( "shadowTransMatrix", mLight0->getShadowTransformationMatrix( mCam ) );
	mShader.uniform( "ao", true );
	mShader.uniform( "shadows", true );
	mShader.uniform( "screenSize", Vec2f(getWindowWidth(), getWindowHeight()) );

	mLight0->update( mCam );
	glPushMatrix();
	gl::setMatrices(  _seeThroughLightCamera ? mLight0->getShadowCamera() : mCam );
		render();
	glPopMatrix();

	mShader.unbind();
	mPostProd.unbindTexture( POSTPROD_SLOT0, 1 );
	mGBuffer.unbindTexture( GBUFFER_ALBEDO, 2 );
	mDepthFbo.unbindTexture();

}
void PuppetMaster::render()
{
	mDisplayList.draw();

	if (ni->mUserGen.GetSkeletonCap().IsTracking(1)) {
		if (!_isTracking) {
			_isTracking = true;
			pickupBulletJointFromFloor(RagDoll::BODYPART_HEAD);
			pickupBulletJointFromFloor(RagDoll::BODYPART_LEFT_LOWER_ARM);
			pickupBulletJointFromFloor(RagDoll::BODYPART_LEFT_UPPER_LEG);
			pickupBulletJointFromFloor(RagDoll::BODYPART_RIGHT_LOWER_ARM);
			pickupBulletJointFromFloor(RagDoll::BODYPART_RIGHT_UPPER_LEG);
		}
		float yOffset = 1200.0f;
		float niToBulletScale = 0.001f;
		btVector3 genPivot;

		XnSkeletonJointPosition joint;
		ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1, XN_SKEL_HEAD, joint);
		btVector3 headPivot = btVector3(joint.position.X, joint.position.Y + yOffset, 0) * niToBulletScale;
		_ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD]->setPivotB(headPivot);
		float zOffset = joint.position.Z;

		transformBulletJointsWithNIJoint(XN_SKEL_LEFT_HAND, RagDoll::BODYPART_LEFT_LOWER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_LEFT_KNEE, RagDoll::BODYPART_LEFT_UPPER_LEG, yOffset, zOffset);

		transformBulletJointsWithNIJoint(XN_SKEL_RIGHT_HAND, RagDoll::BODYPART_RIGHT_LOWER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_RIGHT_KNEE, RagDoll::BODYPART_RIGHT_UPPER_LEG, yOffset, zOffset);

	} else {
		if (_isTracking) {
			_isTracking = false;
			dropBulletJointToFloor(RagDoll::BODYPART_HEAD);
			dropBulletJointToFloor(RagDoll::BODYPART_LEFT_LOWER_ARM);
			dropBulletJointToFloor(RagDoll::BODYPART_LEFT_UPPER_LEG);
			dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_LOWER_ARM);
			dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_UPPER_LEG);
		}

	}

	// Draw each rigid body in the doll
	for (int i = 0; i < RagDoll::BODYPART_COUNT; ++i)
	{
		btScalar m[16];
		btDefaultMotionState* myMotionState = (btDefaultMotionState*)_ragdollController->ragDoll->m_bodies[i]->getMotionState();
		myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);

		btCapsuleShape* shape = dynamic_cast<btCapsuleShape*> (_ragdollController->ragDoll->m_shapes[i]);
		float height = shape->getHalfHeight() * 2.0 + shape->getMargin();

		gl::pushModelView();
			glMultMatrixf(m);
//			gl::drawCylinder(shape->getRadius(), shape->getRadius(), height, 8, 4);
			gl::drawCube( Vec3f::zero(), Vec3f(shape->getRadius() * 1.5, height, shape->getRadius() * 1.5));
		gl::popModelView();
	}

}

void PuppetMaster::drawFloorPlane( float floorSize )
{
	// Draw floor plane
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,1.0f); glVertex3f(-floorSize, 0.0f, floorSize);
		glTexCoord2f(1.0f,1.0f); glVertex3f( floorSize, 0.0f, floorSize);
		glTexCoord2f(1.0f,0.0f); glVertex3f( floorSize, 0.0f,-floorSize);
		glTexCoord2f(0.0f,0.0f); glVertex3f(-floorSize, 0.0f,-floorSize);
	glEnd();
}

void PuppetMaster::transformBulletJointsWithNIJoint(XnSkeletonJoint niJoint, int bulletJoint, float yOffset, float zOffset)
{
	btVector3 pivot;
	XnSkeletonJointPosition joint;
	ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1, niJoint, joint);
	pivot = btVector3(joint.position.X, joint.position.Y + yOffset, joint.position.Z - zOffset) * 0.001f;
	_ragdollController->ragDoll->m_constraints[bulletJoint]->setPivotB(pivot);
}

void PuppetMaster::dropBulletJointToFloor(int bulletJoint)
{
//	float floor = 1.0f;
//	btVector3 pivot = _ragdollController->ragDoll->m_constraints[bulletJoint]->getPivotInB();
//	pivot.setY((floor - pivot.getY()) * .2);
//	_ragdollController->ragDoll->m_constraints[bulletJoint]->setPivotB(pivot);
	_ragdollController->m_dynamicsWorld->removeConstraint(_ragdollController->ragDoll->m_constraints[bulletJoint]);
	_ragdollController->ragDoll->m_bodies[bulletJoint]->forceActivationState(ACTIVE_TAG);
	_ragdollController->ragDoll->m_bodies[bulletJoint]->setDeactivationTime( 0.f );
}

void PuppetMaster::pickupBulletJointFromFloor(int bulletJoint)
{
	_ragdollController->ragDoll->m_bodies[bulletJoint]->setActivationState(DISABLE_DEACTIVATION);
	_ragdollController->m_dynamicsWorld->addConstraint(_ragdollController->ragDoll->m_constraints[bulletJoint]);
}

void PuppetMaster::shutdown()
{
	console() << "quitting..." << std::endl;
	if (isFullScreen()) {
		setFullScreen(false);
	}
	delete mLight0;
	ni->shutdown();
}

void PuppetMaster::resize(ResizeEvent event)
{

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

void PuppetMaster::keyDown(KeyEvent event)
{
}

void PuppetMaster::keyUp(KeyEvent event)
{
	switch(event.getChar()) {
		case KeyEvent::KEY_q: quit(); break;
		case KeyEvent::KEY_f: setFullScreen(!isFullScreen()); break;
		case KeyEvent::KEY_c: _seeThroughLightCamera = !_seeThroughLightCamera; break;
	}
}

void PuppetMaster::initShadowMap()
{
	mDepthFbo = gl::Fbo( SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, false, false, true );
	mDepthFbo.bindDepthTexture();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void PuppetMaster::updateShadowMap()
{
	mDepthFbo.bindFramebuffer();

	glPolygonOffset( 1.0f, 1.0f );
	glEnable( GL_POLYGON_OFFSET_FILL );
	glClear( GL_DEPTH_BUFFER_BIT );

	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );

	gl::pushMatrices();
		mLight0->update(mCam);
		mLight0->setShadowRenderMatrices();
		render();
	gl::popMatrices();

	glPopAttrib();

	glDisable( GL_POLYGON_OFFSET_FILL );

	mDepthFbo.unbindFramebuffer();
}

CINDER_APP_BASIC( PuppetMaster, RendererGl )
