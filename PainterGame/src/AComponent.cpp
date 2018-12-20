#include "AComponent.hpp"
#include "GameObject.hpp"
#include <iostream>

AComponent::AComponent()
{
	this->id = -1;
}

AComponent::AComponent(GameObject* go, unsigned int id)
{
	this->gameObject = go;
	this->id = id;
}


void AComponent::OnCreate()
{
	// do nothing.
}

void AComponent::OnUpdate(float deltaTime)
{
	// do nothing.
}

void AComponent::OnCollision(GameObject* other, glm::vec3 hitPoint)
{
	// do nothing.
}

GameObject* AComponent::GetGameObject() { return gameObject; }

void AComponent::SetGameObject(GameObject* gameObject)
{
	this->gameObject = gameObject;
}

unsigned int AComponent::GetID() { return id; }