#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

class StainSet
{
public:
	StainSet(GLint perlinNoiseTexture);
	~StainSet();

	// The size of a paint drop texture.
	const GLint dropSize = 32;

	// The size of a stain texture.
	const GLint stainSize = 128;

	// The amount of paint drops to add to the stain texture.
	int nDropsPerStain = 10;

	// Adds a new paint drop texture to use when generating new stains.
	void AddPaintDropTexture(GLint stainID);

	// Generates a new stain.
	GLint GetNextRandomStain();

	// Starts the procedural stain generation thread.
	void StartProceduralGenerationThread();

private:
	// The set of textures used to generate stains.
	std::vector<GLuint> dropTextures;

	// The last generated stain texture.
	GLuint currentStainTex;

	// The texture used to add noise.
	GLint perlinNoiseTexture;

	// The drop textures stored as array of floats.
	std::vector<std::vector<GLfloat>> dropMasks;

	void GenerateStain(GLint stainSize, std::vector<GLfloat>& pixels);

	void ProceduralGenerationThread();

	std::default_random_engine generator;
	std::normal_distribution<float> distribution;

	std::mutex mtx, procMtx;
	std::condition_variable cv;

	// Contains the procedurally generated stains.
	std::queue<std::vector<GLfloat>> proceduralStains;

	std::thread proceduralGenerationThread;
};

