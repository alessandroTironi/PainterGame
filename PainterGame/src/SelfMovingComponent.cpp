#include "SelfMovingComponent.h"
#include "GameObject.hpp"


SelfMovingComponent::SelfMovingComponent(GameObject* gameObject, 
	glm::vec3 displacement, float speed) : AComponent(gameObject, SELFMOVING_COMPONENT)
{
	rb = static_cast<RigidbodyComponent*>(gameObject->GetComponent(RIGIDBODY_COMPONENT));
	beginPosition = gameObject->GetTransform()->GetAbsolutePosition();
	endPosition = beginPosition + displacement;
	currentDestination = endPosition;
	this->speed = speed;
	t = 0;
}


SelfMovingComponent::~SelfMovingComponent()
{
}

void SelfMovingComponent::OnUpdate(float deltaTime)
{
	glm::vec3 currentPosition = gameObject->GetTransform()->GetAbsolutePosition();
	t += deltaTime * speed;
	if (t > 1)
		t = 1;

	glm::vec3 newPosition;
	if (currentDestination == endPosition)
	{
		newPosition = glm::mix(beginPosition, endPosition, t);
		if (t >= 1)
		{
			currentDestination = beginPosition;
			t = 0.0f;
		}
	}
	else
	{
		newPosition = glm::mix(endPosition, beginPosition, t);
		if (t >= 1)
		{
			currentDestination = endPosition;
			t = 0.0f;
		}
	}
	rb->SetPosition(newPosition);
	gameObject->GetTransform()->SetAbsolutePosition(newPosition);
}
