#pragma once

#include <glm\glm.hpp>

#include <btBulletDynamicsCommon.h>
#include "GameObject.hpp"

class PhysicsModule
{
public:
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btCollisionWorld* collisionWorld;

	double sceneSize = 100;
	unsigned int maxColliders = 500;

	PhysicsModule()
	{
		// Collision detection between rigid bodies (identified as possible collisions 
		// during BroadPhase Collision Detection).
		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		this->collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		this->dispatcher = new btCollisionDispatcher(collisionConfiguration);

		// Sets the overlapping cache, which stores collision detection results.
		btScalar sSceneSize = (btScalar)sceneSize;
		btVector3 worldAabbMin(-sSceneSize, -sSceneSize, -sSceneSize);
		btVector3 worldAabbMax(sSceneSize, sSceneSize, sSceneSize);
		this->overlappingPairCache = new bt32BitAxisSweep3(worldAabbMin, worldAabbMax, maxColliders, 0, true);

		// Sets the numerical integration method for computing position and rotation of rigid bodies
		// from forces, collisions, constraints, etc.
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		this->solver = new btSequentialImpulseConstraintSolver;

		//  DynamicsWorld is the main class of physics simulation.
		this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		// Sets gravity.
		this->dynamicsWorld->setGravity(btVector3(0, -9.82f, 0));

		// Sets collision world.
		this->collisionWorld = new btCollisionWorld(this->dispatcher, this->overlappingPairCache,
			this->collisionConfiguration);
	}

	/* Creates a rigid body of different shapes (see type) */
	btRigidBody* createRigidBody(int type, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float m, float friction, float restitution)
	{

		btCollisionShape* cShape;

		// Position and rotation data need to be converted to Bullet structs.
		btVector3 position = btVector3(pos.x, pos.y, pos.z);
		btQuaternion rotation;
		rotation.setEuler(rot.x, rot.y, rot.z);

		// Box shape
		if (type == 0)
		{
			btVector3 dim = btVector3(size.x, size.y, size.z);
			cShape = new btBoxShape(dim);
		}
		// Sphere shape (we use the first size component as radius).
		else if (type == 1)
			cShape = new btSphereShape(size.x);
		else if (type == 2)
			// Plane shape
			cShape = new btStaticPlaneShape(btVector3(0, 1, 0), pos.y);
		else if (type == 3)
			// Cylinder shape
			cShape = new btCylinderShape(btVector3(size.x, size.y, size.z));

		// Updates collision shapes list.
		this->collisionShapes.push_back(cShape);

		// Sets initial transform information.
		// NOTE: initial position must equal the mesh's position.
		btTransform objTransform;
		objTransform.setIdentity();
		objTransform.setRotation(rotation);
		objTransform.setOrigin(position);

		// Mass = 0 => static object.
		btScalar mass = m;
		bool isDynamic = (mass != 0.0f);

		// If the object is dynamic then compute local intertia.
		btVector3 localInertia(0.0, 0.0, 0.0);
		if (isDynamic)
			cShape->calculateLocalInertia(mass, localInertia);

		// Initializes motion state with the current transform data.      
		btDefaultMotionState* motionState = new btDefaultMotionState(objTransform);

		// Sets the rigid body's data struct.
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, cShape, localInertia);
		rbInfo.m_friction = friction;
		rbInfo.m_restitution = restitution;

		// If the shape is spherical, we need to adjust friction.
		if (type == 1) {
			// In physical simulation, spheres touch the plane in only one point, thus preventing friction sphere-plane
			// from being correctly applied.
			// For this reason, we apply a rolling friction and we also add an angular damping factor to apply a resistance 
			// for making the sphere stop rolling after some time.
			rbInfo.m_angularDamping = 0.3f;
			rbInfo.m_rollingFriction = 0.3f;
		}

		// Create the rigid body and add it to the dynamics world.
		btRigidBody* body = new btRigidBody(rbInfo);
		this->dynamicsWorld->addRigidBody(body);

		return body;
	}

	void removeRigidBody(btRigidBody* toRemove)
	{
		this->dynamicsWorld->removeRigidBody(toRemove);
		this->collisionShapes.remove(toRemove->getCollisionShape());
		this->collisionWorld->removeCollisionObject(toRemove);
	}

	void Clear()
	{
		//remove the rigidbodies from the dynamics world and delete them
		for (int i = this->dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			// Deletes all the motions states and collision objects.
			btCollisionObject* obj = this->dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			this->dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		//delete collision shapes
		for (int j = 0; j<this->collisionShapes.size(); j++)
		{
			btCollisionShape* shape = this->collisionShapes[j];
			this->collisionShapes[j] = 0;
			delete shape;
		}

		//delete dynamics world
		delete this->dynamicsWorld;

		//delete solver
		delete this->solver;

		//delete broadphase
		delete this->overlappingPairCache;

		//delete dispatcher
		delete this->dispatcher;

		delete this->collisionConfiguration;

		this->collisionShapes.clear();
	}

	void PerformCollisionDetection()
	{
		collisionWorld->performDiscreteCollisionDetection();
		int nManifolds = collisionWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < nManifolds; i++)
		{
			btPersistentManifold* contactManifold = collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();
			contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				//std::cout << "Collision between shapes " << obA->getCollisionShape()
				//	<< " and " << obB->getCollisionShape() << std::endl;
				GameObject* goA = static_cast<GameObject*>(obA->getCollisionShape()->getUserPointer());
				GameObject* goB = static_cast<GameObject*>(obB->getCollisionShape()->getUserPointer());
				if (goA != NULL && goB != NULL)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);
					btVector3 ptA, ptB;
					ptA = pt.getPositionWorldOnA();
					ptB = pt.getPositionWorldOnB();
					//std::cout << "Collision between " << goA->GetName() << " and "
					//	<< goB->GetName() << std::endl;
					goA->EnterCollision(goB, glm::vec3(ptA.getX(), ptA.getY(), ptA.getZ()));
					goB->EnterCollision(goA, glm::vec3(ptB.getX(), ptB.getY(), ptB.getZ()));
				}
			}
		}
	}

	// Retrieves a vector of gameobjects colliding with the given collider.
	std::vector<GameObject*> GetGameObjectsCollidingWith(GameObject* collider)
	{
		std::vector<GameObject*> collisions;
		
		int nManifolds = collisionWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < nManifolds; i++)
		{
			btPersistentManifold* contactManifold = collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			GameObject* goA = static_cast<GameObject*>(obA->getCollisionShape()->getUserPointer());
			GameObject* goB = static_cast<GameObject*>(obB->getCollisionShape()->getUserPointer());
			if (goA != NULL && goB != NULL)
				std::cout << "Collision between " << goA->GetName() << " and " << goB->GetName() << std::endl;
			if (goA == collider && goB != NULL && std::find(collisions.begin(), collisions.end(), goB) == collisions.end())
				collisions.push_back(goB);
			if (goB == collider && goA != NULL && std::find(collisions.begin(), collisions.end(), goA) == collisions.end())
				collisions.push_back(goA);
		}

		return collisions;

		/*
		std::vector<GameObject*> collisions;
		btCollisionObjectArray coArray = dynamicsWorld->getCollisionObjectArray();
		int nCollisionObjects = coArray.size();
		for (int i = 0; i < nCollisionObjects; i++)
		{
			btCollisionObject* cObj = coArray.at(i);
			if (collider->checkCollideWith(cObj))
				collisions.push_back(static_cast<GameObject*>(cObj->getCollisionShape()->getUserPointer()));
		}
		return collisions;
		*/
	}
};

