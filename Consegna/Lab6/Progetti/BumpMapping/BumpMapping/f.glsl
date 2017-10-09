varying vec3 L;
varying vec3 V;
uniform sampler2D texMap;

void main() {
    vec4 N = texture2D(texMap, gl_TexCoord[0].st);
    vec3 NN =  normalize(2.0 * N.xyz - 1.0); // passaggio inverso da [0, 1] a [-1, 1]
    vec3 NL = normalize(L);
    float Kd = max(dot(NN, NL), 0.0);
    gl_FragColor = Kd*gl_FrontLightProduct[0].diffuse;
}
