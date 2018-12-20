#include "RigidbodyComponent.h"
#include "GameObject.hpp"

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


RigidbodyComponent::RigidbodyComponent(GameObject* go, PhysicsModule* physicsWorld, 
	btRigidBody* rb) : AComponent(go, RIGIDBODY_COMPONENT)
{
	this->physicsWorld = physicsWorld;
	this->rb = rb;
	this->collider = this->rb->getCollisionShape();
	if (go != NULL)
		this->goTransform = go->GetTransform();
}

RigidbodyComponent::~RigidbodyComponent()
{
	physicsWorld->removeRigidBody(rb);
	physicsWorld->collisionWorld->removeCollisionObject(rb);
	delete collider;
	delete rb;
}

void RigidbodyComponent::OnCreate()
{
	Transform* tr = gameObject->GetTransform();
	tr->SetRigidbody(rb);
	rb->getCollisionShape()->setUserPointer(gameObject);
}

void RigidbodyComponent::OnUpdate(float deltaTime)
{
	btTransform rbTransform;
	rb->getMotionState()->getWorldTransform(rbTransform);
	GLfloat modelMat[16];
	rbTransform.getOpenGLMatrix(modelMat);

	glm::mat4 modelMatrix = glm::make_mat4(modelMat)* glm::scale(modelMatrix, goTransform->GetAbsoluteScale());
	btVector3 rbCoM = rb->getCenterOfMassTransform().getOrigin();
	goTransform->SetAbsolutePosition(glm::vec3(rbCoM.getX(), rbCoM.getY(), rbCoM.getZ()));
	btQuaternion btQuat = rbTransform.getRotation();
	glm::vec3 newRotation = quat2euler(glm::fquat(btQuat.getW(), btQuat.getX(), btQuat.getY(), btQuat.getZ()));
	//goTransform->SetAbsoluteRotation(newRotation);
}

glm::vec3 RigidbodyComponent::GetLinearVelocity()
{
	btVector3 linVel = this->rb->getLinearVelocity();
	return glm::vec3(linVel.getX(), linVel.getY(), linVel.getZ());
}

void RigidbodyComponent::SetPosition(glm::vec3 position)
{
	btTransform currentTr; 
	rb->getMotionState()->getWorldTransform(currentTr);
	btTransform newTr;
	newTr.setOrigin(btVector3(position.x, position.y, position.z));
	newTr.setRotation(rb->getWorldTransform().getRotation());
	rb->getMotionState()->setWorldTransform(newTr);
}