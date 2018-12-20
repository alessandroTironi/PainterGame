/*
	Combines the Lambert illumination model with the plain texturing
	tecnique. 
	This vertex shader simply provides light and normal data to the 
	fragment shader.
*/
#version 440 core

// Position in world coordinates
layout (location = 0) in vec3 position;
// Vertex' normal
layout (location = 1) in vec3 normal;
// Vertex' UV coordinates
layout (location = 2) in vec2 UV;

// Transform matrices.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Normals transformation matrix ( = transpose of the reverted model-view)
uniform mat3 normalMatrix;

// Position of the point light.
uniform vec3 pointLightPosition;
// Outputs: direction of incident light, normal direction and UV interpolation.
out vec3 lightDir;
out vec3 vNormal;
out vec2 interp_UV;

void main()
{
	interp_UV = UV;

	// Position in model-view coordinates.
	vec4 mvPosition = viewMatrix * modelMatrix * vec4( position, 1.0 );

	// Computing light incidence.
	vec4 lightPos = viewMatrix  * vec4( pointLightPosition, 1.0 );
	lightDir = lightPos.xyz - mvPosition.xyz;

	// Computing normal direction.
	vNormal = normalize( normalMatrix * normal );

	// Computes position in world space.
	gl_Position = projectionMatrix * mvPosition;
}