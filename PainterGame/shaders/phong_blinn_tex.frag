#version 440 core

// Output color of the shader.
out vec4 colorFrag;

// Light incidence vector.
in vec3 lightDir;
// normal in view coordinates.
in vec3 vNormal;
// view position in view coordinates.
in vec3 vViewPosition;

// Interpolation between UVs of vertices.
in vec2 interp_UV;

// Amount of repetitions of the texture.
uniform vec2 repeat;

// The texture to use.
uniform sampler2D tex;
uniform sampler2D normalMap;
uniform float usesNormalMap = 0.0;

// Ambient and specular components.
uniform vec3 diffuseColor;
uniform vec3 ambientColor; 
uniform vec3 specularColor;
uniform float usesTexture = 0.0;

// Weights of components.
uniform float Kd;
uniform float Ka;
uniform float Ks;

// Attenuation parameters.
uniform float constant;
uniform float linear;
uniform float quadratic;

// Shininess coefficient.
uniform float shininess;

// Paint parameters
// The paintmap of the model.
uniform usampler2D paintMap;
// The color of the paint.
uniform vec3 paintColor;
// 1 if the model is paintable, 0 otherwise.
uniform float isPaintable = 0.0;
// The texture that contains the noise.
uniform sampler2D perlinNoise;
// Tha maximum unsigned byte (used for normalization).
const uint max_ubyte = 255;

void main()
{
    // applico la ripetizione delle UV e campiono la texture
    vec2 repeated_Uv = mod(interp_UV*repeat, 1.0);
    vec4 surfaceColor = texture(tex, repeated_Uv);
    float s = shininess;
    if (usesTexture < 1.0)
        // If not using a texture, replace texture color with diffuse color.
        surfaceColor = vec4(diffuseColor, 1.0); 

    float paintAlpha = 0.0;
    float texelSize = 1.0 / textureSize(paintMap, 0).x;
    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
            paintAlpha += float(texture(paintMap, interp_UV + texelSize * vec2(x, y)).r)/max_ubyte; 
    paintAlpha = (9.0 - paintAlpha) / 9.0;
    
    // Consider paint alpha only if the material is paintable.
	paintAlpha *= isPaintable;
    
    // Applies noise.
    float kSpec = Ks;
    
	paintAlpha = clamp(paintAlpha - texture2D(perlinNoise, repeated_Uv).r, 0.0, 1.0);
    if (paintAlpha > 0.21)
    {
        paintAlpha = 1.0;
        // Paint has a greater shininess.
        s = 100.0;
        kSpec = 0.9;
    }
    else
        paintAlpha = 0.0; 

    // Blends surface color with paint color.
    surfaceColor = surfaceColor * (1.0 - paintAlpha) + paintAlpha * vec4(paintColor, 1.0);

    // Computes ambiental component.
    vec4 color = vec4(Ka*ambientColor,1.0);

    // If found, uses a normal map instead of vertex normal.
    vec3 N = normalize(vNormal);
    if (usesNormalMap > 0.0)
        N = normalize(texture2D(normalMap, repeated_Uv).rgb * 2.0 - 1.0);

    vec3 L = normalize(lightDir.xyz);
    float distanceL = length(L);

    float attenuation = 1.0/(constant + linear*distanceL + quadratic*(distanceL*distanceL));

    // Computes lambertian coefficient.
    float lambertian = max(dot(L,N), 0.0);

    // Computes specular component only if the lambertian coefficient is positive.
    if(lambertian > 0.0) 
    {
        // Normalizes view vector.
        vec3 V = normalize( vViewPosition );

        // Computes halved direction between light and view directions.
        vec3 H = normalize(L + V);

        float specAngle = max(dot(H, N), 0.0);
        // Applies shininess.
        float specular = pow(specAngle, s);

        // Adds diffusive and specular components.
        color += Kd * lambertian * surfaceColor +
                        vec4(kSpec * specular * specularColor,1.0);
        color*=attenuation;
    }

    colorFrag  = color;
}