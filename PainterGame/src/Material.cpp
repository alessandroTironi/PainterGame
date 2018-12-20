#include "Material.hpp"

#include <glm\vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

Material::Material(Shader *shader)
{
	this->shader = shader;

	// Adds default uniforms.
	this->AddUniform("projectionMatrix");
	this->AddUniform("modelMatrix");
	this->AddUniform("viewMatrix");
	this->AddUniform("normalMatrix");
}

void Material::AddUniform(char *name)
{
	GLint uniformLocation = glGetUniformLocation(shader->program, name);
	this->uniformLocations.emplace(name, uniformLocation);
}

inline void Material::LoadUniform(char* uniformName, GLfloat parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniform1f(location, parameter);
}

inline void Material::LoadUniform(char* uniformName, GLint parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniform1i(location, parameter);
}

inline void Material::LoadUniform(char* uniformName, GLuint parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniform1ui(location, parameter);
}

inline void Material::LoadUniform(char* uniformName, glm::vec2 parameter)
{
	glUniform2fv(glGetUniformLocation(shader->program, uniformName), 1,
		glm::value_ptr(parameter));
}

inline void Material::LoadUniform(char* uniformName, glm::vec3 parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniform3fv(location, 1, glm::value_ptr(parameter));
}

void Material::LoadUniform(char* uniformName, glm::mat3 parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(parameter));
}

void Material::LoadUniform(char* uniformName, glm::mat4 parameter)
{
	GLint location = glGetUniformLocation(shader->program, uniformName);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(parameter));
}

void LambertianTexturingShaderParamSet::LoadUniforms(Material *material)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	material->LoadUniform("tex", 1);
	material->LoadUniform("Kd", kd);
	material->LoadUniform("repeat", repeat);
	material->LoadUniform("pointLightPosition", pointLightPosition);
}

void BasicShaderParamSet::LoadUniforms(Material* material)
{
	material->LoadUniform("color", color);
}

void PaintMapShaderParamSet::LoadUniforms(Material* material)
{
	material->LoadUniform("paintSpaceMatrix", paintSpaceMatrix);
}

void PaintableLambertianTexturingShaderParamSet::LoadUniforms(Material* material)
{
	LambertianTexturingShaderParamSet::LoadUniforms(material);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, paintMap);
	material->LoadUniform("paintMap", 10);
	material->LoadUniform("paintColor", paintColor);
}

void PaintableShaderParamSet::LoadUniforms(Material* material)
{
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, paintMap);
	material->LoadUniform("paintMap", 10);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, perlinNoise);
	material->LoadUniform("perlinNoise", 11);

	material->LoadUniform("isPaintable", isPaintable);
	material->LoadUniform("paintColor", paintColor);
}

void PaintableBlinnPhongTexturingShaderParamSet::LoadUniforms(Material* material)
{
	PaintableShaderParamSet::LoadUniforms(material);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	material->LoadUniform("tex", 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	material->LoadUniform("normalMap", 1);

	material->LoadUniform("usesTexture", diffuseTexture > 0? 1.0f : 0.0f);
	material->LoadUniform("usesNormalMap", normalMap > 0? 1.0f : 0.0f);

	material->LoadUniform("diffuseColor", diffuseColor);
	material->LoadUniform("ambientColor", ambientColor);
	material->LoadUniform("specularColor", specularColor);
	material->LoadUniform("repeat", repeat);
	material->LoadUniform("Kd", Kd);
	material->LoadUniform("Ks", Ks);
	material->LoadUniform("Ka", Ka);
	material->LoadUniform("shininess", shininess);
	material->LoadUniform("constant", constant);
	material->LoadUniform("quadratic", quadratic);
	material->LoadUniform("linear", linear);
	material->LoadUniform("pointLightPosition", pointLightPosition);
}
PaintableBlinnPhongTexturingShaderParamSet PaintableBlinnPhongTexturingShaderParamSet::Clone()
{
	PaintableBlinnPhongTexturingShaderParamSet paramSet;
	paramSet.ambientColor = ambientColor;
	paramSet.specularColor = specularColor;
	paramSet.diffuseColor = diffuseColor;

	paramSet.constant = constant;
	paramSet.linear = linear;
	paramSet.quadratic = quadratic;

	paramSet.repeat = repeat;
	paramSet.Ka = Ka;
	paramSet.Kd = Kd;
	paramSet.Ks = Ks;

	paramSet.pointLightPosition = pointLightPosition;
	paramSet.shininess = shininess;
	paramSet.diffuseTexture = diffuseTexture;
	paramSet.normalMap = normalMap;

	paramSet.perlinNoise = perlinNoise;
	paramSet.isPaintable = isPaintable;
	paramSet.paintColor = paintColor;

	return paramSet;
}

void LambertShaderParamSet::LoadUniforms(Material* material)
{
	material->LoadUniform("color", color);
	material->LoadUniform("Kd", Kd);
	material->LoadUniform("repeat", repeat);
	material->LoadUniform("pointLightPosition", pointLightPosition);
}