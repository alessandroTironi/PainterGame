#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Transform.hpp"
#include "RigidbodyComponent.h"

#define DEG2RAD 0.0174533f

// Basic constructor with default values.
Transform::Transform()
{
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 0, 0);
	this->scale = glm::vec3(1, 1, 1);
}

// Customizable constructor
Transform::Transform(const glm::vec3 position = glm::vec3(0, 0, 0), 
	const glm::vec3 rotation = glm::vec3(0, 0, 0), 
	const glm::vec3 scale = glm::vec3(1, 1, 1), 
	Transform* parent = nullptr)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->parent = parent;
}

void Transform::SetRigidbody(btRigidBody* rb)
{
	this->rb = rb;
}


// Updates the transformation matrix with the 
glm::mat4 Transform::GetTransformMatrix()
{
	if (rb != NULL)
	{
		btTransform btTr; 
		rb->getMotionState()->getWorldTransform(btTr);
		GLfloat modelMatrix[16];
		btTr.getOpenGLMatrix(modelMatrix);
		glm::mat4 trMat = glm::mat4(1.0f);
		trMat = glm::make_mat4(modelMatrix) * glm::scale(trMat, scale);
		return trMat;
	}
	glm::mat4 translation = glm::translate(glm::mat4(), this->position);
	glm::mat4 rotation = glm::mat4_cast(euler2quat(this->rotation * DEG2RAD));
	glm::mat4 scaling = glm::scale(glm::mat4(), this->scale);
	glm::mat4 trMatrix = translation * rotation * scaling;
	return trMatrix;
}


/// <summary>
/// Rotates a 4x4 matrix by a vector3 of Euler angles.
/// </summary>
/// <param name="mat">The matrix to rotate</param>
/// <param name="eulerAngles">The vector3 of Euler angles that describes the rotation.</param>
inline glm::mat4 Transform::rotateEuler(glm::mat4 mat, glm::vec3 eulerAngles)
{
	glm::mat4 matrix = mat;
	matrix = glm::rotate(matrix, eulerAngles.x, glm::vec3(1, 0, 0));
	matrix = glm::rotate(matrix, eulerAngles.y, glm::vec3(0, 1, 0));
	matrix = glm::rotate(matrix, eulerAngles.z, glm::vec3(0, 0, 1));
	return matrix;
}

// Basic getters
inline glm::vec3 Transform::GetLocalPosition()
{
	return this->position;
}
inline glm::vec3 Transform::GetLocalRotation()
{
	return this->rotation;
}
inline glm::vec3 Transform::GetLocalScale()
{
	return this->scale;
}

// Absolute getters
glm::vec3 Transform::GetAbsolutePosition()
{
	glm::vec3 absPos = this->position;
	for (Transform* p = this->parent; p != nullptr; p = p->parent)
		absPos += p->position;
	return absPos;
}

void Transform::SetAbsolutePosition(glm::vec3 newPosition)
{
	// Computes the new abs position with the displacement from the current abs position.
	glm::vec3 displacement = newPosition - GetAbsolutePosition();
	position += displacement;
}

glm::vec3 Transform::GetAbsoluteRotation()
{
	glm::vec3 absRot = this->rotation;
	for (Transform* p = this->parent; p != nullptr; p = p->parent)
		absRot += p->rotation;
	return absRot;
}

glm::fquat Transform::GetAbsoluteRotationQuaternion()
{
	return euler2quat(GetAbsoluteRotation() * DEG2RAD);
}

void Transform::SetAbsoluteRotation(glm::vec3 newRotation)
{
	glm::vec3 displacement = newRotation - GetAbsoluteRotation();
	rotation += displacement;
}

glm::vec3 Transform::GetAbsoluteScale()
{
	glm::vec3 absScale = { this->scale.x, this->scale.y, this->scale.z };
	for (Transform* p = this->parent; p != nullptr; p = p->parent)
		absScale = { absScale.x * p->scale.x, absScale.y * p->scale.y, absScale.z * p->scale.z };
	return absScale;
}

// Local setters
inline void Transform::SetLocalPosition(const glm::vec3 newPos)
{
	this->position = newPos;
}
inline void Transform::SetLocalRotation(const glm::vec3 newRot)
{
	this->rotation = newRot;
}
inline void Transform::SetLocalScale(const glm::vec3 newScale)
{
	this->scale = newScale;
}

glm::fquat euler2quat(glm::vec3 euler)
{
	glm::fquat xQuat(1, 0, 0, euler.x);
	glm::fquat yQuat(0, 1, 0, euler.y);
	glm::fquat zQuat(0, 0, 1, euler.z);
	return xQuat * yQuat * zQuat;
}

glm::vec3 quat2euler(glm::fquat q)
{
	glm::vec3 rot;

	// roll (x-axis rotation)
	float sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	rot.x = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		rot.y = copysign(3.141f / 2, sinp); // use 90 degrees if out of range
	else
		rot.y = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	rot.z = atan2(siny_cosp, cosy_cosp);

	return rot;
}