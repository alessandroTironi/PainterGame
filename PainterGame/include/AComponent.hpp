#pragma once

#include "glm/glm.hpp"

#define PAINT_BALL_COMPONENT 0
#define PAINTABLE_COMPONENT 1
#define RIGIDBODY_COMPONENT 2
#define SELFMOVING_COMPONENT 3

class GameObject;

class AComponent
{
public:
	AComponent();
	AComponent(GameObject* go, unsigned int id);

	// Retrieves the component's ID.
	unsigned int GetID();

	// Invoked when created.
	virtual void OnCreate();

	// Invoked at each frame.
	virtual void OnUpdate(float deltaTime);

	// Invoked every time a collision is detected.
	virtual void OnCollision(GameObject *other, glm::vec3 hitPoint);

	// Returns the gameobject that owns the component.
	GameObject* GetGameObject();

	// Sets the component's parent.
	void SetGameObject(GameObject* gameObject);

	// The next component attached to the gameobject.
	AComponent* nextComponent;

protected:
	// The unique ID of the component.
	unsigned int id;

	// The gameobject this component is attached to.
	GameObject* gameObject = NULL;
};