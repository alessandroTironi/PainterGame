#version 330 core
out vec4 outColor;

in vec4 colorFrag;

void main()
{    
    outColor = colorFrag;
}