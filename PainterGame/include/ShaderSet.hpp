#pragma once

#include <vector>

#include "Shader.hpp"

#define SHADER_BASIC 0
#define SHADER_LAMBERTIAN_TEXTURING 1
#define SHADER_PAINTMAP 2
#define SHADER_PAINTABLE_LAMBERTIAN_TEXTURING 3
#define SHADER_BLINN_PHONG 4
#define SHADER_LAMBERT 5
#define SHADER_UI 6

class ShaderSet
{
public:
	std::vector<Shader> availableShaders;

	ShaderSet();
};