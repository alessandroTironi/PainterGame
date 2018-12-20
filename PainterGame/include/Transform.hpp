#pragma once

#include <vector>

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

#include "btBulletDynamicsCommon.h"

class Transform
{
private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::vector<Transform> children;
	Transform *parent = nullptr;

	glm::mat4 rotateEuler(glm::mat4 mat, glm::vec3 eulerAngles);

	// The attached RB, if any.
	btRigidBody* rb = NULL;
public:
	Transform();
	Transform(const glm::vec3, const glm::vec3, const glm::vec3, Transform*);

	glm::vec3 GetAbsolutePosition();
	glm::vec3 GetLocalPosition();
	void SetLocalPosition(const glm::vec3 newPosition);
	void SetAbsolutePosition(const glm::vec3 newPosition);

	glm::vec3 GetAbsoluteRotation();
	glm::fquat GetAbsoluteRotationQuaternion();
	glm::vec3 GetLocalRotation();
	void SetLocalRotation(const glm::vec3 newRotation);
	void SetAbsoluteRotation(const glm::vec3 newRotation);

	glm::vec3 GetAbsoluteScale();
	glm::vec3 GetLocalScale();
	void SetLocalScale(const glm::vec3 newScale);

	glm::mat4 GetTransformMatrix();

	void SetRigidbody(btRigidBody* rb);
};

glm::fquat euler2quat(glm::vec3 euler);

glm::vec3 quat2euler(glm::fquat q);