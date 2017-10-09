attribute vec4 vertices2;
attribute vec4 color;

attribute vec4 verticesQuads;
attribute vec4 colorQuads;

attribute vec4 verticesFTS;
attribute vec4 colorFTS;

uniform float time;

void main() {
    float s =  0.5 * (1.0 + sin(0.001 * time));
    
    // RICHIESTA 1
    // Triangolo
//    vec4 t = mix(gl_Vertex, vertices2, s); // interpola a partire da gl_Vertex fino a vertices2 con parametro s
//    gl_Position = gl_ModelViewProjectionMatrix * t;
//    gl_FrontColor = mix(gl_Color, color, s);
    
    // Quadrato
//    vec4 t = mix(gl_Vertex, verticesQuads, s);
//    gl_Position = gl_ModelViewProjectionMatrix * t;
//    gl_FrontColor = mix(gl_Color, colorQuads, s);
    // FINE
    
    // RICHIESTA 2
    // Da triangolo a quadrato
    vec4 t = mix(gl_Vertex, verticesFTS, s);
    gl_Position = gl_ModelViewProjectionMatrix * t;
    gl_FrontColor = mix(gl_Color, colorFTS, s);
    // FINE
}
