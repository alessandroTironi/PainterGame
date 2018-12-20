#include "StainSet.h"
#include <iostream>

#include <algorithm>
#include <ctime>

void ExportTexture(GLint texture, GLint width, GLint height, std::string name, GLenum format);
int clamp(int value, int min, int max);

bool threadIsRunning = false;

StainSet::StainSet(GLint perlinNoiseTexture)
{
	dropTextures = std::vector<GLuint>();
	this->perlinNoiseTexture = perlinNoiseTexture;
	glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_2D, perlinNoiseTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	this->distribution = std::normal_distribution<float>(stainSize / 2 - dropSize / 2, 20);
	generator.seed(time(0));
	srand(time(0));
}


StainSet::~StainSet()
{
}

void StainSet::StartProceduralGenerationThread()
{
	if (proceduralGenerationThread.joinable())
		proceduralGenerationThread.join();
	threadIsRunning = true;
	proceduralGenerationThread = std::thread(&StainSet::ProceduralGenerationThread, this);
}

void StainSet::AddPaintDropTexture(GLint stainID)
{
	dropTextures.push_back(stainID);

	// Extracts the float vector from the image.
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, stainID);
	std::vector<GLfloat> newMask(dropSize * dropSize, 1.0f);
	std::vector<GLfloat> pixels(dropSize * dropSize * 3, 1.0f);
	int size = 32 * 32 * 3;
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &pixels[0]);
	for (int i = 0; i < size; i += 3)
		newMask[i / 3] = pixels[i];
	dropMasks.push_back(newMask);

	// Restores OpenGL state.
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

GLint StainSet::GetNextRandomStain()
{
	// Creates the new stain texture.
	const GLint stainSize = 128;
	
	// Deletes the currently used texture.
	glDeleteTextures(1, &currentStainTex);

	std::vector<GLfloat> pixels(stainSize * stainSize, 1.0f);
	if (proceduralStains.size() > 0)
	{
		mtx.lock();
		pixels = proceduralStains.front();
		proceduralStains.pop();
		mtx.unlock();

		// awake thread
		if (!threadIsRunning)
			StartProceduralGenerationThread();
	}
	else
		GenerateStain(stainSize, pixels);
	
	// Generates the texture.
	glGenTextures(1, &currentStainTex);
	glBindTexture(GL_TEXTURE_2D, currentStainTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, stainSize, stainSize, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	return currentStainTex;
}

void StainSet::ProceduralGenerationThread()
{
	//std::cout << "[PROCEDURAL]: I am awake!" << std::endl;
	while (true)
	{
		// Performing the while-loop end condition evaluation in order to
		// apply mutual exclusion.
		mtx.lock();
		int nStains = proceduralStains.size();
		mtx.unlock();
		if (nStains >= 50)
			break;

		int stainSize = 128;
		std::vector<GLfloat> pixels(stainSize * stainSize, 1.0f);
		GenerateStain(stainSize, pixels);
			
		mtx.lock();
		proceduralStains.push(pixels);
		mtx.unlock();

	}

	// sleep
	//std::cout << "[PROCEDURAL]: I am sleeping..." << std::endl;
	threadIsRunning = false;
}

inline void StainSet::GenerateStain(GLint stainSize, std::vector<GLfloat>& pixels)
{
	// Randomly adds paint drops on the stain texture.
	// Drops are added in a random position and are added to the paint already
	// on the texture.
	for (int i = 0; i < nDropsPerStain; i++)
	{
		int randomPaintId = rand() % dropMasks.size();
		int xDst = clamp((int)distribution(generator), 0, stainSize - dropSize);
		int yDst = clamp((int)distribution(generator), 0, stainSize - dropSize);
		for (int x = 0; x < dropSize; x++)
			for (int y = 0; y < dropSize; y++)
			{
				int dropIdx = y * dropSize + x;
				int dstIdx = (yDst + y) * stainSize + (xDst + x);
				pixels[dstIdx] = fminf(pixels[dstIdx], dropMasks[randomPaintId][dropIdx]);
			}
	}
}

inline int clamp(int value, int min, int max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}