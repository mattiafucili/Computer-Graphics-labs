varying vec3 N;
varying vec3 L;
varying vec3 E;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex; // punto nell'oggetto espresso nel sistema della camera
    vec4 eyeLightPos = gl_LightSource[0].position;

    N = normalize(gl_NormalMatrix * gl_Normal);
    L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    
    // RICHIESTA
    vec4 zeroCamera = vec4(0.0);
    E = normalize(zeroCamera.xyz - eyePosition.xyz);
    // FINE
}
