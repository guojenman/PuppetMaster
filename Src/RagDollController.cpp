

#define CONSTRAINT_DEBUG_SIZE 0.2f

#include "RagDollController.h"
#include "btBulletDynamicsCommon.h"
//#include "GlutStuff.h"
//#include "GL_ShapeDrawer.h"

#include "LinearMath/btIDebugDraw.h"

//#include "GLDebugDrawer.h"
//#include "RagDollController.h"


// Enrico: Shouldn't these three variables be real constants and not defines?

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616
#endif

void RagDollController::initPhysics()
{
	// Setup the basic world

//	setTexturing(true);
//	setShadows(true);

//	setCameraDistance(btScalar(5.));

	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	m_broadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);

	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	//m_dynamicsWorld->getDispatchInfo().m_useConvexConservativeDistanceUtil = true;
	//m_dynamicsWorld->getDispatchInfo().m_convexConservativeDistanceThreshold = 0.01f;



	// Setup a big ground box
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(200.),btScalar(10.),btScalar(200.)));
		m_collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,-10,0));

#define CREATE_GROUND_COLLISION_OBJECT 1
#ifdef CREATE_GROUND_COLLISION_OBJECT
		btCollisionObject* fixedGround = new btCollisionObject();
		fixedGround->setCollisionShape(groundShape);
		fixedGround->setWorldTransform(groundTransform);
		m_dynamicsWorld->addCollisionObject(fixedGround);
#else
		localCreateRigidBody(btScalar(0.),groundTransform,groundShape);
#endif //CREATE_GROUND_COLLISION_OBJECT

	}

	// Spawn one ragdoll
	btVector3 startOffset(1,0.5,0);
	spawnRagdoll(startOffset);
//	startOffset.setValue(-1,0.5,0);
//	spawnRagdoll(startOffset);

//	clientResetScene();
}

void RagDollController::spawnRagdoll(const btVector3& startOffset)
{
	ragDoll = new RagDollA(m_dynamicsWorld, startOffset);
}

void RagDollController::clientMoveAndDisplay( double delta )
{
	//simple dynamics world doesn't handle fixed-time-stepping
	float ms = 16;
//
	float minFPS = 1000000.f/60.f;
	if (ms > minFPS)
		ms = minFPS;

	if (m_dynamicsWorld) {
		m_dynamicsWorld->stepSimulation(ms / 1000000.f);

		//optional but useful: debug drawing
		m_dynamicsWorld->debugDrawWorld();
	}
//
//	renderme();
//
//	glFlush();
//
//	glutSwapBuffers();
}

void RagDollController::displayCallback()
{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	renderme();
//
//	//optional but useful: debug drawing
//	if (m_dynamicsWorld)
//		m_dynamicsWorld->debugDrawWorld();
//
//	glFlush();
//	glutSwapBuffers();
}

void RagDollController::keyboardCallback(unsigned char key, int x, int y)
{
//	switch (key)
//	{
//	case 'e':
//		{
//		btVector3 startOffset(0,2,0);
//		spawnRagdoll(startOffset);
//		break;
//		}
//	default:
//		DemoApplication::keyboardCallback(key, x, y);
//	}


}



void	RagDollController::exitPhysics()
{

	int i;


		delete ragDoll;

	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them

	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	//delete dynamics world
	delete m_dynamicsWorld;

	//delete solver
	delete m_solver;

	//delete broadphase
	delete m_broadphase;

	//delete dispatcher
	delete m_dispatcher;

	delete m_collisionConfiguration;


}


