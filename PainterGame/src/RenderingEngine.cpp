#include "RenderingEngine.hpp"
#include "RigidbodyComponent.h"
#include "PaintableComponent.h"

#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad();

void ExportTexture(GLint texture, GLint width, GLint height, std::string name, GLenum format);

RenderingEngine::RenderingEngine(PlayerController* player)
{
	renderableObjects = std::list<GameObject*>();
	paintableObjects = std::list<GameObject*>();
	gameObjectCounter = 0;
	this->player = player;

	uiShader = new Shader("shaders/ui.vert", "shaders/ui.frag", SHADER_UI);

	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}

/// <summary>
/// Renders all the objects in the scene.
/// </summary>
/// <param name="pc">Reference to the current player controller.</param>
/// <param name="projection">The projection matrix.</param>
void RenderingEngine::RenderAll(glm::mat4 viewMat, glm::mat4 projection)
{
	glm::vec3 lightPosition(0, 4, -1);
	// Texture unit 1 is the default used with the normal rendering.
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Set blue as background color  
	glClearColor(0.0f, 0.0f, 1.0f, 0.75f);
	
	for (std::list<GameObject*>::iterator it = renderableObjects.begin(); it != renderableObjects.end(); ++it)
	{
		GameObject* currentObj = *it;
		Material* mat = currentObj->GetMaterial();
		Transform* tr = currentObj->GetTransform();
		Model* model = currentObj->GetModel();

		mat->shader->Use();
		mat->shaderParams->LoadUniforms(mat);
		mat->LoadUniform("projectionMatrix", projection);
		mat->LoadUniform("viewMatrix", player->GetViewMatrix());
		glm::mat4 modelMatrix = tr->GetTransformMatrix();
		mat->LoadUniform("modelMatrix", modelMatrix);
		glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(viewMat * modelMatrix));
		mat->LoadUniform("normalMatrix", normalMatrix);
		model->Draw(*mat->shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	uiShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glUniform1i(glGetUniformLocation(uiShader->program, "scene"), 0);
	glUniform1i(glGetUniformLocation(uiShader->program, "ui"), 1);
	renderQuad();
}

/// <summary>
/// Creates a new GameObject for the scene.
/// </summary>
/// <param name="model">The model for the game object.</param>
/// <param name="position">The position in world space.</param>
/// <param name="rotation">The orientation in world space.</param>
/// <param name="scale">The scale of the object.</param>
/// <param name="parent">The object's parent.</param>
/// <param name="material">Reference to the used material.</param>
/// <param name="rigidBody">The attached rigidbody, if any.</param>
GameObject* RenderingEngine::AddGameObject(const std::string& name, Model *model, glm::vec3 position,
	glm::vec3 rotation, glm::vec3 scale, GameObject* parent, Material *material)
{
	GameObject* object = new GameObject(gameObjectCounter++, name, position, rotation, scale, model, 
		material);
	object->SetEngine(this);
	renderableObjects.push_back(object);
	//std::cout << "Created GO named " << name << " with address " << object << std::endl;
	return object;
}

void RenderingEngine::MarkGameObjectForDestruction(GameObject* toDestroy)
{
	if (!toDestroy->IsBeingDestroyed())
		objectsToDestroy.push_back(toDestroy);

}

void RenderingEngine::DestroyGameObjects()
{
	std::vector<GameObject*>::iterator it = objectsToDestroy.begin();
	while (it != objectsToDestroy.end())
	{
		GameObject *go = *it;
		if (go->IsBeingDestroyed())
		{
			renderableObjects.remove(go);
			//std::cout << "Removed gameobject " << go->GetName() << " from rendering engine." << std::endl;
		}
		RigidbodyComponent* rb = static_cast<RigidbodyComponent*>(go->GetComponent(RIGIDBODY_COMPONENT));
		if (rb != NULL)
			delete rb;
		it++;
	}
	objectsToDestroy.clear();
}

void RenderingEngine::UpdateComponents(float deltaTime)
{
	for (std::list<GameObject*>::iterator it = renderableObjects.begin(); it != renderableObjects.end(); ++it)
		(*it)->UpdateComponents(deltaTime);
}

/// <summary>
/// Rotates a 4x4 matrix by a vector3 of Euler angles.
/// </summary>
/// <param name="mat">The matrix to rotate</param>
/// <param name="eulerAngles">The vector3 of Euler angles that describes the rotation.</param>
inline glm::mat4 rotateEuler(glm::mat4 mat, glm::vec3 eulerAngles)
{
	glm::mat4 matrix = mat;
	matrix = glm::rotate(matrix, eulerAngles.x, glm::vec3(1, 0, 0));
	matrix = glm::rotate(matrix, eulerAngles.y, glm::vec3(0, 1, 0));
	matrix = glm::rotate(matrix, eulerAngles.z, glm::vec3(0, 0, 1));
	return matrix;
}

void RenderingEngine::SetPaintable(GameObject* go, bool paintable)
{
	renderableObjects.remove(go);
	paintableObjects.remove(go);
	if (paintable)
	{
		paintableObjects.push_back(go);
		// Updates the shader params.
		PaintableComponent* pc = static_cast<PaintableComponent*>(go->GetComponent(PAINTABLE_COMPONENT));
		PaintableShaderParamSet* sp = static_cast<PaintableShaderParamSet*>(go->GetMaterial()->shaderParams);
		sp->paintMap = pc->GetPaintMap();
		sp->isPaintable = 1;
		sp->paintColor = glm::vec3(0, 1, 0);
	}
	else
		renderableObjects.push_back(go);
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}