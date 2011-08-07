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
#include "audio/AudioNode.h"
#include "particle/ParticleController.h"

#include <btBulletDynamicsCommon.h>
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define DEBUG_DRAW_BULLET 0
#define DEBUG_USE_NITE 1

extern int counter;

class PuppetMaster : public AppBasic {
public:
	void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void setupCamera();
	void setupAudioNodes();
	void setupParticleController();


	void		mouseDown( ci::app::MouseEvent event );
	void		mouseMove( ci::app::MouseEvent event );
	void		mouseDrag( ci::app::MouseEvent event );
	void		mouseUp( ci::app::MouseEvent event );
	void 		transformBulletJointsWithNIJoint(XnSkeletonJoint niJoint, int bulletJoint, float yOffset, float zOffset);
	void		dropBulletJointToFloor(int bulletJoint);
	void		drawFloorPlane( float floorSize );

	void update();
	void updateJoints();
	void updateAudioNodes();
	void updateParticleController();
	void draw();
	void shutdown();

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);

	WuCinderNITE* ni;
	ci::MayaCamUI		_mayaCam;
	Vec2f				_mousePosition;
	bool 				_mouseIsDown;

	// AudioNodes
	float scanlinePosition;
	float scanLineSpeed;
	float lastScanlinePosition;
	std::vector<AudioNode*> _audioNodes;

	// ParticleController
	ParticleController* _particleController;

	CameraPersp mCam;
	RagDollController *_ragdollController;
	GLDebugDrawer _bulletDebugDraw;
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
#if DEBUG_DRAW_BULLET
	_bulletDebugDraw.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	_ragdollController->m_dynamicsWorld->setDebugDrawer(&_bulletDebugDraw);
#endif


#if DEBUG_USE_NITE
	ni = WuCinderNITE::getInstance();
	ni->setup("Resources/SkeletonRec.oni");
	ni->startUpdating();
#endif

	setupAudioNodes();
	setupParticleController();

}

void PuppetMaster::setupCamera()
{
	// Camera perspective properties
	// Set mayacamera
	//_mayaCam.setCurrentCam( cam );
}


void PuppetMaster::setupAudioNodes() {
	std::string base_path = getAppPath() + "/Contents/Resources/";

	scanlinePosition = 0;
	lastScanlinePosition = 0;
	scanLineSpeed = 0.025;

	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_LEFT_LOWER_ARM,	base_path + "piano_B.mp3"));
	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_RIGHT_LOWER_ARM, base_path + "piano_D.mp3"));

	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_LEFT_LOWER_LEG, base_path + "piano_F_sharp.mp3"));
	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_RIGHT_LOWER_LEG, base_path + "piano_middle_C.mp3"));
	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_HEAD, base_path + "piano_G_sharp.mp3"));
}

void PuppetMaster::setupParticleController() {
	_particleController = new ParticleController();

	// Create N emitters
	size_t len = _audioNodes.size();
	for (int i = 0; i < len; ++i) {
		_particleController->createEmitterWithJointID( _audioNodes[i]->jointID );
	}
}


void PuppetMaster::update()
{
	_ragdollController->clientMoveAndDisplay( 16.0 );
	updateAudioNodes();
	updateJoints();



#if DEBUG_USE_NITE
	mCam.setPerspective( 60, getWindowAspectRatio(), 0.1f, ni->maxDepth);
#else
	mCam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 100000);
#endif
	mCam.lookAt(ci::Vec3f(0, 2, -3.0f), ci::Vec3f(0, 2, 3));

//	btHingeConstraint* hinge = _ragdollController->ragDoll->m_joints[RagDoll::BODYPART_HEAD];

//	btPoint2PointConstraint* constraint = _ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD];
//	btVector3 pivot = constraint->getPivotInB();
//	pivot.setY(pivot.getY() + 0.005);
//	constraint->setPivotB(pivot);
}


void PuppetMaster::updateJoints() {

#if DEBUG_USE_NITE
	if (ni->mUserGen.GetSkeletonCap().IsTracking(1)) {
		float yOffset = 2000.0f;
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

	} else
#endif
	{
		dropBulletJointToFloor(RagDoll::BODYPART_HEAD);
		dropBulletJointToFloor(RagDoll::BODYPART_LEFT_LOWER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_LEFT_UPPER_LEG);
		dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_LOWER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_UPPER_LEG);

	}
}

void PuppetMaster::updateAudioNodes() {

#if DEBUG_USE_NITE
	if (!ni->mUserGen.GetSkeletonCap().IsTracking(1))
		return;
#endif

	scanlinePosition += scanLineSpeed;
	if(scanlinePosition > 4)
		scanlinePosition = 0;

	// Play tone if between last and current scanline position
	size_t len = _audioNodes.size();
	for (int i = 0; i < len; ++i) {
		btTransform trans;
		_ragdollController->ragDoll->m_bodies[_audioNodes[i]->jointID]->getMotionState()->getWorldTransform(trans);


		// If it falls within range, emit some particles and play a tone
		float y = trans.getOrigin().getY();
		if( y < scanlinePosition && y > lastScanlinePosition ) {
			Emitter* emitter = _particleController->getEmitterWithJointID( _audioNodes[i]->jointID );
			if(emitter) emitter->addParticles( ci::Rand::randInt( 1, 5) );
			_audioNodes[i]->reset();
		}



		_audioNodes[i]->update();
	}

	lastScanlinePosition = scanlinePosition;
}


void PuppetMaster::updateParticleController() {
//	_particleController->update( _ragdollController->ragDoll );
}

void PuppetMaster::draw()
{
	counter++;
	gl::clear(ColorA(0, 0, 0, 0), true);

	gl::pushMatrices();
	gl::setMatrices( mCam );

//
//
//
	gl::pushModelView();
	_ragdollController->m_dynamicsWorld->debugDrawWorld();
	gl::popModelView();
////
//	// Draw each rigid body in the doll
	for (int i = 0; i < RagDoll::BODYPART_COUNT; ++i)
	{
		btScalar m[16];
		btRigidBody* body = _ragdollController->ragDoll->m_bodies[i];

		btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
		myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);

		btCapsuleShape* shape = dynamic_cast<btCapsuleShape*> (_ragdollController->ragDoll->m_shapes[i]);
		float height = (shape->getHalfHeight()*2) + 2*shape->getMargin();

		gl::pushModelView();
			glMultMatrixf(m);
			gl::drawCube( Vec3f::zero(), Vec3f(shape->getRadius(), height, shape->getRadius()));
		gl::popModelView();
	}
//
////	gl::pushModelView();
////	drawFloorPlane(10);
////	gl::popModelView();
//
	ci::gl::drawLine(ci::Vec3f(-1000.0f, scanlinePosition, 0), ci::Vec3f(1000.0f, scanlinePosition, 0));
	_particleController->update( _ragdollController->ragDoll );
	_particleController->draw();



	gl::popMatrices();

//	_particleController->update( _ragdollController->ragDoll );
//	_particleController->draw();

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
	float floor = 3.5f;
	btVector3 pivot = _ragdollController->ragDoll->m_constraints[bulletJoint]->getPivotInB();
	pivot.setY((floor - pivot.getY()) * .5);
	_ragdollController->ragDoll->m_constraints[bulletJoint]->setPivotB(pivot);
}

void PuppetMaster::shutdown()
{
	console() << "quitting..." << std::endl;
	ni->shutdown();
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
	_particleController->keyDown( event );
}

void PuppetMaster::keyUp(KeyEvent event)
{
	switch(event.getChar()) {
		case KeyEvent::KEY_q: quit(); break;
		case KeyEvent::KEY_f: setFullScreen(!isFullScreen()); break;
	}
}

CINDER_APP_BASIC( PuppetMaster, RendererGl )
