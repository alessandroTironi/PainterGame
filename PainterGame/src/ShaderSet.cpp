#include "ShaderSet.hpp"

ShaderSet::ShaderSet()
{
	availableShaders = std::vector<Shader>();
	availableShaders.push_back(Shader("shaders/basic.vert","shaders/basic.frag", SHADER_BASIC));
	availableShaders.push_back(Shader("shaders/lambertian_texturing.vert",
		"shaders/lambertian_texturing.frag", SHADER_LAMBERTIAN_TEXTURING));
	availableShaders.push_back(Shader("shaders/paintmap.vert",
		"shaders/paintmap.frag", SHADER_PAINTMAP));
	availableShaders.push_back(Shader("shaders/lambertian_texturing.vert",
		"shaders/paintable_lambertian_texturing.frag",
		SHADER_PAINTABLE_LAMBERTIAN_TEXTURING));
	availableShaders.push_back(Shader("shaders/phong_tex.vert","shaders/phong_blinn_tex.frag", SHADER_BLINN_PHONG));
	availableShaders.push_back(Shader("shaders/lambertian_texturing.vert",
		"shaders/lambert.frag", SHADER_LAMBERT));
	availableShaders.push_back(Shader("shaders/ui.vert", "shaders/ui.frag", SHADER_UI));
}