#pragma once

//Header files fo Bullet Physica Engine
#include "..\Bullet3\BulletCollision\CollisionDispatch\btDefaultCollisionConfiguration.h"
#include "..\Bullet3\BulletCollision\CollisionDispatch\btCollisionDispatcher.h"
#include "..\Bullet3\BulletCollision\BroadphaseCollision\btDbvtBroadphase.h"
#include "..\Bullet3\BulletDynamics\ConstraintSolver\btConstraintSolver.h"
#include "..\Bullet3\BulletDynamics\ConstraintSolver\btSequentialImpulseConstraintSolver.h"
#include "..\Bullet3\BulletDynamics\Dynamics\btDiscreteDynamicsWorld.h"
#include "BulletCollision\CollisionShapes\btBoxShape.h"
#include "BulletCollision\CollisionShapes\btSphereShape.h"
#include "LinearMath\btDefaultMotionState.h"

#include <vector>
#include <algorithm>
#include <memory>

class BulletPhysicsEngine
{
public:
	virtual ~BulletPhysicsEngine();

	virtual bool Initialize();

	btRigidBody* CreateRigidBody(float mass, const btTransform& startTransform, std::shared_ptr<btCollisionShape> shape, const btVector4& color = btVector4(1, 0, 0, 1));

protected:
	//Bullet Physics Engine member
	std::shared_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> m_dispatcher;
	std::shared_ptr<btDbvtBroadphase> m_broadphase;
	std::shared_ptr<btConstraintSolver> m_solver;
	std::shared_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

	btAlignedObjectArray<std::shared_ptr<btCollisionShape>>	m_collisionShapes;
};