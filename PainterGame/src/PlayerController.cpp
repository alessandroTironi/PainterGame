#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include <PlayerController.hpp>
#include "PaintBallComponent.hpp"
#include "RigidbodyComponent.h"

using namespace glm;

PlayerController::PlayerController(const std::string& gunModelPath, vec3 position, vec3 up, GLfloat yaw, 
	GLfloat pitch) : localFront(vec3(0.0f, 0.0f, -1.0f)), speed(INITIAL_SPEED), 
	mouseSensitivity(INITIAL_MOUSE_SENSITIVITY)
{
	// Loads the gun model.
	//Model gun(gunModelPath);
	//this->gunModel = &gun;

	// Assigns transform data.
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	this->UpdateCameraVectors();
}

void PlayerController::SetPaintMaterial(Material* material)
{
	paintMaterial = material;
}

// Computes the current view matrix.
mat4 PlayerController::GetViewMatrix()
{
	return glm::lookAt(this->position, this->position + this->localFront, this->localUp);
}

// Applies movement to the player.
void PlayerController::ProcessKeyboardCommand(CameraDirections direction, GLfloat deltaTime)
{
	// Computes velocity for movement smooth.
	GLfloat velocity = this->speed * deltaTime;
	// Modifies the position based on the direction.
	if (direction == NORTH)
		this->position += this->worldFront * velocity;
	if (direction == SOUTH)
		this->position -= this->worldFront * velocity;
	if (direction == EAST)
		this->position += this->right * velocity;
	if (direction == WEST)
		this->position -= this->right * velocity;
}

// Applies movement to the camera.
void PlayerController::ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch)
{
	// Applies sensitivity for weighting the movement.
	xOffset *= this->mouseSensitivity;
	yOffset *= this->mouseSensitivity;

	// Updates rotation angles.
	this->yaw += xOffset;
	this->pitch += yOffset;

	// Applies a constraint on the pitch rotation
	if (constrainPitch)
	{
		if (this->pitch > 89.0f)
			this->pitch = 89.0f;
		if (this->pitch < -89.0f)
			this->pitch = -89.0f;
	}

	this->UpdateCameraVectors();
}

// Updates the camera vectors from the current yaw and pitch values.
void PlayerController::UpdateCameraVectors()
{
	// Computes localFront from yaw and pitch.
	vec3 front;
	front.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
	front.y = sin(radians(this->pitch));
	front.z = sin(radians(this->yaw)) * cos(radians(this->pitch));

	// World front equals local front except for the Y which is zero.
	this->worldFront = this->localFront = glm::normalize(front);
	this->worldFront.y = 0.0f;

	// Uses cross product for computing right and localUp vector
	this->right = glm::cross(this->localFront, this->worldUp);
	this->localUp = glm::cross(this->right, this->localFront);

}

glm::vec3 PlayerController::GetPosition()
{
	return this->position;
}


void PlayerController::Shoot(Model* paintBallModel, RenderingEngine* engine,
	PhysicsModule* physicsModule, float normCursorX, float normCursorY, glm::mat4 projection)
{
	btRigidBody* paintBallRb = physicsModule->createRigidBody(1, this->position, 
		glm::vec3(0.15f, 0.15f, 0.15f), glm::vec3(0, 0, 0), 3, 0.3f, 0.3f);
	GameObject* paintBall = engine->AddGameObject("PaintBall", paintBallModel, this->position, 
		glm::vec3(0, 0, 0), glm::vec3(0.15f, 0.15f, 0.15f), nullptr, paintMaterial);
	paintBall->AddComponent(new RigidbodyComponent(paintBall, physicsModule, paintBallRb));

	PaintBallComponent* paintBallComponent = new PaintBallComponent(paintBall, physicsModule);
	paintBallComponent->SetLocalRight(glm::cross(this->localFront, this->worldUp));
	paintBall->AddComponent(paintBallComponent);

	// Applies the impulse to the projectile.
	// When the player presses the space bar, a paint ball is shot to the center of the screen.
	btVector3 pos, impulse;
	// Initial paintball speed.
	GLfloat shootInitialSpeed = 150.0f;
	glm::mat4 unproject;

	// Shoot direction always is the forward vector.
	glm::vec4 shoot(0.f, 0.f, 1.f, 1.f);

	// Unprojecting the projection and view matrices in order to obatain the paintball
	// direction in world coordinates.
	unproject = glm::inverse(projection * GetViewMatrix());
	// Multiply by speed. 
	shoot = glm::normalize(unproject * shoot) * shootInitialSpeed;

	// Applying the impulse.
	impulse = btVector3(shoot.x, shoot.y, shoot.z);
	paintBallRb->applyCentralImpulse(impulse);

}