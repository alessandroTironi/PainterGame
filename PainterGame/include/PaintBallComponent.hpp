#pragma once
#include <glm/glm.hpp>

#include "AComponent.hpp"
#include "PhysicsModule.h"

class PaintBallComponent : public AComponent
{
public:
	PaintBallComponent(GameObject* gameObject, PhysicsModule* physicsModule);


	void OnCreate() override;

	void OnUpdate(float deltaTime) override;

	// Spreads paint on the other collider.
	void OnCollision(GameObject* other, glm::vec3 hitPoint) override;

	// Sets the paintball's local right vector.
	void SetLocalRight(glm::vec3 localRight);

	// Returns the local right vector.
	glm::vec3 GetLocalRight();

	// Returns the direction of the paint ball.
	glm::vec3 GetDirection();

private:
	// The paintball's local right vector.
	glm::vec3 localRight;

	// Describes the paintball's trajectory.
	glm::vec3 direction;

	// The previous position of the paint ball.
	glm::vec3 previousPosition;

	// The spherical collision shape used to spherecast the paintball.
	PhysicsModule* physicsModule;


	bool exploded = false;
};