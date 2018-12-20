#version 440 core

// posizione vertice in coordinate mondo
layout (location = 0) in vec3 position;
// normale vertice
layout (location = 1) in vec3 normal;
// coordinate texture
layout (location = 2) in vec2 UV;

// matrice di modellazione
uniform mat4 modelMatrix;
// matrice di vista
uniform mat4 viewMatrix;
// matrice di proiezione
uniform mat4 projectionMatrix;

// matrice di trasformazione delle normali (= trasposta dell'inversa della model-view)
uniform mat3 normalMatrix;

// la posizione della point light è passata come uniform
// NB) se ci fossero + luci e di diverso tipo, lo shader dovrebbe essere modificato con un ciclo for, e con diversa considerazione di direzioni, angoli di cutoff per gli spotlight ecc
uniform vec3 pointLightPosition;

// direzione di incidenza della luce (in coordinate vista)
out vec3 lightDir;
// normale (in coordinate vista)
out vec3 vNormal;

// oltre al calcolo di normale e posizione in coordinate vista, deve calcolare 
// anche il vettore di riflessione per il riflesso speculare.
// Per fare questo calcolo nel fragment shader, ho bisogno di passare la direzione di vista in coordinate vista,
// che verrà interpolata dal rasterizzatore per ogni frammento
out vec3 vViewPosition;

out vec2 interp_UV;


void main()
{
    // posizione vertice in coordinate ModelView (vedere ultima riga per il calcolo finale della posizione in coordinate camera)
    vec4 mvPosition = viewMatrix * modelMatrix * vec4( position, 1.0 );

    // calcolo della direzione di vista, negata per avere il verso dal vertice alla camera
    vViewPosition = -mvPosition.xyz;
    // trasformazione coordinate normali in coordinate vista 
    vNormal = normalize( normalMatrix * normal );

    // calcolo del vettore di incidenza della luce.
    vec4 lightPos = viewMatrix  * vec4(pointLightPosition, 1.0);
    lightDir = lightPos.xyz - mvPosition.xyz;

    // calcolo posizione vertici in coordinate vista
    gl_Position = projectionMatrix * mvPosition;

    // assegnando i valori per-vertex delle UV a una variabile con qualificatore "out", 
    // i valori verranno interpolati su tutti i frammenti generati in fase
    // di rasterizzazione tra un vertice e l'altro.
    interp_UV = UV;
}