#include "../common/common.h"

GLuint program[3];

static void init()
{
    const float teapotColor[]     = {0.0f, 0.3f, 0.6f, 1.0f};
    const float teapotSpecular[]  = {0.5f, 1.0f, 1.0f, 1.0f};
    const float teapotShininess[] = {80.0f}; // alpha nella formula di Phong
    
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, teapotColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, teapotSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, teapotShininess);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double) glutGet(GLUT_WINDOW_WIDTH) / (double) glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    const GLfloat  lightPos[4] = {2.0f, 3.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    program[0] = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/v1.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/f1.glsl");
    program[1] = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/v2.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/f2.glsl");
    // RICHIESTA 1
    program[2] = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/v3.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/PhongLighting/PhongLighting/f3.glsl");
    // FINE
}

static void draw(void)
{
    double t = (double)glutGet(GLUT_ELAPSED_TIME);
    double k = 0.05 * 360.0/1000.0;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // RICHIESTA 2
    glUseProgram(program[0]);
    glPushMatrix();
    glTranslatef(-1.9f, 0.0f, -5.0f);
    glRotatef(k*t, 1.0, 0.0, 0.0);
    glRotatef(k*t, 0.0, 1.0, 0.0);
    glutSolidTeapot(0.6);
    glPopMatrix();
    
    glUseProgram(program[1]);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(k*t, 1.0, 0.0, 0.0);
    glRotatef(k*t, 0.0, 1.0, 0.0);
    glutSolidTeapot(0.6);
    glPopMatrix();
    
    glUseProgram(program[2]);
    glPushMatrix();
    glTranslatef(1.9f, 0.0f, -5.0f);
    glRotatef(k*t, 1.0, 0.0, 0.0);
    glRotatef(k*t, 0.0, 1.0, 0.0);
    glutSolidTeapot(0.6);
    glPopMatrix();
    // FINE
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    // Uso il carattere esc per chiudere
    commonKeyboard(key, x, y);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    // RICHIESTA 2
    glutInitWindowSize(1000, 700);
    // FINE
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    
    init();
    
    glutMainLoop();
    return 0;
}

