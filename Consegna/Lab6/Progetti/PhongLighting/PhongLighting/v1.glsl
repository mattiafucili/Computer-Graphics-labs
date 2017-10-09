void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex;
    vec4 eyeLightPos = gl_LightSource[0].position;

    vec3 N = normalize(gl_NormalMatrix * gl_Normal);
    vec3 L = normalize(eyeLightPos.xyz - eyePosition.xyz); // direzione luce
    vec3 E = -normalize(eyePosition.xyz); // direzione camera
    vec3 H = normalize(L + E);

    float Kd = max(dot(N, L), 0.0);
    float Ks = pow(max(dot(H, N), 0.0), gl_FrontMaterial.shininess);
    float Ka = 0.0;

    vec4 ambient  = Ka * gl_FrontLightProduct[0].ambient;
    vec4 diffuse  = Kd * gl_FrontLightProduct[0].diffuse;
    vec4 specular = Ks * gl_FrontLightProduct[0].specular;

    gl_FrontColor = ambient + diffuse + specular;
}
