#pragma once
#include <glm/glm.hpp>

#include "AComponent.hpp"
#include "PhysicsModule.h"
#include "Transform.hpp"

class RigidbodyComponent : public AComponent
{
public:
	RigidbodyComponent(GameObject* go, PhysicsModule* physicsWorld, btRigidBody* rb);
	~RigidbodyComponent();

	// Sets the Transform's field.
	void OnCreate() override;

	// Updates the Transform's data with the state of the physics simulation.
	void OnUpdate(float deltaTime) override;

	// Returns the linear velocity of the rigidbody.
	glm::vec3 GetLinearVelocity();

	// Sets a new position for the rigidbody and ignores gravity.
	void SetPosition(glm::vec3 position);

	// The rigidbody associated to the gameobject.
	btRigidBody* rb;

	// The collider which shapes the rigidbody.
	btCollisionShape* collider;

protected:
	// The physics world this component belongs to.
	PhysicsModule* physicsWorld;

	// The Transform object associated to the gameobject.
	Transform* goTransform;
};

