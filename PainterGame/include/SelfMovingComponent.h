#pragma once
#include "AComponent.hpp"
#include "RigidbodyComponent.h"

class SelfMovingComponent : public AComponent
{
public:
	SelfMovingComponent(GameObject* gameObject, glm::vec3 displacement, float speed);
	~SelfMovingComponent();

	void OnUpdate(float deltaTime) override;

private:

	glm::vec3 beginPosition, endPosition, currentDestination;

	float speed, t;

	RigidbodyComponent* rb;
};

