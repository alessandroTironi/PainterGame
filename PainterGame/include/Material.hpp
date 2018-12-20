#pragma once
#include <map>

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <GL\GL.h>

#include <glm\glm.hpp>

#include "Shader.hpp"
#include "ShaderSet.hpp"

struct ShaderParamSet;

/// <summary> Represents a material. </summary>
class Material
{
private:
	/// <summary> Maps each uniform name to its location in the shader program. </summary>
	std::map<char*, GLint> uniformLocations;
public:
	/// <summary> Pointer to the shader program. </summary>
	Shader *shader;

	glm::vec3 diffuseColor;

	/// <summary> Pointer to the set of parameters for the shader. </summary> 
	ShaderParamSet *shaderParams;

	/// <summary> Basic constructor. </summary>
	Material(Shader *shader);

	/// <summary> 
	/// Registers a new uniform parameter for the shader. 
	/// </summary>
	void AddUniform(char *uniformName);

	/// <summary>
	/// Loads a uniform parameter of type float.
	/// </summary>.
	void LoadUniform(char* uniformName, GLfloat parameter);

	/// <summary>
	/// Loads a uniform parameter of type int.
	/// </summary>
	void LoadUniform(char* uniformName, GLint parameter);

	/// <summary>
	/// Loads a uniform parameter of type uint.
	/// </summary>
	void LoadUniform(char* uniformName, GLuint parameter);

	/// <summary>
	/// Loads a uniform parameter of type Vector2.
	/// </summary>
	void LoadUniform(char* uniformName, glm::vec2 parameter);

	/// <summary>
	/// Loads a uniform parameter of type Vector3.
	/// </summary>
	void LoadUniform(char* uniformName, glm::vec3 parameter);

	/// <summary>
	/// Loads a uniform parameter of type Matrix 3x3.
	/// </summary>
	void LoadUniform(char* uniformName, glm::mat3 parameter);

	/// <summary>
	/// Loads a uniform parameter of type Matrix 4x4.
	/// </summary>
	void LoadUniform(char* uniformName, glm::mat4 parameter);
};

/// <summary>
/// A generic container for set of shader parameters.
/// </summary>
struct ShaderParamSet 
{
	virtual void LoadUniforms(Material*) = 0;
};

/// <summary>
/// The set of parameters for the lambertian texturing shader.
/// </summary>
struct LambertianTexturingShaderParamSet : ShaderParamSet
{
	// The applied texture
	GLuint texture;

	GLfloat repeat;
	GLfloat kd;

	// Position of light (must be updated for each frame)
	glm::vec3 pointLightPosition;

	void LoadUniforms(Material*);
};

struct BasicShaderParamSet : ShaderParamSet
{
	glm::vec3 color;

	void LoadUniforms(Material*);
};

struct LambertShaderParamSet : ShaderParamSet
{
	glm::vec3 color;
	GLfloat Kd, repeat;
	glm::vec3 pointLightPosition;

	void LoadUniforms(Material*);
};

struct PaintMapShaderParamSet : ShaderParamSet
{
	glm::mat4 paintSpaceMatrix;

	void LoadUniforms(Material*);
};

struct PaintableLambertianTexturingShaderParamSet : LambertianTexturingShaderParamSet
{
	GLint paintMap;

	glm::vec3 paintColor;

	void LoadUniforms(Material*);
};

struct PaintableShaderParamSet : ShaderParamSet
{
	GLint paintMap = -1;
	GLfloat isPaintable = 0.0f;
	glm::vec3 paintColor;
	GLint perlinNoise = -1;

	void LoadUniforms(Material*);
};

struct PaintableBlinnPhongTexturingShaderParamSet : PaintableShaderParamSet
{
	GLint diffuseTexture = -1;
	GLint normalMap = -1;

	glm::vec3 diffuseColor, ambientColor, specularColor;
	GLfloat Kd = 0.8f, Ka = 0.1f, Ks = 0.5f;
	GLfloat shininess = 25.f;
	glm::vec2 repeat = glm::vec2(30.f, 30.f);

	glm::vec3 pointLightPosition;
	

	GLfloat constant = 1.0f, quadratic = 0.032f, linear = 0.09f;

	void LoadUniforms(Material*);

	PaintableBlinnPhongTexturingShaderParamSet Clone();
};