#pragma once
#include <list>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Material.hpp"
#include "Model.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "PlayerController.hpp"

#define CUBE_OBJ_PATH "Models/Cube.obj"
#define CYLINDER_OBJ_PATH "Models/Cylinder.obj"
#define SPHERE_OBJ_PATH "Models/Sphere.obj"
#define BUNNY_OBJ_PATH "Models/bunny_lp.obj"

/// <summary>
/// Rotates a 4x4 matrix with a vector3 of Euler angles.
/// </summary>
glm::mat4 rotateEuler(glm::mat4, glm::vec3);

class PlayerController;

class RenderingEngine
{
private:
	/// <summary>The player used to retrieve the view matrix.</summary>
	PlayerController* player;

	///<summary> Contains the game objects that must be rendered.</summary>
	std::list<GameObject*> renderableObjects;

	/// <summary> Contains the paintable game objects.</summary>
	std::list<GameObject*> paintableObjects;

	// The objects that needs to be destroyed.
	vector<GameObject*> objectsToDestroy;

	// Keeps count of all the gameObjects added to the engine.
	unsigned long gameObjectCounter = 0;
	
	// The FBO used to render the scene without UI.
	GLuint hdrFBO;

public:
	RenderingEngine(PlayerController* pc);

	// The shader that renders the UI layer.
	Shader* uiShader;

	// The texture the scene is rendered on.
	GLuint renderedTexture;

	// The UI layer.
	GLuint uiTexture;

	bool hdrFboSnapshot = false;

	/// <summary>
	/// Renders all the objects in the scene.
	/// </summary>
	void RenderAll(glm::mat4 viewMat, glm::mat4 projection);

	/// <summary>
	/// Renders the paint map for the specified game object with the provided paint map shader.
	/// </summary>
	void RenderPaintMap(GameObject* toRender, Shader* paintMapShader);

	/// <summary>
	/// Creates a new GameObject for the scene.
	/// </summary>
	GameObject *AddGameObject(const std::string& name, Model* mesh, glm::vec3 position, glm::vec3 rotation, 
		glm::vec3 scale, GameObject* parent, Material* material);

	// Marks an existing gameobject as ready to be destroyed.
	void MarkGameObjectForDestruction(GameObject* go);

	// Destryes the objects marked as destroyable.
	void DestroyGameObjects();

	/// <summary>
	/// Calls the OnUpdate method on all the gameobjects.
	/// </summary>
	void UpdateComponents(float deltaTime);

	/// <summary>
	/// Sets the gameobject's rendering list.
	/// </summary>
	void SetPaintable(GameObject* go, bool paintable);
};
