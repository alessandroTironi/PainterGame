#version 440 core

layout (location = 0) in vec3 position;

layout (location = 1) in vec3 normal;

layout (location = 2) in vec2 UV;

// Paint transform matrix.
uniform mat4 paintSpaceMatrix;

// Matrix of the model.
uniform mat4 modelMatrix;

// The direction of the paintball in world coordinates.
uniform vec3 paintBallDirection;

out vec2 interp_UV;

out vec3 vNormal;

out vec4 posPaintSpace;

out vec3 paintDir;

void main()
{
    
    interp_UV = UV;
    vNormal = normalize(normal);
    paintDir = normalize(paintBallDirection);

    // Computes vertices positions in paint view coordinates.
    gl_Position = paintSpaceMatrix * modelMatrix * vec4(position, 1.0f);
    
    // posizione vertice in coordinate mondo 
    vec4 mPosition = modelMatrix * vec4( position, 1.0 );
    posPaintSpace = paintSpaceMatrix * mPosition;
}