#pragma once
#include <string>

#include <gl\glew.h>
#include <glfw\glfw3.h>

class Shader
{
public:
	// The shader program.
	GLuint program;

	// The unique id of the shader.
	int id;

	// Constructor based on vertex shader and fragment shader paths.
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, int id);

	// Activates the shader in the current rendering process.
	void Use();

	// Deletes the shader at the application quit.
	void Delete();

private:
	// Checks GLSL syntax errors.
	void CheckCompileErrors(GLuint shader, std::string type);
};