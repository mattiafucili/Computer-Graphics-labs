varying vec3 N;
varying vec3 L;
varying vec3 E;
varying vec3 R;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex;
    vec4 eyeLightPos = gl_LightSource[0].position;

    N = normalize(gl_NormalMatrix * gl_Normal);
    L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    E = -normalize(eyePosition.xyz);
    // RICHIESTA 1
	R = -reflect(L, N);
    // FINE
}
