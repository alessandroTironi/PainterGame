#version 440 core
#extension GL_EXT_shader_image_load_store : enable

in vec2 interp_UV;

// Normal of the fragment.
in vec3 vNormal;

// Direction of the paintball.
in vec3 paintDir;

// Position in paint projection space.
in vec4 posPaintSpace;

// The previous state of the paint map.
uniform layout(binding = 3, r8ui) uimage2D previous_paint_map;

// The size of the paint map.
uniform int paint_map_size;

// The texture that represents the stain.
uniform sampler2D stainTex;

// The maximum unsigned byte (used for normalization).
const uint max_ubyte = 255;

void main()
{
    // Retrieves color on the previous version of the paint map.
    ivec2 uv_pixels = ivec2(interp_UV * paint_map_size);
    uv_pixels.x = clamp(uv_pixels.x, 0, paint_map_size - 1);
    uv_pixels.y = clamp(uv_pixels.y, 0, paint_map_size - 1);
    uint previous_paint_color = imageLoad(previous_paint_map, uv_pixels).r;

    // Retrieves paint color added to the fragment with the current splat.
    vec3 projCoords = posPaintSpace.xyz / posPaintSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    uint addedColor = uint(max_ubyte * gl_FragDepth);

    // Computes incidence angle between paint ball direction and face normal.
    float paintLevel = 1.0 - texture2D(stainTex, projCoords.xy).r;
    float incidence = dot(paintDir, vNormal);

    // If dot product < 0 then the face got hit by the paint.
    if (incidence < 0 && paintLevel > 0)
    {
        // Computes new paint color value.
        uint paint = min(previous_paint_color, addedColor);

        imageStore(previous_paint_map, uv_pixels, uvec4(paint));
    }
}