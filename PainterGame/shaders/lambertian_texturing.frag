/*
	Combining the Lambert model with plain texturing.
	Diffuse color information from the Lambert model is replaced
	by the texture color.
*/

#version 440 core

// Output color.
out vec4 colorFrag;

// Used texture.
uniform sampler2D tex; 
// Weight of components.
uniform float Kd;
// Texture's repetition.
uniform float repeat;

// Input: fragment's light, normal and UV interpolation.
in vec3 lightDir;
in vec3 vNormal;
in vec2 interp_UV;

void main()
{

	// Normalizes normal and light vectors.	
	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightDir.xyz);

	// Computes lambertian coefficient.
	float lambertian = max(dot(L,N), 0.0);

	// Computes the texture color.
	vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
	vec4 colorTex = texture(tex, repeated_UV);

	// Computes final color.
	vec3 color = vec3(Kd * lambertian * colorTex);
	colorFrag  = vec4(color,1.0);
}