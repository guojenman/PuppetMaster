/*
 * RagDoll.h
 *
 *  Created on: Aug 6, 2011
 *      Author: mariogonzalez
 */

#ifndef RagDoll_H_
#define RagDoll_H_

#include "LinearMath/btAlignedObjectArray.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "RagDoll.h"
#include "cinder/Vector.h"

class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

class RagDoll {
public:
	enum
		{
			BODYPART_PELVIS = 0,
			BODYPART_SPINE,
			BODYPART_HEAD,

			BODYPART_LEFT_UPPER_LEG,
			BODYPART_LEFT_LOWER_LEG,

			BODYPART_RIGHT_UPPER_LEG,
			BODYPART_RIGHT_LOWER_LEG,

			BODYPART_LEFT_UPPER_ARM,
			BODYPART_LEFT_LOWER_ARM,

			BODYPART_RIGHT_UPPER_ARM,
			BODYPART_RIGHT_LOWER_ARM,

			BODYPART_COUNT
		};

		enum
		{
			JOINT_PELVIS_SPINE = 0,
			JOINT_SPINE_HEAD,

			JOINT_LEFT_HIP,
			JOINT_LEFT_KNEE,

			JOINT_RIGHT_HIP,
			JOINT_RIGHT_KNEE,

			JOINT_LEFT_SHOULDER,
			JOINT_LEFT_ELBOW,

			JOINT_RIGHT_SHOULDER,
			JOINT_RIGHT_ELBOW,

			JOINT_COUNT
		};


	RagDoll();
	RagDoll(btDynamicsWorld* ownerWorld, const btVector3& positionOffset);
	virtual ~RagDoll();

	btDynamicsWorld* m_ownerWorld;
	btCollisionShape* m_shapes[BODYPART_COUNT];
	btRigidBody* m_bodies[BODYPART_COUNT];
	ci::Vec3f* m_sizes[BODYPART_COUNT];
	btTypedConstraint* m_joints[JOINT_COUNT];

private:
	btRigidBody* localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape);
};

#endif /* RagDoll_H_ */
