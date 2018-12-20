/*
	Combining the Lambert model with plain texturing.
	Diffuse color information from the Lambert model is replaced
	by the texture color.
*/

#version 440 core

// Output color.
layout (location = 0) out vec4 colorFrag;

// Used texture.
uniform sampler2D tex; 
// Weight of components.
uniform float Kd;
// Texture's repetition.
uniform float repeat;
// The paint map.
uniform sampler2D paintMap;
// The color of the paint.
uniform vec3 paintColor;

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
	vec4 colorTex = texture2D(tex, repeated_UV);

	// Computes final color.
	vec3 color = vec3(Kd * lambertian * colorTex);
    
	//float paintAlpha = 1.0 - texture2D(paintMap, interp_UV).r;
	float paintAlpha = 0.0;
	float texelSize = 1.0 / textureSize(paintMap, 0).x;
	for (int x = -1; x < 2; x++)
		for (int y = -1; y < 2; y++)
			paintAlpha += 1.0 - texture2D(paintMap, vec2(interp_UV.x + texelSize * x, interp_UV.y + texelSize * y)).r;
	paintAlpha /= 9.0;

	colorFrag = vec4(color, 1.0) * (1.0 - paintAlpha) + vec4(paintColor * Kd * lambertian, 1.0) * paintAlpha;
}