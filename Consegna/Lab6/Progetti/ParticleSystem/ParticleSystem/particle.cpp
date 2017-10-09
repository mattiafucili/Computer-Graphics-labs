#include "../common/common.h"

#define N 100

GLfloat velocity[N][3];
GLubyte color[N][3];

GLuint program;
GLuint vxParam, vyParam, vzParam, timeParam;

float timeStart = 0.0;

void randomizeParticles()
{
    int i, j;
    for(i=0;i<N;i++)
    {
        velocity[i][0] = 0.001 * 0.5 * ((rand()%256)/256.0-0.5);
        velocity[i][1] = 0.001 * 1.0 * ((rand()%256)/256.0+2.0);
        // RICHIESTA 2
        velocity[i][2] = 0.001 * 1.0 * ((rand()%256)/256.0-1.0);
        // FINE
    }
    
    for(i=0;i<N;i++)
    {
        for(j=0; j<3; j++)
            color[i][j] = rand()%256;
    }
}

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0,0.0,0.0);
    glPointSize(10.0);
    
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(85.0, (double) glutGet(GLUT_WINDOW_WIDTH) / (double) glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (2.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    glEnable(GL_DEPTH_TEST);
    
    // enable changing the point size in the vertex shader via the gl_PointSize variable;
    // RICHIESTA 1
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    // FINE
    program = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/ParticleSystem/ParticleSystem/v.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/ParticleSystem/ParticleSystem/f.glsl");
    timeParam = glGetUniformLocation(program, "time");
    vxParam = glGetAttribLocation(program, "vx");
    vyParam = glGetAttribLocation(program, "vy");
    // RICHIESTA 2
    vzParam = glGetAttribLocation(program, "vz");
    // FINE
    randomizeParticles();
}

static void draw(void)
{
    int i;
    double h = 0.1;
    float time = glutGet(GLUT_ELAPSED_TIME) - timeStart;
    glUniform1f(timeParam, time);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
    for(i=0; i<N; i++)
    {
        glColor3ubv(color[i]);
        glVertexAttrib1f(vxParam, velocity[i][0]);
        glVertexAttrib1f(vyParam, velocity[i][1]);
        // RICHIESTA 2
        glVertexAttrib1f(vzParam, velocity[i][2]);
        // FINE
        glVertex2d(h, h);
        glVertex2d(h, -h);
        glVertex2d(-h, -h);
        glVertex2d(-h, h);
    }
    glEnd();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    commonKeyboard(key, x, y);
    if(key == ' ')
    {
        timeStart = glutGet(GLUT_ELAPSED_TIME);
        randomizeParticles();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    
    init();
    
    glutMainLoop();
    return 0;
}

