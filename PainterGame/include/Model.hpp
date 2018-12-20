#pragma once

#include <iostream>
#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"

class Model
{
public:
	// The meshes of the model.
	std::vector<Mesh> meshes;
	// The textures loaded by the model.
	std::vector<Texture> loadedTextures;
	// The model's directory.
	std::string directory;

	// Constructor: sets model file path.
	Model(const std::string& path);

	// Renders the model.
	void Draw(Shader shader);

	// Destructor.
	virtual ~Model();

private:
	// Loads the model from the given path.
	void Load(std::string path);

	// Processes the loaded Assimp's data struct.
	void ProcessAssimpNode(aiNode* node, const aiScene* scene);

	// Processes the mesh loaded by the Assimp importer.
	Mesh ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);

	// Loads textures defined by the model's materials.
	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};