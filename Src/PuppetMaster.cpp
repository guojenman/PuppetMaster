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

#define DEBUG_DRAW_BULLET 1

class PuppetMaster : public AppBasic {
public:
	void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void setupCamera();

	void		mouseDown( ci::app::MouseEvent event );
	void		mouseMove( ci::app::MouseEvent event );
	void		mouseDrag( ci::app::MouseEvent event );
	void		mouseUp( ci::app::MouseEvent event );
	void 		transformBulletJointsWithNIJoint(XnSkeletonJoint niJoint, int bulletJoint, float yOffset, float zOffset);
	void		dropBulletJointToFloor(int bulletJoint);

	void update();
	void draw();
	void shutdown();

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);

	WuCinderNITE* ni;
	ci::MayaCamUI		_mayaCam;
	Vec2f				_mousePosition;
	bool 				_mouseIsDown;

	bool				_stepPhysics;


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
	_stepPhysics = false;
	_ragdollController = new RagDollController();
	_ragdollController->initPhysics();
#if DEBUG_DRAW_BULLET
	_bulletDebugDraw.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	_ragdollController->m_dynamicsWorld->setDebugDrawer(&_bulletDebugDraw);
	_stepPhysics = true;
#endif


	ni = WuCinderNITE::getInstance();
	ni->setup("Resources/SkeletonRec.oni");
	ni->startUpdating();

}

void PuppetMaster::setupCamera()
{
	// Camera perspective properties
	// Set mayacamera
	//_mayaCam.setCurrentCam( cam );
}


void PuppetMaster::update()
{
	if(_stepPhysics) {
		_ragdollController->clientMoveAndDisplay( 16.0 );
	}
	mCam.setPerspective( 60, getWindowAspectRatio(), 0.1f, ni->maxDepth);
	mCam.lookAt(ci::Vec3f(0, 2, -3.0f), ci::Vec3f(0, 2, 3));
//	btHingeConstraint* hinge = _ragdollController->ragDoll->m_joints[RagDoll::BODYPART_HEAD];

//	btPoint2PointConstraint* constraint = _ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD];
//	btVector3 pivot = constraint->getPivotInB();
//	pivot.setY(pivot.getY() + 0.005);
//	constraint->setPivotB(pivot);
}

void PuppetMaster::draw()
{
	gl::clear(ColorA(0, 0, 0, 0), true);

	gl::pushMatrices();
	gl::setMatrices( mCam );


	if (ni->mUserGen.GetSkeletonCap().IsTracking(1)) {

		float yOffset = 2000.0f;
		float niToBulletScale = 0.001f;
		btVector3 genPivot;

		XnSkeletonJointPosition joint;
		ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1, XN_SKEL_HEAD, joint);
		btVector3 headPivot = btVector3(joint.position.X, joint.position.Y + yOffset, 0) * niToBulletScale;
		_ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD]->setPivotB(headPivot);
		float zOffset = joint.position.Z;

//		transformBulletJointsWithNIJoint(XN_SKEL_LEFT_ELBOW, RagDoll::BODYPART_LEFT_UPPER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_LEFT_HAND, RagDoll::BODYPART_LEFT_LOWER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_LEFT_KNEE, RagDoll::BODYPART_LEFT_UPPER_LEG, yOffset, zOffset);

//		transformBulletJointsWithNIJoint(XN_SKEL_RIGHT_ELBOW, RagDoll::BODYPART_RIGHT_UPPER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_RIGHT_HAND, RagDoll::BODYPART_RIGHT_LOWER_ARM, yOffset, zOffset);
		transformBulletJointsWithNIJoint(XN_SKEL_RIGHT_KNEE, RagDoll::BODYPART_RIGHT_UPPER_LEG, yOffset, zOffset);

	} else {
		dropBulletJointToFloor(RagDoll::BODYPART_HEAD);
//		dropBulletJointToFloor(RagDoll::BODYPART_LEFT_UPPER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_LEFT_LOWER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_LEFT_UPPER_LEG);
//		dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_UPPER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_LOWER_ARM);
		dropBulletJointToFloor(RagDoll::BODYPART_RIGHT_UPPER_LEG);

	}

#if DEBUG_DRAW_BULLET
	gl::pushModelView();
	_ragdollController->m_dynamicsWorld->debugDrawWorld();
	gl::popModelView();
	gl::popMatrices();
	return;
#endif
	// Draw each rigid body in the doll
	for (int i = 0; i < RagDoll::BODYPART_COUNT; ++i)
	{
		btRigidBody* body = _ragdollController->ragDoll->m_bodies[i];

		btTransform trans;
		body->getMotionState()->getWorldTransform( trans );

		float mSize = 0.1;
		ci::Vec3f pos = ci::Vec3f( trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ() );
		ci::Quatf rotation = ci::Quatf( trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ(), trans.getRotation().getW() );

		btCapsuleShape* shape = dynamic_cast<btCapsuleShape*> (_ragdollController->ragDoll->m_shapes[i]);

		//new btCapsuleShape(btScalar(0.15), btScalar(0.20));
//		new btCapsuleShape(btScalar(0.15), btScalar(0.20));

		float r = shape->getRadius();
		float h = shape->getHalfHeight();

		//ci::Vec3f* m_sizes[BODYPART_COUNT]
		ci::gl::pushModelView();
			ci::gl::translate( pos );
			ci::gl::rotate( rotation );
			ci::gl::drawCube( ci::Vec3f::zero(), ci::Vec3f(shape->getRadius(), shape->getHalfHeight(), shape->getRadius()) * 2 );
		ci::gl::popModelView();
	}
	gl::popMatrices();
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
	if (event.getChar() == KeyEvent::KEY_s) {
		_stepPhysics = true;
	}
}

void PuppetMaster::keyUp(KeyEvent event)
{
	switch(event.getChar()) {
		case KeyEvent::KEY_q: quit(); break;
		case KeyEvent::KEY_f: setFullScreen(!isFullScreen()); break;
		case KeyEvent::KEY_s: _stepPhysics = false; break;
	}
}

CINDER_APP_BASIC( PuppetMaster, RendererGl )
