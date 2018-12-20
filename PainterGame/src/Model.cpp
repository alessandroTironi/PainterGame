#include <vector>

// Library for image loading.
#include <stb_image\stb_image.h>

#include "Model.hpp"

GLint TextureFromFile(const char* path, string directory);

Model::Model(const std::string& path)
{
	this->Load(path);
}

// Renders the model by calling Mesh.Draw().
void Model::Draw(Shader shader)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw(shader);
}

// Destructor: de-allocates mesh memory.
Model::~Model()
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Delete();
}

void Model::Load(std::string path)
{
	// Defines the Assimp importer and the post-processing operations.
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices 
		| aiProcess_FlipUVs 
		| aiProcess_GenSmoothNormals 
		| aiProcess_CalcTangentSpace
	);

	// Error check.
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// recupera la directory dal path del modello
	//this->directory = path.substr(0, path.find_last_of('/'));

	// Starts the processing of the Assimp's data struct.
	this->ProcessAssimpNode(scene->mRootNode, scene);
}

void Model::ProcessAssimpNode(aiNode* node, const aiScene* scene)
{
	// For each mesh
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// Extracts the Mesh and adds it to the meshes vector.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessAssimpMesh(mesh, scene));
	}

	// Repeat for each child of the current node.
	for (GLuint i = 0; i < node->mNumChildren; i++)
		this->ProcessAssimpNode(node->mChildren[i], scene);
}

Mesh Model::ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene)
{
	// Vectors for the read vertices and faces.
	vector<Vertex> vertices;
	vector<GLuint> faceIndices;
	vector<Texture> textures;

	// For each vertex
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Converts Assimp's data structs to GLM's data structs.
		glm::vec3 vector;
		// Coordinate vertici
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// Normali
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		// Texture Coordinates

		// If the model has texture coordinates, assign them to a GLM data struct, otherwise set them to 0.
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			// Assume a unique set of UV coordinates.
			// In realtà, è possibile avere fino a 8 diverse coordinate texture: per altri modelli e formati, questo codice va adattato e modificato
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;

			// Tangenti
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;
			// Bitangenti
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
		else {
			vertex.uv = glm::vec2(0.0f, 0.0f);
			cout << "WARNING::ASSIMP:: MODEL WITHOUT UV COORDINATES -> TANGENT AND BITANGENT ARE = 0" << endl;
		}
		// Adds the vertex to the list.    
		vertices.push_back(vertex);
	}

	// Gets the indices of the vertices the mesh is made of.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			faceIndices.push_back(face.mIndices[j]);
	}

	// Processes the materials.
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. Normal maps
		std::vector<Texture> normalMaps = this->LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. Height maps
		std::vector<Texture> heightMaps = this->LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	// Creates the new mesh with the loaded vertices, faces and textures.
	return Mesh(vertices, faceIndices, textures);
}

// Loads (if not loaded yet) the textures defined by the model's materials.
vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	unsigned int texCount = mat->GetTextureCount(type);
	for (GLuint i = 0; i < texCount; i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Skips this texture if already loaded.
		GLboolean skip = false;
		for (GLuint j = 0; j < loadedTextures.size(); j++)
		{
			if (loadedTextures[j].path == str)
			{
				textures.push_back(loadedTextures[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->loadedTextures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

// Loads a texture from the specified file path.
GLint TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, 0, STBI_rgb);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	return textureID;
}

