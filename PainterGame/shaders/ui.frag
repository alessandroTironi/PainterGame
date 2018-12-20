#version 440 core

out vec4 fragColor;

uniform sampler2D scene;
uniform sampler2D ui;

in vec2 TexCoords;

void main()
{
    vec4 uiColor = texture2D(ui, TexCoords);
    vec4 sceneColor = texture2D(scene, TexCoords);
    if (uiColor.r > 0 && uiColor.g > 0 && uiColor.b > 0)
        fragColor = uiColor;
    else
        fragColor = sceneColor;
}