#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL\GL.h>

// Include user-defined headers.
#include "Model.hpp"
#include "PlayerController.hpp"
#include "Material.hpp"
#include "RenderingEngine.hpp"
#include "PhysicsModule.h"
#include "RigidbodyComponent.h"
#include "SelfMovingComponent.h"

#include "PaintableComponent.h"
#include "StainSet.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define SCREEN_WIDTH 1920	
#define SCREEN_HEIGHT 1080

// Callback for errors.
static void error_callback(int error, const char* description);
// Callback for keyboard input.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// Callback for mouse input.
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Texture-loading function.
GLint LoadTexture(const char* path);
// Processes input
void ApplyPlayerCameraMovements(GLfloat deltaTime);

// Pressed keys.
bool keys[1024];
GLfloat lastX = 0.0f, lastY = 0.0f;
// Spawns the player.
PlayerController playerController("../../Project/ProgettoPGTR/Models/PaintGun.obj", glm::vec3(0.0f, 2.0f, 0.0f));

// The main instance of the rendering engine.
RenderingEngine *renderingEngine;

// The main set of shaders.
ShaderSet* SHADERS;

// The physics component.
PhysicsModule *physicsModule;

// Projection and view matrices used in the rendering.
glm::mat4 projection, view;

// The mouse pointer's position.
float cursorX, cursorY;

Model* paintBallModel;

// The gaussian kernel with linear layout.
GLfloat* gaussKernel = (GLfloat*)malloc(sizeof(GLfloat) * 49);

int main(int argc, char *argv[])
{
	//Set the error callback  
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW  
	if (!glfwInit())
	{
		std::exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional  
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Request a specific OpenGL version  
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); //Request a specific OpenGL version  
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//Declare a window object  
	GLFWwindow* window;

	// Create a window and create its OpenGL context
	// Fullscreen
	//window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Paint Game", glfwGetPrimaryMonitor(), NULL);
	// Window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Paint Game", NULL, NULL);

	// Hides mouse cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);

	//Sets the input callbacks.  
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	// Retrieves the frame buffer size.
	int viewportWidth, viewportHeight;
	glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);

	// Initializes the rendering engine and the shader set.
	renderingEngine = new RenderingEngine(&playerController);
	SHADERS = new ShaderSet();
	physicsModule = new PhysicsModule();

	// Loads the models
	//Model scenery("../../Project/ProgettoPGTR/Models/SplatoonTestScenery.obj");
	Model floorModel(CUBE_OBJ_PATH);
	Model wallModel(CUBE_OBJ_PATH);
	Model towerModel(CYLINDER_OBJ_PATH);
	Model bunnyModel(BUNNY_OBJ_PATH);
	Model sphereModel(SPHERE_OBJ_PATH);
	paintBallModel = new Model(SPHERE_OBJ_PATH);

	// Loads the scenery's texture.
	GLuint crackedTexture = LoadTexture("Textures/Floor.png");
	GLuint asphaltTexture = LoadTexture("Textures/Asphalt.jpg");
	GLuint asphaltNormalMap = LoadTexture("Textures/Asphalt-NormalMap.jpg");
	GLint brickWallTexture = LoadTexture("Textures/BrickWall.jpg");
	GLint brickWallNormalMap = LoadTexture("Textures/BrickWall-NormalMap.jpg");
	GLint woodBoxNormalMap = LoadTexture("Textures/WoodBox-NormalMap.jpg");
	GLint perlinNoiseTex = LoadTexture("Textures/PerlinNoise2.png");
	StainSet* stainSet = new StainSet(perlinNoiseTex);
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop0.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop1.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop2.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop3.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop4.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop5.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop6.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop7.png"));
	stainSet->AddPaintDropTexture(LoadTexture("Textures/Drop8.png"));
	stainSet->StartProceduralGenerationThread();

	GLint uiTex = LoadTexture("Textures/Cursor.png");
	renderingEngine->uiTexture = uiTex;
	glm::vec3 pointLightPosition(0, 5, 5);

	// Wall materials.
	Material wallMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet wallMatParams;
	wallMatParams.diffuseTexture = brickWallTexture;
	wallMatParams.normalMap = brickWallNormalMap;
	wallMatParams.perlinNoise = perlinNoiseTex;
	wallMatParams.shininess = 5.0f;
	wallMatParams.specularColor = glm::vec3(1.f, 1.f, 1.f);
	wallMatParams.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	wallMatParams.pointLightPosition = pointLightPosition;
	wallMatParams.repeat = glm::vec2(50.f, 50.f);
	wallMaterial.shaderParams = &wallMatParams;

	Material wall2Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]),
		wall3Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]),
		wall4Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	wall2Material.shaderParams = &wallMatParams.Clone();
	wall3Material.shaderParams = &wallMatParams.Clone();
	wall4Material.shaderParams = &wallMatParams.Clone();

	// Floor material.
	Material floorMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet floorMatParams;
	floorMatParams.diffuseTexture = asphaltTexture;
	floorMatParams.normalMap = asphaltNormalMap;
	floorMatParams.perlinNoise = perlinNoiseTex;
	floorMatParams.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	floorMatParams.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	floorMatParams.pointLightPosition = pointLightPosition;
	floorMatParams.repeat = glm::vec2(50.f, 50.f);
	floorMaterial.shaderParams = &floorMatParams;

	Material upMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	upMaterial.shaderParams = &floorMatParams.Clone();

	// Tower material.
	Material towerMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet towerMatParams;
	towerMatParams.diffuseTexture = crackedTexture;
	towerMatParams.perlinNoise = perlinNoiseTex;
	towerMatParams.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	towerMatParams.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	towerMatParams.pointLightPosition = pointLightPosition;
	towerMatParams.repeat = glm::vec2(5.f, 20.f);
	towerMaterial.shaderParams = &towerMatParams;

	// Sphere material.
	Material sphereMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet sphereMatParams;
	//sphereMatParams.diffuseTexture = crackedTexture;
	sphereMatParams.diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
	sphereMatParams.Ks = 0.1f;
	sphereMatParams.perlinNoise = perlinNoiseTex;
	sphereMatParams.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	sphereMatParams.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	sphereMatParams.pointLightPosition = glm::vec3(0, 5, -5);
	sphereMatParams.repeat = glm::vec2(10.f, 10.f);
	sphereMaterial.shaderParams = &sphereMatParams;

	// Bunny material.
	Material bunnyMaterial(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet bunnyMatParams;
	bunnyMatParams.diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);
	bunnyMatParams.perlinNoise = perlinNoiseTex;
	bunnyMatParams.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	bunnyMatParams.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	bunnyMatParams.pointLightPosition = pointLightPosition;
	bunnyMatParams.repeat = glm::vec2(10.f, 10.f);
	bunnyMatParams.shininess = 5.0f;
	bunnyMaterial.shaderParams = &bunnyMatParams;

	// Paintball material.
	Material paintBallMaterial(&SHADERS->availableShaders[SHADER_LAMBERT]);
	LambertShaderParamSet pbMatParams;
	pbMatParams.color = glm::vec3(0.0f, 1.0f, 0.0f);
	pbMatParams.Kd = 0.8f;
	pbMatParams.repeat = 30.0f;
	pbMatParams.pointLightPosition = pointLightPosition;
	paintBallMaterial.shaderParams = &pbMatParams;
	playerController.SetPaintMaterial(&paintBallMaterial);

	// Wood box material.
	Material woodBox1Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	PaintableBlinnPhongTexturingShaderParamSet woodBoxParams;
	woodBoxParams.diffuseColor = glm::vec3(153.f / 255.f, 102.f / 255.f, 51.f / 255.f);
	woodBoxParams.normalMap = woodBoxNormalMap;
	woodBoxParams.perlinNoise = perlinNoiseTex;
	woodBoxParams.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	woodBoxParams.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	woodBoxParams.pointLightPosition = pointLightPosition;
	woodBoxParams.repeat = glm::vec2(3.f, 3.f);
	woodBoxParams.shininess = 5.0f;
	woodBoxParams.Ka = 0.1f;
	woodBoxParams.Kd = 0.8f;
	woodBox1Material.shaderParams = &woodBoxParams;

	Material woodBox2Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]),
		woodBox3Material(&SHADERS->availableShaders[SHADER_BLINN_PHONG]);
	woodBox2Material.shaderParams = &woodBoxParams.Clone();
	woodBox3Material.shaderParams = &woodBoxParams.Clone();

	GameObject *floor = renderingEngine->AddGameObject("Floor", &floorModel, 
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(10, 0.01, 10), nullptr, &floorMaterial);
	GameObject *wall1 = renderingEngine->AddGameObject("Wall1", &wallModel, 
		glm::vec3(0, 5, 9), glm::vec3(0, 0, 0), glm::vec3(10, 5, 1), nullptr, &wallMaterial);
	GameObject *wall2 = renderingEngine->AddGameObject("Wall2", &wallModel,
		glm::vec3(-9, 5, 0), glm::vec3(0, 0, 0), glm::vec3(1, 5, 10), nullptr, &wall2Material);
	GameObject *wall3 = renderingEngine->AddGameObject("Wall3", &wallModel,
		glm::vec3(9, 5, 0), glm::vec3(0, 0, 0), glm::vec3(1, 5, 10), nullptr, &wall3Material);
	GameObject *wall4 = renderingEngine->AddGameObject("Wall4", &wallModel,
		glm::vec3(0, 5, -9), glm::vec3(0, 0, 3.14f / 2.0f), glm::vec3(5, 9, 1), 
		nullptr, &wall4Material);
	GameObject *sphere = renderingEngine->AddGameObject("Sphere", &sphereModel, glm::vec3(-4, 1.5, -6),
		glm::vec3(0, 0, 0), glm::vec3(2, 2, 2), nullptr, &sphereMaterial);
	GameObject *tower = renderingEngine->AddGameObject("Tower", &towerModel, glm::vec3(0, 4.5, -6),
		glm::vec3(0, 0, 0), glm::vec3(2, 10, 2), nullptr, &towerMaterial);
	GameObject *box1 = renderingEngine->AddGameObject("Box1", &wallModel, glm::vec3(5, 1, 5),
		glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), nullptr, &woodBox1Material);
	GameObject *box2 = renderingEngine->AddGameObject("Box2", &wallModel, glm::vec3(5, 3, 6),
		glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), nullptr, &woodBox2Material);
	GameObject *box3 = renderingEngine->AddGameObject("Box3", &wallModel, glm::vec3(5, 1, 7),
		glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), nullptr, &woodBox3Material);

	GameObject* up = renderingEngine->AddGameObject("Up", &floorModel, glm::vec3(0, 10.0f, 0),
		glm::vec3(0, 0, 0), glm::vec3(10, 0.01, 10), nullptr, &upMaterial);
	GameObject* bunny = renderingEngine->AddGameObject("Bunny", &bunnyModel, glm::vec3(4, 1.5, -6),
		glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f), nullptr, &bunnyMaterial);

	// Physics initialization.
	Transform* floorTr = floor->GetTransform();
	Transform* wall1Tr = wall1->GetTransform();
	Transform* wall2Tr = wall2->GetTransform();
	Transform* wall3Tr = wall3->GetTransform();
	Transform* wall4Tr = wall4->GetTransform();
	Transform* box1Tr = box1->GetTransform(), *box2Tr = box2->GetTransform(),
		*box3Tr = box3->GetTransform(), *bunnyTr = bunny->GetTransform();
	Transform* sphereTr = sphere->GetTransform();
	Transform* towerTr = tower->GetTransform();
	btRigidBody* planeRb = physicsModule->createRigidBody(2, glm::vec3(0, -0.01, 0), 
		floorTr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	floor->AddComponent(new RigidbodyComponent(floor, physicsModule, planeRb));
	btRigidBody* upRb = physicsModule->createRigidBody(0, glm::vec3(0, 10, 0),
		glm::vec3(20, 0.5, 20), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	up->AddComponent(new RigidbodyComponent(up, physicsModule, upRb));
	btRigidBody* wall1Rb = physicsModule->createRigidBody(0, wall1Tr->GetAbsolutePosition(),
		wall1Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	wall1->AddComponent(new RigidbodyComponent(wall1, physicsModule, wall1Rb));
	btRigidBody* wall2Rb = physicsModule->createRigidBody(0, wall2Tr->GetAbsolutePosition(),
		wall2Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	wall2->AddComponent(new RigidbodyComponent(wall2, physicsModule, wall2Rb));
	btRigidBody* wall3Rb = physicsModule->createRigidBody(0, wall3Tr->GetAbsolutePosition(),
		wall3Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	wall3->AddComponent(new RigidbodyComponent(wall3, physicsModule, wall3Rb));
	btRigidBody* wall4Rb = physicsModule->createRigidBody(0, wall4Tr->GetAbsolutePosition(),
		wall4Tr->GetAbsoluteScale(), glm::vec3(0, 0, 3.14f / 2.0f), 0, 0.3, 0.3);
	wall4->AddComponent(new RigidbodyComponent(wall4, physicsModule, wall4Rb));
	btRigidBody* sphereRb = physicsModule->createRigidBody(1, sphereTr->GetAbsolutePosition(),
		sphereTr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	sphere->AddComponent(new RigidbodyComponent(sphere, physicsModule, sphereRb));
	btRigidBody* towerRb = physicsModule->createRigidBody(3, towerTr->GetAbsolutePosition(),
		glm::vec3(2.f, 10.f, 2.f), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	tower->AddComponent(new RigidbodyComponent(tower, physicsModule, towerRb));

	btRigidBody* box1Rb = physicsModule->createRigidBody(0, box1Tr->GetAbsolutePosition(),
		box1Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	box1->AddComponent(new RigidbodyComponent(box1, physicsModule, box1Rb));
	btRigidBody* box2Rb = physicsModule->createRigidBody(0, box2Tr->GetAbsolutePosition(),
		box2Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	box2->AddComponent(new RigidbodyComponent(box2, physicsModule, box2Rb));
	btRigidBody* box3Rb = physicsModule->createRigidBody(0, box3Tr->GetAbsolutePosition(),
		box3Tr->GetAbsoluteScale(), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	box3->AddComponent(new RigidbodyComponent(box3, physicsModule, box3Rb));
	btRigidBody* bunnyRb = physicsModule->createRigidBody(0, bunnyTr->GetAbsolutePosition(),
		glm::vec3(2.5f, 1.5, 1), glm::vec3(0, 0, 0), 0, 0.3, 0.3);
	bunny->AddComponent(new RigidbodyComponent(bunny, physicsModule, bunnyRb));
	GLfloat maxFrameRate = 1.0f / 60.0f;

	//Set blue as background color  
	glClearColor(0.0f, 0.0f, 1.0f, 0.75f);

	// The bunny autonomously moves along a line.
	sphere->AddComponent(new SelfMovingComponent(sphere, glm::vec3(0, 0, 4), 0.25f));

	// Enables depth buffer.
	glEnable(GL_DEPTH_TEST);

	// Creates projection matrix.
	// Projection matrix: angolo FOV angle, aspect ratio, near plane and far plane.
	projection = glm::perspective(45.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 10000.0f);

	// Sets the wall as paintable.
	wall1->AddComponent(new PaintableComponent(wall1, 
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 600));
	wall2->AddComponent(new PaintableComponent(wall2,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 600));
	wall3->AddComponent(new PaintableComponent(wall3,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 600));
	wall4->AddComponent(new PaintableComponent(wall4,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 600));
	floor->AddComponent(new PaintableComponent(floor, 
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 1024));
	sphere->AddComponent(new PaintableComponent(sphere, 
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 256));
	tower->AddComponent(new PaintableComponent(tower, 
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 256));

	box1->AddComponent(new PaintableComponent(box1,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 150));
	box2->AddComponent(new PaintableComponent(box2,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 150));
	box3->AddComponent(new PaintableComponent(box3,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 150));
	up->AddComponent(new PaintableComponent(up,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 800));
	bunny->AddComponent(new PaintableComponent(bunny,
		&SHADERS->availableShaders[SHADER_PAINTMAP], stainSet, 200));

	// Main Loop
	// Check if the ESC key had been pressed or if the window had been closed
	GLfloat lastFrameTime = 0.0f, deltaTime;
	GLfloat sceneryMaterial[] = { 1.0f, 1.0f, 0.0f };
	while (!glfwWindowShouldClose(window))
	{
		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

		GLfloat currentFrameTime = (GLfloat)glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
		float frameRate = 1.0f / deltaTime;
		//std::cout << frameRate << std::endl;

		// Updates the physics simulation.
		physicsModule->dynamicsWorld->stepSimulation((deltaTime < maxFrameRate ? deltaTime : maxFrameRate), 10);
		physicsModule->PerformCollisionDetection();

		// Moves the main character.
		ApplyPlayerCameraMovements(deltaTime);

		// Updates all the components.
		renderingEngine->UpdateComponents(deltaTime);
		
		// Destroys the gameobjects that need to be destroyed.
		renderingEngine->DestroyGameObjects();

		// Resets the viewport.
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Main rendering routine.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		renderingEngine->RenderAll(playerController.GetViewMatrix(), projection);		

		//Swaps buffers  
		glfwSwapBuffers(window);
	}  

	// Destroys all the used shaders.
	for (int i = 0; i < SHADERS->availableShaders.size(); i++)
		SHADERS->availableShaders[i].Delete();

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW  
	glfwTerminate();

	std::exit(EXIT_SUCCESS);
}

//Define an error callback  
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Manages other keyboard inputs.
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_SPACE && !keys[key])
			playerController.Shoot(paintBallModel, renderingEngine, physicsModule,
				cursorX / SCREEN_WIDTH, cursorY / SCREEN_HEIGHT, projection);
		keys[key] = true;

		if (key == GLFW_KEY_E)
			renderingEngine->hdrFboSnapshot = true;
	}
	if (action == GLFW_RELEASE)
		keys[key] = false;
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos;
	cursorX = xpos;
	cursorY = ypos;

	// current coordinates are the last position in the next frame.
	lastX = (GLfloat) xpos;
	lastY = (GLfloat) ypos;

	// invio lo spostamento alla classe Camera per aggiornare la visualizzazione
	playerController.ProcessMouseMovement(xoffset, yoffset, true);
}

void ApplyPlayerCameraMovements(GLfloat deltaTime)
{
	if (keys[GLFW_KEY_W])
		playerController.ProcessKeyboardCommand(NORTH, deltaTime);
	if (keys[GLFW_KEY_S])
		playerController.ProcessKeyboardCommand(SOUTH, deltaTime);
	if (keys[GLFW_KEY_A])
		playerController.ProcessKeyboardCommand(WEST, deltaTime);
	if (keys[GLFW_KEY_D])
		playerController.ProcessKeyboardCommand(EAST, deltaTime);
}

// Loads a texture from disk, creates an OpenGL texture and returns its ID.
GLint LoadTexture(const char* path)
{
	GLuint textureImage;
	int w, h, channels;
	unsigned char* image;
	image = stbi_load(path, &w, &h, &channels, STBI_rgb);

	if (image == nullptr)
		std::cout << "Failed to load texture!" << std::endl;

	glGenTextures(1, &textureImage);
	glBindTexture(GL_TEXTURE_2D, textureImage);
	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	stbi_image_free(image);

	return textureImage;

}