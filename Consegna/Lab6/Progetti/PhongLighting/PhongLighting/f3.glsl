varying vec3 N;
varying vec3 L;
varying vec3 E;
varying vec3 R;

void main() {
    vec3 Normal = normalize(N);
    vec3 Light  = normalize(L);
    vec3 Eye    = normalize(E);
    vec3 Refl   = normalize(R);

    float Kd = max(dot(Normal, Light), 0.0);
    // RICHIESTA 1
    float Ks = pow(max(dot(Refl, Eye), 0.0), gl_FrontMaterial.shininess); // qui ora abbiamo V ed R come nella formula
    // FINE
    float Ka = 0.0;

    vec4 ambient  = Ka * gl_FrontLightProduct[0].ambient;
    vec4 diffuse  = Kd * gl_FrontLightProduct[0].diffuse;
    vec4 specular = Ks * gl_FrontLightProduct[0].specular;

    gl_FragColor = ambient + diffuse + specular;
}
