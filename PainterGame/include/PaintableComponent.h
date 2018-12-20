#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include <glm/glm.hpp>

#include "AComponent.hpp"
#include "PaintBallComponent.hpp"
#include "GameObject.hpp"
#include "Shader.hpp"
#include "PlayerController.hpp"
#include "StainSet.h"

class PaintableComponent : public AComponent
{
public:
	PaintableComponent(GameObject* gameObject, Shader* paintMapShader,
		StainSet* stainSet, unsigned int paintMapSize);
	~PaintableComponent();

	// The default size of the texture used for the paint map.
	GLint PAINTMAP_SIZE = 1024;

	// Creates the texture used for the object's paint map.
	void OnCreate() override;

	void OnUpdate(float deltaTime) override;

	void RenderPaintMap(glm::mat4 paintSpaceMatrix, glm::vec3 paintDirection);

	GLuint GetPaintMap();

private:
	// The shader used to compute paint stains projection.
	Shader* depthMapShader;

	// The shader used to update the paintmap.
	Shader* paintMapShader;

	// The paintmap framebuffer.
	GLuint paintMapFBO;

	// The paintmap.
	GLuint paintMap;

	// The stain set to sample stains from.
	StainSet* stainSet;

	// Points to the rendering engine currently in use.
	PlayerController* player;

	glm::mat4 paintSpaceMatrix;

	void CreatePaintMap();
};

