#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.hpp"

// Class constructor.
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, int id)
{
	// Step 1: load source from paths.        
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Reads file content and manages errors with exceptions.
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// Converts strings to GLchar*
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();

	// Step 2: compiles headers.
	GLuint vertex, fragment;

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// controllo errori in fase di compilazione
	CheckCompileErrors(vertex, "VERTEX");

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// controllo errori in fase di compilazione
	CheckCompileErrors(fragment, "FRAGMENT");

	// Creates Shader Program
	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	// Checks linker errors.
	CheckCompileErrors(this->program, "PROGRAM");

	// Shaders have been linked: delete them.
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Sets the unique id.
	this->id = id;
}

// Uses this shader.
void Shader::Use()
{
	glUseProgram(this->program);
}

// Deletes the shader.
void Shader::Delete()
{
	glDeleteProgram(this->program);
}

// Compile error check.
void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	// Buffer for error messages.
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		// Gets the shader's compile status.
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			// If an error occurred then retrieve it and print it.
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}
	else
	{
		// Gets the shader's linking status.
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			// If an error occurred then retrieve it and print it.
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}
}