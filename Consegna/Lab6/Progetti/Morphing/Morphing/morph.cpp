#include "../common/common.h"

GLuint program;
GLint timeParam;
GLint vertices_front_location;
GLint color;

GLint verticesQuads;
GLint colorQuads;

GLint verticesFTS;
GLint colorFTS;

const GLfloat vertices_front[3][2] = {{0.0, 1.0},{0.5, 0.0},{1.0, 1.0}}; // triangolo di partenza
const GLfloat vertices_front_colors[3][3] = {{1.0, 0.0, 0.0}, {0.0, 0.7, 0.0}, {0.0, 0.0, 1.0}}; // colore - rosso, verde scuro, blu

const GLfloat vertices_flip[3][2] = {{0.0, 0.0},{0.5, 1.0},{1.0, 0.0}}; // triangolo girato
const GLfloat vertices_flip_colors[3][3] = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 1.0}}; // colore - giallo, verde chiaro, fucsia

const GLfloat vertices_front_quads[4][3] = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}}; // quadrato di partenza
const GLfloat vertices_front_quads_colors[4][3] = {{0.0, 0.7, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.7, 0.0}, {0.0, 0.0, 1.0}}; // colore - rosso, verde scuro, blu

const GLfloat vertices_flip_quads[4][3] = {{0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}}; // quadrato girato
const GLfloat vertices_flip_quads_colors[4][3] = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 1.0}, {0.0, 1.0, 0.0}}; // colore - giallo, verde chiaro, fucsia

// FTS = From Triangle to Square
const GLfloat vertices_triang_FTS[4][3] = {{0.0, 0.0}, {1.0, 0.0}, {0.5, 1.0}, {0.5, 1.0}}; // triangolo di partenza
const GLfloat vertices_triang_FTS_colors[4][3] = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 1.0}, {0.0, 1.0, 0.0}}; // colore - rosso, verde scuro, blu

const GLfloat vertices_quad_FTS[4][3] = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}}; // quadrato di arrivo
const GLfloat vertices_quad_FTS_colors[4][3] = {{1.0, 0.0, 0.0}, {0.0, 0.7, 0.0}, {0.0, 0.7, 0.0}, {0.0, 0.0, 1.0}}; // colore - giallo, verde chiaro, fucsia

static void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0,0.0,0.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);
    
    glEnable(GL_DEPTH_TEST);
    
    program = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/Morphing/Morphing/v.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/Morphing/Morphing/f.glsl");
    
    // Setup uniform and attribute prameters
    timeParam = glGetUniformLocation(program, "time");
    vertices_front_location = glGetAttribLocation(program, "vertices2");

    color = glGetAttribLocation(program, "color");
    
    verticesQuads = glGetAttribLocation(program, "verticesQuads");
    colorQuads = glGetAttribLocation(program, "colorQuads");
    
    verticesFTS = glGetAttribLocation(program, "verticesFTS");
    colorFTS = glGetAttribLocation(program, "colorFTS");
}

static void draw(void) {
    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // RICHIESTA 1
    // Triangolo
//    glBegin(GL_TRIANGLES);
//        glColor3f(vertices_front_colors[0][0], vertices_front_colors[0][1], vertices_front_colors[0][2]); // colore front
//        glVertexAttrib2fv(vertices_front_location, &vertices_front[0][0]); // vertice front
//        glVertexAttrib3fv(color, &vertices_flip_colors[0][0]); // colore girato
//        glVertex2fv(vertices_flip[0]); // vertice girato
//
//        glColor3f(vertices_front_colors[1][0], vertices_front_colors[1][1], vertices_front_colors[1][2]);
//        glVertexAttrib2fv(vertices_front_location, &vertices_front[1][0]);
//        glVertexAttrib3fv(color, &vertices_flip_colors[1][0]);
//        glVertex2fv(vertices_flip[1]);
//
//        glColor3f(vertices_front_colors[2][0], vertices_front_colors[2][1], vertices_front_colors[2][2]);
//        glVertexAttrib2fv(vertices_front_location, &vertices_front[2][0]);
//        glVertexAttrib3fv(color, &vertices_flip_colors[2][0]);
//        glVertex2fv(vertices_flip[2]);
//    glEnd();
    
    // Quadrato
//    glBegin(GL_QUADS);
//        glColor3f(vertices_front_quads_colors[0][0], vertices_front_quads_colors[0][1], vertices_front_quads_colors[0][2]); // colore front
//        glVertexAttrib2fv(verticesQuads, &vertices_front_quads[0][0]); // vertice front
//        glVertexAttrib3fv(colorQuads, &vertices_flip_quads_colors[0][0]); // colore girato
//        glVertex2fv(vertices_flip_quads[0]); // vertice girato
//
//        glColor3f(vertices_front_quads_colors[1][0], vertices_front_quads_colors[1][1], vertices_front_quads_colors[1][2]);
//        glVertexAttrib2fv(verticesQuads, &vertices_front_quads[1][0]);
//        glVertexAttrib3fv(colorQuads, &vertices_flip_quads_colors[1][0]);
//        glVertex2fv(vertices_flip_quads[1]);
//
//        glColor3f(vertices_front_quads_colors[2][0], vertices_front_quads_colors[2][1], vertices_front_quads_colors[2][2]);
//        glVertexAttrib2fv(verticesQuads, &vertices_front_quads[2][0]);
//        glVertexAttrib3fv(colorQuads, &vertices_flip_quads_colors[2][0]);
//        glVertex2fv(vertices_flip_quads[2]);
//
//        glColor3f(vertices_front_quads_colors[3][0], vertices_front_quads_colors[3][1], vertices_front_quads_colors[3][2]);
//        glVertexAttrib2fv(verticesQuads, &vertices_front_quads[3][0]);
//        glVertexAttrib3fv(colorQuads, &vertices_flip_quads_colors[3][0]);
//        glVertex2fv(vertices_flip_quads[3]);
//        glEnd();
    // FINE
    
    // RICHIESTA 2
    // Da triangolo a quadrato
    glBegin(GL_QUADS);
        glColor3f(vertices_triang_FTS_colors[0][0], vertices_triang_FTS_colors[0][1], vertices_triang_FTS_colors[0][2]); // colore triangolo
        glVertexAttrib2fv(verticesFTS, &vertices_triang_FTS[0][0]); // vertice triangolo
        glVertexAttrib3fv(colorFTS, &vertices_quad_FTS_colors[0][0]); // colore quadrato
        glVertex2fv(vertices_quad_FTS[0]); // vertice quadrato
    
        glColor3f(vertices_triang_FTS_colors[1][0], vertices_triang_FTS_colors[1][1], vertices_triang_FTS_colors[1][2]);
        glVertexAttrib2fv(verticesFTS, &vertices_triang_FTS[1][0]);
        glVertexAttrib3fv(colorFTS, &vertices_quad_FTS_colors[1][0]);
        glVertex2fv(vertices_quad_FTS[1]);
    
        glColor3f(vertices_triang_FTS_colors[2][0], vertices_triang_FTS_colors[2][1], vertices_triang_FTS_colors[2][2]);
        glVertexAttrib2fv(verticesFTS, &vertices_triang_FTS[2][0]);
        glVertexAttrib3fv(colorFTS, &vertices_quad_FTS_colors[2][0]);
        glVertex2fv(vertices_quad_FTS[2]);
    
        glColor3f(vertices_triang_FTS_colors[3][0], vertices_triang_FTS_colors[3][1], vertices_triang_FTS_colors[3][2]);
        glVertexAttrib2fv(verticesFTS, &vertices_triang_FTS[3][0]);
        glVertexAttrib3fv(colorFTS, &vertices_quad_FTS_colors[3][0]);
        glVertex2fv(vertices_quad_FTS[3]);
    glEnd();
    // FINE
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);
    
    init();
    
    glutMainLoop();
    return 0;
}

