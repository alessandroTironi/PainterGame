#include "GameObject.hpp"

#include "RenderingEngine.hpp"

GameObject::GameObject(unsigned long id, const std::string& name, Transform* transform, Model* model, 
	Material* material)
{
	this->goId = id;
	this->name = name;
	this->transform = transform;
	this->model = model;
	this->material = material;
	this->firstComponent = NULL;
}

GameObject::GameObject(unsigned long id, const std::string& name, glm::vec3 position, glm::vec3 rotation, 
	glm::vec3 scale, Model* model, Material* material)
{
	this->goId = id;
	this->name = name;
	this->transform = new Transform(position, rotation, scale, NULL);
	this->model = model; 
	this->material = material;
	this->firstComponent = NULL;
}

GameObject::~GameObject()
{
	delete this->transform;
	AComponent* currentComponent = firstComponent;
	AComponent* nextComponent;
	while (currentComponent != NULL)
	{
		nextComponent = currentComponent->nextComponent;
		delete currentComponent;
		currentComponent = nextComponent;
	}
}

const std::string GameObject::GetName() { return this->name; }
Transform* GameObject::GetTransform() { return this->transform; }
Model* GameObject::GetModel() { return this->model; }
Material* GameObject::GetMaterial() { return this->material; }

void GameObject::SetEngine(RenderingEngine* engine)
{
	this->engine = engine;
}

AComponent* GameObject::GetComponent(unsigned int componentId)
{
	for (AComponent* current = firstComponent; current != NULL; current = current->nextComponent)
		if (current->GetID() == componentId)
			return current;
	return NULL;
}

void GameObject::AddComponent(AComponent* newComponent)
{
	if (firstComponent == nullptr)
		firstComponent = newComponent;
	else
	{
		AComponent* current;
		for (current = firstComponent; current->nextComponent != NULL; current = current->nextComponent);
		current->nextComponent = newComponent;
	}
	newComponent->SetGameObject(this);
	newComponent->OnCreate();
}

void GameObject::UpdateComponents(float deltaTime)
{
	for (AComponent* current = firstComponent; current != NULL; current = current->nextComponent)
		current->OnUpdate(deltaTime);
}

void GameObject::EnterCollision(GameObject* other, glm::vec3 hitPoint)
{
	for (AComponent* current = firstComponent; current != NULL; current = current->nextComponent)
		current->OnCollision(other, hitPoint);
}

void GameObject::Destroy()
{
	engine->MarkGameObjectForDestruction(this);
	bIsBeingDestroyed = true;
}

bool GameObject::IsBeingDestroyed() { return bIsBeingDestroyed; }