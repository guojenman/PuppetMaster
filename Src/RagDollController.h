#include "LinearMath/btAlignedObjectArray.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;


class RagDollController
{	///this is the most important class
	btDynamicsWorld*		m_dynamicsWorld;

	btAlignedObjectArray<class RagDoll*> m_ragdolls;

	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

	btBroadphaseInterface*	m_broadphase;

	btCollisionDispatcher*	m_dispatcher;

	btConstraintSolver*	m_solver;

	btDefaultCollisionConfiguration* m_collisionConfiguration;

public:
	void initPhysics();

	void exitPhysics();

	virtual ~RagDollController()
	{
		exitPhysics();
	}

	void spawnRagdoll(const btVector3& startOffset);

	virtual void clientMoveAndDisplay();

	virtual void displayCallback();

	virtual void keyboardCallback(unsigned char key, int x, int y);

//	static DemoApplication* Create()
//	{
//		RagDollController* demo = new RagDollController();
//		demo->myinit();
//		demo->initPhysics();
//		return demo;
//	}

};

