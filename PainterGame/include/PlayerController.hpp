#pragma once

#include <string>

#include <glm/glm.hpp>

#include "RenderingEngine.hpp"
#include "PhysicsModule.h"

using namespace glm;

// Available directions.
enum CameraDirections
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};

// Initial values.
const GLfloat INITIAL_YAW = -90.0f;
const GLfloat INITIAL_PITCH = 0.0f;
const GLfloat INITIAL_SPEED = 5.0f;
const GLfloat INITIAL_MOUSE_SENSITIVITY = 0.25f;

// Normal directions.
const vec3 RIGHT = vec3(1.0f, 0.0f, 0.0f);
const vec3 UP = vec3(0.0f, 1.0f, 0.0f);

class PlayerController 
{
private:
	Model* gunModel;
	
	// The position and directions of the player.
	vec3 position;
	vec3 localFront;
	vec3 worldFront;
	vec3 localUp;
	vec3 worldUp;
	vec3 right;

	// Rotation angles of the player
	GLfloat yaw;
	GLfloat pitch;

	// The material used to render the paint.
	Material* paintMaterial;

	// Updates the direction vectors.
	void UpdateCameraVectors();
public:
	// Camera parameters.
	GLfloat speed;
	GLfloat mouseSensitivity;

	// Constructor.
	PlayerController(const std::string& gunModelPath, vec3 position = vec3(0.0f, 0.0f, 0.0f),
		vec3 up = vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = INITIAL_YAW, GLfloat pitch = INITIAL_PITCH);
	
	// Retrives the view matrix to be used in the rendering process.
	mat4 GetViewMatrix();

	// Processes movement command.
	void ProcessKeyboardCommand(CameraDirections direction, GLfloat deltaTime);

	// Processes camera movement command.
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = false);

	vec3 GetPosition();

	// Shoots a paintball from the current position.
	void Shoot(Model* projectileModel, RenderingEngine* engine, PhysicsModule* physicsModule,
		float cursorX, float cursorY, glm::mat4 projectionMat);

	// Sets the material used to render the paint.
	void SetPaintMaterial(Material* material);
};