//#include "cinder/app/AppBasic.h"
//#include "cinder/app/MouseEvent.h"
//#include "cinder/Color.h"
//#include "cinder/gl/gl.h"
//#include "cinder/gl/Texture.h"
//#include "cinder/Vector.h"
//#include "cinder/Rand.h"
//#include "cinder/Camera.h"
//#include "RagDollController.h"
//#include "RagDoll.h"
//#include "cinder/Quaternion.h"
//#include "cinder/MayaCamUI.h"
//
//#include "GLDebugDrawer.h"
//#include "WuCinderNITE.h"
//#include <btBulletDynamicsCommon.h>
//#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
//#include "audio/AudioNode.h"
//#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
//
//using namespace ci;
//using namespace ci::app;
//using namespace std;
//
//#define DEBUG_DRAW_BULLET 1
//
//class PuppetMaster: public AppBasic {
//public:
//	void prepareSettings(AppBasic::Settings *settings);
//	void setup();
//	void setupCamera();
//	void setupAudioNodes();
//
//	void mouseDown(ci::app::MouseEvent event);
//	void mouseMove(ci::app::MouseEvent event);
//	void mouseDrag(ci::app::MouseEvent event);
//	void mouseUp(ci::app::MouseEvent event);
//
//	void update();
//	void updateAudioNodes();
//	void draw();
//	void shutdown();
//
//	void keyDown(KeyEvent event);
//	void keyUp(KeyEvent event);
//
//	WuCinderNITE* ni;
//	ci::MayaCamUI _mayaCam;
//	Vec2f _mousePosition;
//	bool _mouseIsDown;
//	bool _stepPhysics;
//
//	float scanlinePosition;
//	float scanLineSpeed;
//	float lastScanlinePosition;
//	std::vector<AudioNode*> _audioNodes;
//
//	CameraPersp mCam;
//	RagDollController *_ragdollController;
//	GLDebugDrawer _bulletDebugDraw;
//
//};
//
//void PuppetMaster::prepareSettings(AppBasic::Settings *settings) {
//	settings->setWindowSize(640, 480);
//}
//
//void PuppetMaster::setup() {
//	XnMapOutputMode mapMode;
//	mapMode.nFPS = 30;
//	mapMode.nXRes = 640;
//	mapMode.nYRes = 480;
//	_stepPhysics = false;
//	_ragdollController = new RagDollController();
//	_ragdollController->initPhysics();
//
//
//#if DEBUG_DRAW_BULLET
//	_bulletDebugDraw.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
//	_ragdollController->m_dynamicsWorld->setDebugDrawer(&_bulletDebugDraw);
//	_stepPhysics = true;
//#endif
//
//	ni = WuCinderNITE::getInstance();
//	ni->setup("Resources/SkeletonRec.oni");
//	ni->startUpdating();
//
//
////		    std::cout << "AppPath: " << this->getAppPath() << std::endl;
//
//
//	setupAudioNodes();
//}
//
//void PuppetMaster::setupCamera() {
//	// Camera perspective properties
//		float cameraFOV			= 60.0f;
//		float cameraNear		= 1.0f;
//		float cameraFar			= FLT_MAX;
//
//		ci::Vec3f p = ci::Vec3f::one() * 2000.0f;// Start off this far away from the center
//		ci::CameraPersp cam = ci::CameraPersp( getWindowWidth(), getWindowHeight(), cameraFOV );
//
//		cam.setWorldUp( ci::Vec3f(0, 1, 0) );
//		cam.setEyePoint( ci::Vec3f(0, 0, 0 ) );
//		cam.setCenterOfInterestPoint( ci::Vec3f::zero() );
//		cam.setPerspective( cameraFOV, getWindowAspectRatio(), cameraNear, cameraFar );
//		cam.setViewDirection( ci::Vec3f(0, 0, 1 ) );
//
//
//		// Set mayacamera
//		_mayaCam.setCurrentCam( cam );
//}
//
//void PuppetMaster::setupAudioNodes() {
//	std::string base_path = getAppPath() + "/Contents/Resources/";
//
//
//	scanlinePosition = 0;
//	lastScanlinePosition = 0;
//	scanLineSpeed = 0.025;
//
//	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_LEFT_LOWER_ARM,	base_path + "piano_B.mp3"));
//	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_RIGHT_LOWER_ARM, base_path + "piano_D.mp3"));
//
//	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_LEFT_LOWER_LEG, base_path + "piano_F_sharp.mp3"));
//	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_RIGHT_LOWER_LEG, base_path + "piano_middle_C.mp3"));
//	_audioNodes.push_back(new AudioNode(RagDoll::BODYPART_HEAD, base_path + "piano_G_sharp.mp3"));
//}
//
//void PuppetMaster::update() {
//	if (_stepPhysics) {
//		_ragdollController->clientMoveAndDisplay(16.0);
//	}
//
//
//	updateAudioNodes();
//	mCam.setPerspective(60, getWindowAspectRatio(), 0.1f, ni->maxDepth);
//	mCam.lookAt(ci::Vec3f(0, 2, -5.0f), ci::Vec3f(0, 0, 3));
//	//	btHingeConstraint* hinge = _ragdollController->ragDoll->m_joints[RagDoll::BODYPART_HEAD];
//
//	//	btPoint2PointConstraint* constraint = _ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD];
//	//	btVector3 pivot = constraint->getPivotInB();
//	//	pivot.setY(pivot.getY() + 0.005);
//	//	constraint->setPivotB(pivot);
//}
//
//void PuppetMaster::updateAudioNodes() {
//	scanlinePosition += scanLineSpeed;
//	if(scanlinePosition > 4.5)
//		scanlinePosition = 0;
//
//	size_t len = _audioNodes.size();
//	for (int i = 0; i < len; ++i) {
//		btTransform trans;
//		_ragdollController->ragDoll->m_bodies[_audioNodes[i]->jointID]->getMotionState()->getWorldTransform(trans);
//
//
//		float y = trans.getOrigin().getY();
//
//		if( y < scanlinePosition && y > lastScanlinePosition )
//			_audioNodes[i]->reset();
//
////		if( i == 0)
////			std::cout << trans.getOrigin().getY() << std::endl;
//
//		_audioNodes[i]->update();
//
//	}
//
//	lastScanlinePosition = scanlinePosition;
//}
//
//void PuppetMaster::draw() {
//	gl::clear(ColorA(0, 0, 0, 0), true);
//
//	gl::pushMatrices();
//	gl::setMatrices(_mayaCam.getCamera());
//
//	gl::pushModelView();
//	ni->renderSkeleton(0.001f);
//	gl::popModelView();
//
//	if (ni->mUserGen.GetSkeletonCap().IsTracking(1)) {
//
//		btVector3 genPivot;
//
//		XnSkeletonJointPosition joint;
//		ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1, XN_SKEL_HEAD,
//				joint);
//		btVector3 headPivot = btVector3(joint.position.X,
//				joint.position.Y + 10.0f, joint.position.Z) * .001f;
//		_ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_HEAD]->setPivotB(
//				headPivot);
//
//		gl::pushModelView();
//		gl::drawCube(
//				Vec3f(headPivot.getX(), headPivot.getY(), headPivot.getZ()),
//				Vec3f(0.5f, 0.5f, 0.5f));
//		gl::popModelView();
//
//		ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1,
//				XN_SKEL_LEFT_HAND, joint);
//		genPivot = btVector3(joint.position.X, joint.position.Y + 5.0f,
//				joint.position.Z) * .001f;
//		_ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_LEFT_LOWER_ARM]->setPivotB(
//				genPivot);
//
//		ni->mUserGen.GetSkeletonCap().GetSkeletonJointPosition(1,
//				XN_SKEL_RIGHT_HAND, joint);
//		genPivot = btVector3(joint.position.X, joint.position.Y + 5.0f,
//				joint.position.Z) * .001f;
//		_ragdollController->ragDoll->m_constraints[RagDoll::BODYPART_RIGHT_LOWER_ARM]->setPivotB(
//				genPivot);
//
//		//		btTransform trans;
//		//		trans.setIdentity();
//		//		_ragdollController->ragDoll->m_bodies[RagDoll::BODYPART_HEAD]->getMotionState()->getWorldTransform(trans);
//		//		_ragdollController->ragDoll->m_bodies[RagDoll::BODYPART_HEAD]->translate(headPivot - trans.getOrigin());
//	}
//
//#if DEBUG_DRAW_BULLET
//	gl::pushModelView();
//	_ragdollController->m_dynamicsWorld->debugDrawWorld();
//	gl::popModelView();
//
//	ci::gl::drawLine(ci::Vec3f(-1000.0f, scanlinePosition, 0), ci::Vec3f(1000.0f, scanlinePosition, 0));
//	gl::popMatrices();
//
//
//	return;
//#endif
//	// Draw each rigid body in the doll
//	for (int i = 0; i < RagDoll::BODYPART_COUNT; ++i) {
//		btRigidBody* body = _ragdollController->ragDoll->m_bodies[i];
//
//		btTransform trans;
//		body->getMotionState()->getWorldTransform(trans);
//
//		float mSize = 0.1;
//		ci::Vec3f pos = ci::Vec3f(trans.getOrigin().getX(),
//				trans.getOrigin().getY(), trans.getOrigin().getZ());
//		ci::Quatf rotation = ci::Quatf(trans.getRotation().getX(),
//				trans.getRotation().getY(), trans.getRotation().getZ(),
//				trans.getRotation().getW());
//
//		btCapsuleShape
//				* shape =
//						dynamic_cast<btCapsuleShape*> (_ragdollController->ragDoll->m_shapes[i]);
//
//		//new btCapsuleShape(btScalar(0.15), btScalar(0.20));
//		//		new btCapsuleShape(btScalar(0.15), btScalar(0.20));
//
//		float r = shape->getRadius();
//		float h = shape->getHalfHeight();
//
//		//ci::Vec3f* m_sizes[BODYPART_COUNT]
//		ci::gl::pushModelView();
//		ci::gl::translate(pos);
//		ci::gl::rotate(rotation);
//		ci::gl::drawCube(
//				ci::Vec3f::zero(),
//				ci::Vec3f(shape->getRadius(), shape->getHalfHeight(),
//						shape->getRadius()) * 2);
//		ci::gl::popModelView();
//	}
//
//	gl::popMatrices();
//}
//
//void PuppetMaster::shutdown() {
//	console() << "quitting..." << std::endl;
//	ni->shutdown();
//}
//
//void PuppetMaster::mouseDown(ci::app::MouseEvent event) {
//	_mouseIsDown = true;
//	_mayaCam.mouseDown(event.getPos());
//}
//
//void PuppetMaster::mouseDrag(ci::app::MouseEvent event) {
//	_mayaCam.mouseDrag(event.getPos(), event.isLeftDown(), event.isMetaDown(),
//			event.isRightDown());
//	_mousePosition = event.getPos();
//}
//
//void PuppetMaster::mouseMove(ci::app::MouseEvent event) {
//	_mayaCam.mouseDrag(event.getPos(), event.isLeftDown(), event.isMetaDown(),
//			event.isRightDown());
//	_mousePosition = event.getPos();
//}
//
//void PuppetMaster::mouseUp(ci::app::MouseEvent event) {
//	_mouseIsDown = false;
//	_mayaCam.mouseDown(event.getPos());
//}
//
//void PuppetMaster::keyDown(KeyEvent event) {
//	if (event.getChar() == KeyEvent::KEY_s) {
//		_stepPhysics = true;
//	}
//}
//
//void PuppetMaster::keyUp(KeyEvent event) {
//	if (event.getChar() == KeyEvent::KEY_q) {
//		quit();
//	}
//	if (event.getChar() == KeyEvent::KEY_s) {
//		_stepPhysics = false;
//	}
//}
//
////CINDER_APP_BASIC( PuppetMaster, RendererGl )
