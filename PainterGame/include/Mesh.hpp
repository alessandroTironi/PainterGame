#pragma once
#include <vector>

//Include GLEW  
#include <GL/glew.h>

//Include GLFW  
#include <GLFW/glfw3.h>  
// GLM data structs for writing in VAO, VBO and EBO
#include <glm\glm.hpp>

// Assimp includes
#include <assimp/Importer.hpp>

#include <Shader.hpp>

using namespace std;

struct Vertex
{
	// Position in space.
	glm::vec3 position;
	// Normal
	glm::vec3 normal;
	// UV coordinates
	glm::vec2 uv;
	// Tangents
	glm::vec3 tangent;
	// Btangents
	glm::vec3 bitangent;
};

struct Texture
{
	// The texture's name.
	GLuint id;
	// The texture's type.
	string type;
	// The texture's path.
	aiString path;
};

class Mesh
{
public:
	// The mesh's vertices.
	vector<Vertex> vertices;
	// The face indices.
	vector<GLuint> faceIndices;
	// The textures.
	vector<Texture> textures;
	
	// Array buffer objects.
	GLuint VAO, VBO, EBO;

	Mesh(vector<Vertex> vertices, vector<GLuint> faceIndices, vector<Texture> textures);

	// Renders the mesh with the provided shader.
	void Draw(Shader shader);

	void Delete();
};
