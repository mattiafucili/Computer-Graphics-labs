uniform float time;
attribute  float vx, vy, vz;
const float a = -0.000001;

void main()
{
    
    vec4 t = gl_Vertex;
    t.y = gl_Vertex.y + vy*time + 0.5*a*time*time;
    t.x = gl_Vertex.x + vx*time;
    // RICHIESTA 2
    t.z = gl_Vertex.z + vz*time;
    // FINE
    // RICHIESTA 1
    gl_PointSize = max(t.y * t.z, 1.0);
    // FINE
    gl_Position = gl_ModelViewProjectionMatrix*t;
    gl_FrontColor =  gl_Color;
}
