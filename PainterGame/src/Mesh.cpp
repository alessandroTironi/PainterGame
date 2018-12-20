#include <sstream>

#include <Mesh.hpp>

using namespace std;

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> faceIndices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->faceIndices = faceIndices;
	this->textures = textures;

	// Sets the mesh.
	// Creates the buffer.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Activates the VAO
	glBindVertexArray(this->VAO);
	// Loads vertices into the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 
		&this->vertices[0], GL_STATIC_DRAW);
	// Loads data into the EBO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faceIndices.size() * sizeof(GLuint), 
		&this->faceIndices[0], GL_STATIC_DRAW);

	// Sets pointers to the vertices' attributes.
	// Vertices' positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertices' normals.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	// Vertices' UVs.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	// Tangenti
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
	// Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}

void Mesh::Delete()
{
	// De-allocates the buffer objects.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// Rendering command.
void Mesh::Draw(Shader shader)
{
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	GLuint heightNr = 1;
	
	const size_t nTextures = textures.size();
	for (GLuint i = 0; i < nTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
											// Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		else if (name == "texture_normal")
			ss << normalNr++; // Transfer GLuint to stream
		else if (name == "texture_height")
			ss << heightNr++; // Transfer GLuint to stream
		number = ss.str();
		// Now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.program, (name + number).c_str()), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

	// Activates VAO
	glBindVertexArray(this->VAO);
	// Renders VAO data.
	glDrawElements(GL_TRIANGLES, (GLsizei)this->faceIndices.size(), GL_UNSIGNED_INT, 0);
	// De-activates VAO.
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}