#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PaintableComponent.h"
#include "RenderingEngine.hpp"

#include "bitmap_image.hpp"

void ExportTexture(GLint texture, GLint width, GLint height, std::string name, GLenum format);

PaintableComponent::PaintableComponent(GameObject* gameObject, 
	Shader* paintMapShader, StainSet* stainSet, unsigned int paintMapSize) 
	: AComponent(gameObject, PAINTABLE_COMPONENT)
{
	this->depthMapShader = depthMapShader;
	this->paintMapShader = paintMapShader;
	this->stainSet = stainSet;
	this->PAINTMAP_SIZE = paintMapSize;
}


PaintableComponent::~PaintableComponent()
{
}

void PaintableComponent::OnCreate()
{
	Material* goMat = gameObject->GetMaterial();
	PaintableShaderParamSet *shaderParams =
		static_cast<PaintableShaderParamSet*>(goMat->shaderParams);
	CreatePaintMap();
	shaderParams->paintMap = paintMap;
	shaderParams->paintColor = glm::vec3(0.f, 1.f, 0.f);
	shaderParams->isPaintable = 1.0f;
}

inline void PaintableComponent::CreatePaintMap()
{
	glGenFramebuffers(1, &paintMapFBO);
	// Following texture creation procedure for depth maps.
	glGenTextures(1, &paintMap);
	glBindTexture(GL_TEXTURE_2D, paintMap);
	// Creates texture with only one channel.
	std::vector<GLubyte> pixels(PAINTMAP_SIZE * PAINTMAP_SIZE, (GLubyte)0xffffffff);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, PAINTMAP_SIZE, PAINTMAP_SIZE,
		0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLuint borderColor[] = { UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX };
	glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void PaintableComponent::RenderPaintMap(glm::mat4 paintSpaceMatrix, glm::vec3 paintDirection)
{
	if (paintMap == 0)
		CreatePaintMap();

	Transform* tr = gameObject->GetTransform();
	Model* model = gameObject->GetModel();

	// Creates transformation matrix.
	glm::mat4 modelMatrix = tr->GetTransformMatrix();

	// Computes the projection with the provided shader.
	paintMapShader->Use();
	
	// Loads paintmap shader's uniforms.
	glUniformMatrix4fv(glGetUniformLocation(paintMapShader->program, "paintSpaceMatrix"), 1,
		GL_FALSE, glm::value_ptr(paintSpaceMatrix));
	glViewport(0, 0, PAINTMAP_SIZE, PAINTMAP_SIZE);
	glUniformMatrix4fv(glGetUniformLocation(paintMapShader->program, "modelMatrix"), 1,
		GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform3fv(glGetUniformLocation(paintMapShader->program, "paintBallDirection"), 1,
		glm::value_ptr(paintDirection));
	// The current paint map.
	glBindImageTexture(3, paintMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8UI);
	// The current depth map and its size.
	glUniform1i(glGetUniformLocation(paintMapShader->program, "paint_map_size"), PAINTMAP_SIZE);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, stainSet->GetNextRandomStain());
	glUniform1i(glGetUniformLocation(paintMapShader->program, "stainTex"), 11);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	model->Draw(*paintMapShader);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);	
	
	// Resets the state.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PaintableComponent::OnUpdate(float deltaTime)
{
	
}

GLuint PaintableComponent::GetPaintMap() { return paintMap; }

void ExportTexture(GLint texture, GLint width, GLint height, std::string name, GLenum format)
{
	// Image Writing
	unsigned char* imageData;
	if (format == GL_RGB)
		imageData = (unsigned char *)malloc((int)(width * height * 3));
	else if (format == GL_DEPTH_COMPONENT)
		imageData = (unsigned char *)malloc((int)(width * height));
	else
	{
		std::cout << "ExportTexture: unknown format id" << std::endl;
		return;
	}
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, imageData);
	//write 
	bitmap_image image(width, height);
	image_drawer draw(image);

	for (unsigned int i = 0; i < image.width(); ++i)
	{
		for (unsigned int j = 0; j < image.height(); ++j)
		{
			if (format == GL_RGB)
				image.set_pixel(height - j - 1, width - i - 1, *(++imageData), *(++imageData), *(++imageData));
			else
				image.set_pixel(height - j - 1, width - i - 1, *(++imageData), *imageData, *imageData);
		}
	}

	image.save_image(name);
	glBindTexture(GL_TEXTURE_2D, 0);
}