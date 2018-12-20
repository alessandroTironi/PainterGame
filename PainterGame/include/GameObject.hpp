#pragma once

#include "Transform.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "btBulletDynamicsCommon.h"
#include "AComponent.hpp"

class RenderingEngine;

class GameObject
{
public:
	GameObject(unsigned long id, const std::string& name, Transform* transform, Model* model, 
		Material* material);
	GameObject(unsigned long id, const std::string& name, glm::vec3 position, glm::vec3 rotation, 
		glm::vec3 scale, Model* model, Material* material);
	~GameObject();

	bool operator == (const GameObject& go) const { return this->goId == go.goId; }

	const std::string GetName();
	Transform* GetTransform();
	Model* GetModel();
	Material* GetMaterial();

	// Sets the engine that renders the gameobject.
	void SetEngine(RenderingEngine* engine);

	// Retrives the first component found with the provided ID.
	AComponent* GetComponent(unsigned int componentId);

	// Adds a new component to the gameobject.
	void AddComponent(AComponent* newComponent);

	// Updates all the components.
	void UpdateComponents(float deltaTime);

	// Informs all the attached components of the new collision.
	void EnterCollision(GameObject* other, glm::vec3 hitPoint);

	// Destroys the gameobject and removes it from the list of renderable objects.
	void Destroy();

	// True if the gameobject will be destroyed at the next frame.
	bool IsBeingDestroyed();

	// The linked list of the attached components.
	AComponent* firstComponent;

protected:
	// The name assigned to the gameobject.
	std::string name;

	// The rendering engine used to render the GameObject.
	RenderingEngine* engine;

	// True if this object has been scheduled to destruction.
	bool bIsBeingDestroyed = false;

	// Describes the object's position in the world.
	Transform * transform;

	// The model rendered for this object.
	Model* model = nullptr;

	// The material used to render the model.
	Material* material;

private:
	// The private and unique integer that identifies a gameobject univoquely.
	unsigned long goId;
};

