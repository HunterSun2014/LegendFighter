#include "pch.h"
#include "Bullet3.h"
using namespace std;

BulletPhysicsEngine::~BulletPhysicsEngine()
{
	//remove the rigidbodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
}

bool BulletPhysicsEngine::Initialize()
{
	///collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = make_shared<btCollisionDispatcher>(m_collisionConfiguration.get());

	m_broadphase = make_shared<btDbvtBroadphase>();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = make_shared<btSequentialImpulseConstraintSolver>();

	m_dynamicsWorld = make_shared<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get());

	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));

	return true;
}

btRigidBody* BulletPhysicsEngine::CreateRigidBody(float mass, const btTransform& startTransform, shared_ptr<btCollisionShape> shape, const btVector4& color) 
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);
	//
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	//
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape.get(), localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

	body->setUserIndex(-1);

	m_dynamicsWorld->addRigidBody(body);

	m_collisionShapes.push_back(shape);

	return body;
};