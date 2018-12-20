#include "PaintBallComponent.hpp"
#include "PaintableComponent.h"
#include "RigidbodyComponent.h"
#include "GameObject.hpp"
#include <iostream>

PaintBallComponent::PaintBallComponent(GameObject* gameObject, PhysicsModule* physicsModule) 
	: AComponent(gameObject, PAINT_BALL_COMPONENT)
{
	this->physicsModule = physicsModule;
}

void PaintBallComponent::OnCreate()
{
	previousPosition = gameObject->GetTransform()->GetAbsolutePosition();
}

void PaintBallComponent::OnUpdate(float deltaTime)
{
	RigidbodyComponent *rb = 
		static_cast<RigidbodyComponent*>(gameObject->GetComponent(RIGIDBODY_COMPONENT));
	direction = rb->GetLinearVelocity();
	direction = glm::normalize(direction);
}

glm::vec3 PaintBallComponent::GetDirection() { return direction; }

void PaintBallComponent::OnCollision(GameObject* other, glm::vec3 hitPoint)
{
	if (!exploded)
	{
		btSphereShape* sphereShape = new btSphereShape((btScalar)1.0f);
		btCollisionObject* trigger = new btCollisionObject();
		trigger->setCollisionShape(sphereShape);
		glm::vec3 paintBallPos = gameObject->GetTransform()->GetAbsolutePosition();
		trigger->getWorldTransform().setOrigin(btVector3((btScalar)paintBallPos.x, 
			(btScalar)paintBallPos.y, (btScalar)paintBallPos.z));
		//physicsModule->collisionWorld->addCollisionObject(trigger);

		RigidbodyComponent* rbComponent = 
			static_cast<RigidbodyComponent*>(gameObject->GetComponent(RIGIDBODY_COMPONENT));
		rbComponent->rb->getCollisionShape()->setLocalScaling(btVector3(1.5f, 1.5f, 1.5f));

		// View matrix is created by locating the point of view one unit behind the paint
		// ball, on the same vector that connects the paint ball and the hit point.	
		glm::vec3 localUp = glm::cross(localRight, direction);
		glm::mat4 paintViewMatrix = glm::lookAt(paintBallPos - direction * 2.0f,
			paintBallPos + direction, localUp);

		// Near and far plane: paint explodes with a radius of 1 and does not affect 
		// triangles more distant than 3 units.
		GLfloat nearPlane = 0.05f, farPlane = 3.0f;
		// Makes projection matrix.
		GLfloat frustumSize = 1.f;
		glm::mat4 paintProjectionMatrix = glm::ortho(-frustumSize, frustumSize, -frustumSize,
			frustumSize, nearPlane, farPlane);

		// Computes paint transform matrix.
		glm::mat4 paintSpaceMatrix = paintProjectionMatrix * paintViewMatrix;

		std::vector<GameObject*> collisions = physicsModule->GetGameObjectsCollidingWith(gameObject);
		for (int i = 0; i < collisions.size(); i++)
		{
			PaintableComponent* paintableComponent =
				static_cast<PaintableComponent*>(collisions[i]->GetComponent(PAINTABLE_COMPONENT));
			if (paintableComponent)
				paintableComponent->RenderPaintMap(paintSpaceMatrix, direction);
		}

		gameObject->Destroy();
		//physicsModule->collisionWorld->removeCollisionObject(trigger);

		exploded = true;
	}
	
}

glm::vec3 PaintBallComponent::GetLocalRight() { return this->localRight; }
void PaintBallComponent::SetLocalRight(glm::vec3 localRight) { this->localRight = localRight; }

