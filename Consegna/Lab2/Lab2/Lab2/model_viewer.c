#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "v3d.h"

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#define MAX_V 10000 /* max number of vertices allowed in the mesh model */
#define N_MESH 3

int listname[N_MESH];

int wireframe;
int orpro;
int cull;
int mater;
int shading;

GLfloat fovy;
GLfloat angle[3];

//Parte3 e 4
float matrix[N_MESH][16] = {{0.64, 0.76, 0.0, 0.0, -0.76, 0.64, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.3, 1.4, 2.2, 1.0},
                           {1.0, 0.0, 0.0, 0.0, 0.0, 0.64, 0.76, 0.0, 0.0, -0.76, 0.64, 0.0, 0.5, 1.3, 0.1, 1.0},
                           {0.64, 0.0, -0.76, 0.0, 0.0, 1.0, 0.0, 0.0, 0.76, 0.0, 0.64, 0.0, 0.3, 0.4, 0.2, 1.0}};

float matrixWCS[N_MESH][16] = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
                                                                                                                                                                                                  
float matrixOCS[N_MESH][16] = {{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                              {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};

//fine parte3 e 4

GLfloat camC[3];
GLfloat camE[3];
GLfloat camU[3];
GLfloat lightPos[4];

GLfloat brass_ambient[] = {0.33, 0.22, 0.03, 1.0}, brass_diffuse[] = {0.78, 0.57, 0.11, 1.0}, brass_specular[] = {0.99, 0.91, 0.81, 1.0}, brass_shininess[] = {27.8};
GLfloat red_plastic_ambient[] = {0.0, 0.0, 0.0}, red_plastic_diffuse[] = {0.5, 0.0, 0.0}, red_plastic_specular[] = {0.7, 0.6, 0.6}, red_plastic_shininess[] = {32.0};
GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215}, emerald_diffuse[] = {0.07568, 0.61424, 0.07568}, emerald_specular[] = {0.633, 0.727811, 0.633}, emerald_shininess[] = {76.8};
GLfloat slate_ambient[] = {0.02, 0.02, 0.02}, slate_diffuse[] = {0.02, 0.01, 0.01}, slate_specular[] = {0.4, 0.4, 0.4}, slate_shininess[] = {.78125};

GLfloat aspect = 1.0;

GLUquadricObj* myReusableQuadric = 0;

float tbAngle = 0.0;
float tbAxis[3];

int tbDragging = 0;
float tbV[3];
float tbW[3];

int indexMotion = 0;
int flagMotion = 0;
int numCV = 7;
void deCasteljau(float t, float* result);
float CV[7][3] = { {0, 20, -10}, {10, 20, -7}, {10, 20, 7}, {0, 20, 10}, {-10, 20, 7}, {-10, 20, -7}, {0, 20, -10} };

int selectedMesh = -1;

typedef struct mesh {
    int 	faces[MAX_V][3];    /* faces */
    float 	vertices[MAX_V][3]; /* vertices */
    float 	vnormals[MAX_V][3]; /* vertex normals */
    float   fnormals[MAX_V][3]; /* face normals */
    int nVert, nFaces;
} t_mesh;

enum Modes {
	MODE_INVALID,
	MODE_CHANGE_EYE_POS,
	MODE_CHANGE_REFERENCE_POS,
	MODE_CHANGE_UP_POS,
	MODE_CHANGE_LIGHT_POS,
	MODE_CHANGE_ZOOM,
	MODE_ROTATE_MODEL,

	MODE_CHANGE_CULLING,
	MODE_CHANGE_WIREFRAME,
	MODE_CHANGE_SHADING,
	MODE_CHANGE_PROJECTION,
	MODE_CHANGE_MATERIAL,

	MODE_TRANSLATE_WCS,
	MODE_ROTATE_WCS,
	MODE_TRANSLATE_OCS,
	MODE_ROTATE_OCS,
	MODE_TRANSLATE_VCS,
	MODE_ROTATE_VCS,

	MODE_PRINT_SYSTEM_STATUS,
	MODE_RESET,
	MODE_QUIT
};

enum Modes mode = MODE_CHANGE_REFERENCE_POS;

int WindowWidth = 700;
int WindowHeight = 700;

void drawAxis(float scale, int drawLetters) {
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glScalef(scale, scale, scale);
	glBegin(GL_LINES);

	glColor4d(1.0, 0.0, 0.0, 1.0);
	if(drawLetters) {
		glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 );
		glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 );


	glColor4d( 0.0, 1.0, 0.0, 1.0 );
	if( drawLetters ) {
		glVertex3f( 0.10f, 0.8f, 0.0 );   glVertex3f( 0.10f, 0.90f, 0.0 );
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.05, 1.0, 0.0 );
		glVertex3f( 0.10f, 0.90f, 0.0 );  glVertex3f( 0.15, 1.0, 0.0 );
	}
	glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 );


	glColor4d( 0.0, 0.0, 1.0, 1.0 );
	if( drawLetters ) {
		glVertex3f( 0.05f, 0, 0.8f );  glVertex3f( 0.20f, 0, 0.8f );
		glVertex3f( 0.20f, 0, 0.8f );  glVertex3f( 0.05, 0, 1.0 );
		glVertex3f( 0.05f, 0, 1.0 );   glVertex3f( 0.20, 0, 1.0 );
	}
    glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 );

	glEnd();
	glPopMatrix();

	glEnable( GL_LIGHTING );
}

void computePointOnTrackball( int x, int y, float p[3] ) {
	float zTemp;
	//map to [-1;1]
	p[0] = (2.0f * x - WindowWidth)  / WindowWidth;
	p[1] = (WindowHeight - 2.0f * y) / WindowHeight;

	zTemp = 1.0f - (p[0]*p[0]) - (p[1]*p[1]);
	p[2] = (zTemp > 0.0f ) ? sqrt(zTemp) : 0.0;
}

void mouse( int button, int state, int x, int y ) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		tbDragging = 1;
		computePointOnTrackball(x, y, tbV);
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		tbDragging = 0;

}

void motion( int x, int y ) {
	float dx, dy, dz;

	computePointOnTrackball(x, y, tbW);
	if(tbDragging) {
		dx = tbV[0] - tbW[0];
		dy = tbV[1] - tbW[1];
		dz = tbV[2] - tbW[2];
		if (dx || dy || dz) {
			tbAngle += sqrt(dx*dx + dy*dy + dz*dz) * (180.0 / M_PI);
			tbAxis[0] += tbW[1]*tbV[2] - tbW[2]*tbV[1];
			tbAxis[1] += tbW[2]*tbV[0] - tbW[0]*tbV[2];
			tbAxis[2] += tbW[0]*tbV[1] - tbW[1]*tbV[0];
            
            tbV[0] = tbW[0];
            tbV[1] = tbW[1];
            tbV[2] = tbW[2];
		}
	}
	glutPostRedisplay();
}

void passiveMotion( int x, int y ){ }

void special( int key, int x, int y ) { }

void keyboard (unsigned char key, int x, int y) {
	float* pos = NULL;
	float step = 0;

    if( mode == MODE_CHANGE_EYE_POS ) {
        pos = camE;
        step = 0.1;
	} else if( mode == MODE_CHANGE_REFERENCE_POS ) {
		pos = camC;
		step = 0.1;
	} else if( mode == MODE_CHANGE_UP_POS ) {
		pos = camU;
		step = 0.1;
	} else if( mode == MODE_CHANGE_LIGHT_POS ) {
		pos = lightPos;
		step = 1.0;
	} else if( mode == MODE_ROTATE_MODEL ) {
        pos = angle;
        step = 1.0;
	} else if(mode == MODE_CHANGE_ZOOM){
        pos = &fovy;
        step = 1.0;
    } else if(mode == MODE_ROTATE_WCS) {
        glPushMatrix();
        glLoadIdentity();
        step = 5.0;
        if(key == 'x')
            glRotatef(step, 1, 0, 0);
        else if(key == 'X')
            glRotatef(-step, 1, 0, 0);
        else if(key == 'y')
            glRotatef(step, 0, 1, 0);
        else if(key == 'Y')
            glRotatef(-step, 0, 1, 0);
        else if(key == 'z')
            glRotatef(step, 0, 0, 1);
        else if(key == 'Z')
            glRotatef(-step, 0, 0, 1);

        glMultMatrixf(matrixWCS[selectedMesh]);
        glGetFloatv(GL_MODELVIEW, matrixWCS[selectedMesh]);
        glPopMatrix();
    } else if(mode == MODE_TRANSLATE_WCS) {
        glPushMatrix();
        glLoadIdentity();
        step = 0.5;
        if(key == 'x')
            glTranslatef(step, 0, 0);
        else if(key == 'X')
            glTranslatef(-step, 0, 0);
        else if(key == 'y')
            glTranslatef(0, step, 0);
        else if(key == 'Y')
            glTranslatef(0, -step, 0);
        else if(key == 'z')
            glTranslatef(0, 0, step);
        else if(key == 'Z')
            glTranslatef(0, 0, -step);

        glMultMatrixf(matrixWCS[selectedMesh]);
        glGetFloatv(GL_MODELVIEW, matrixWCS[selectedMesh]);
        glPopMatrix();
    } else if(mode == MODE_ROTATE_OCS) {
        glPushMatrix();
        glLoadIdentity();
        step = 5.0;
        if(key == 'x')
            glRotatef(step, 1, 0, 0);
        else if(key == 'X')
            glRotatef(-step, 1, 0, 0);
        else if(key == 'y')
            glRotatef(step, 0, 1, 0);
        else if(key == 'Y')
            glRotatef(-step, 0, 1, 0);
        else if(key == 'z')
            glRotatef(step, 0, 0, 1);
        else if(key == 'Z')
            glRotatef(-step, 0, 0, 1);

        glMultMatrixf(matrixOCS[selectedMesh]);
        glGetFloatv(GL_MODELVIEW, matrixOCS[selectedMesh]);
        glPopMatrix();
    } else if(mode == MODE_TRANSLATE_OCS) {
        glPushMatrix();
        glLoadIdentity();
        step = 0.5;
        if(key == 'x')
            glTranslatef(step, 0, 0);
        else if(key == 'X')
            glTranslatef(-step, 0, 0);
        else if(key == 'y')
            glTranslatef(0, step, 0);
        else if(key == 'Y')
            glTranslatef(0, -step, 0);
        else if(key == 'z')
            glTranslatef(0, 0, step);
        else if(key == 'Z')
            glTranslatef(0, 0, -step);

        glMultMatrixf(matrixOCS[selectedMesh]);
        glGetFloatv(GL_MODELVIEW, matrixOCS[selectedMesh]);
        glPopMatrix();
    }
    glutPostRedisplay();
	
	if(pos != NULL) {
		if(key == 'x')
            pos[0] += step;
		else if( key == 'X' )
            pos[0] -= step;
		else if( key == 'y' )
            pos[1] += step;
		else if( key == 'Y' )
            pos[1] -= step;
		else if( key == 'z' )
            pos[2] += step;
		else if( key == 'Z' )
            pos[2] -= step;
        else if( key == 'f')
            *pos += step;
        else if( key == 'F')
            *pos -= step;
        
		glutPostRedisplay();
	}

	if( key ==  27 ) //esc
		exit(1);
    
    if(key == 's'){
        flagMotion = !flagMotion;
        indexMotion = 0;
    }
    
    if(key == 'w')
        mode = MODE_TRANSLATE_WCS;
    else if(key == 'W')
        mode = MODE_ROTATE_WCS;
    else if(key == 'o')
        mode = MODE_TRANSLATE_OCS;
    else if(key == 'O')
        mode = MODE_ROTATE_OCS;
    else if(key == '1')
        selectedMesh = 0;
    else if(key == '2')
        selectedMesh = 1;
    else if(key == '3')
        selectedMesh = 2;
}

void idle() {
    float t;
    float result[3];
    
    if(flagMotion) {
        t = (GLfloat) indexMotion/100;
        deCasteljau(t, &result[0]);
        camE[0] = result[0];
        camE[1] = result[1];
        camE[2] = result[2];
        
        indexMotion++;
        if(indexMotion == 100)
            indexMotion = 0;
        
        glutPostRedisplay();
    }
}

void deCasteljau(float t, float* result) {
    float coordX[numCV], coordY[numCV], coordZ[numCV];
    
    for(int i = 0; i < numCV; i++){
        coordX[i] = CV[i][0];
        coordY[i] = CV[i][1];
        coordZ[i] = CV[i][2];
    }
    
    for(int i = 1; i < numCV; i++){
        for(int k = 0; k < numCV - i; k++){
            coordX[k] = (1-t) * coordX[k] + (t) * coordX[k+1];
            coordY[k] = (1-t) * coordY[k] + (t) * coordY[k+1];
            coordZ[k] = (1-t) * coordZ[k] + (t) * coordZ[k+1];
        }
    }
    result[0] = coordX[0];
    result[1] = coordY[0];
    result[2] = coordZ[0];
}


void drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks ) {
	// First draw the cylinder
	gluCylinder( myReusableQuadric, radiusBase, radiusTop, height, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glPopMatrix();

	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();

}

void display() {
    if (mater == 0) { //ottone
		glLightfv(GL_LIGHT0, GL_AMBIENT, brass_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brass_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, brass_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, brass_shininess);
	}
    if (mater == 1) { //plastica rossa
		glLightfv(GL_LIGHT0, GL_AMBIENT, red_plastic_ambient);
   		glMaterialfv(GL_FRONT, GL_DIFFUSE, red_plastic_diffuse);
   		glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
   		glMaterialfv(GL_FRONT, GL_SHININESS, red_plastic_shininess);
	}
   	if (mater == 2) { //smeraldo
		glLightfv(GL_LIGHT0, GL_AMBIENT, emerald_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, emerald_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, emerald_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, emerald_shininess);
	}
    
    if (mater == 3) { //slate
		glLightfv(GL_LIGHT0, GL_AMBIENT, slate_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, slate_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, slate_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, slate_shininess);
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if(shading)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);
    
    if (cull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

	glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity ();
    
    if(orpro)
        gluPerspective(fovy, aspect, 1, 100);
    else
        glOrtho(-1.0, 1.0, -1.0, 1.0, -10, 100);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(camE[0], camE[1], camE[2], camC[0], camC[1], camC[2], camU[0], camU[1], camU[2]);

	glRotatef(tbAngle, tbAxis[0], tbAxis[1], tbAxis[2]);
    
    glLineWidth(2);
    drawAxis(2.0, 1);
    glLineWidth(1);
    
	glRotatef(angle[0], 1.0, 0.0, 0.0);
	glRotatef(angle[1], 0.0, 1.0, 0.0);
	glRotatef(angle[2], 0.0, 0.0, 1.0);
    
    for(int i = 0; i < N_MESH; i++) {
        glPushMatrix();
        glMultMatrixf(matrixWCS[i]);
        glMultMatrixf(matrix[i]);
        drawAxis(1.0, 0);
        glMultMatrixf(matrixOCS[i]);
        glCallList(listname[i]);
        glPopMatrix();
    }
    
    glTranslated(0.0, 0.0, 1.5);
    gluCylinder(myReusableQuadric, 0.5, 0.2, 0.5, 12, 12);
    //	gluSphere(myReusableQuadric, 1.0, 12, 12);

	glutSwapBuffers();
}

void reset() {
	angle[0] = 0.0;
	angle[1] = 0.0;
	angle[2] = 0.0;

    camE[0] = 8.8;
    camE[1] = 4.9;
    camE[2] = 9.0;

//In caso di ortogonale decommentare
//    camE[0] = 9.0;
//    camE[1] = 0.0;
//    camE[2] = 0.0;

	camC[0] = 0.0;
	camC[1] = 0.0;
	camC[2] = 0.0;

	camU[0] = 0.0;
	camU[1] = 1.0;
	camU[2] = 0.0;

	lightPos[0] =  5.0;
	lightPos[1] =  5.0;
	lightPos[2] =  5.0;
	lightPos[3] =  1.0;

	fovy = 20;
	wireframe = 1;
	cull = 0;
	mater = 1;
	orpro = 1;
	shading = 0;

	glutPositionWindow(10,10);
	glutReshapeWindow(700,700);
	glutPostRedisplay();
    
    tbAngle = 0.0;
    tbAxis[0] = 0.0;
    tbAxis[1] = 0.0;
    tbAxis[2] = 0.0;
    
    selectedMesh = 0;
    
    for(int i = 0; i < N_MESH; i++) {
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW, matrixWCS[0]);
        glPopMatrix();
    
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW, matrixOCS[0]);
        glPopMatrix();
    }
}

void loadMesh(char* path, int faces[MAX_V][3], int *nFaces, float vertices[MAX_V][3], int *nVertices, float vnormals[MAX_V][3], float fnormals[MAX_V][3]) {
    FILE* id;
    char s[10];
    int n;
    float a,b,c;

    if((id = fopen(path, "r")) == NULL) {
        perror("file non trovato\n");
        exit (1);
    }
    
    *nFaces = 0;
    *nVertices = 0;
    
    while(!feof(id)) {
        fscanf(id,"%s %d %f %f %f",s, &n, &a,&b,&c);
        switch(s[0]) {
            case 'V':
                vertices[*nVertices][0] = a;
                vertices[*nVertices][1] = b;
                vertices[(*nVertices)++][2] = c;
                break;
                
            case 'N':
                printf(" Vertex normals not used! \n");
                break;
                
            case 'F':
                faces[*nFaces][0] = (int)a - 1;
                faces[*nFaces][1] = (int)b - 1;
                faces[*nFaces][2] = (int)c - 1;
                
                float vA[3], vB[3], vAB[3];
                v3dSub(vertices[(int) b - 1], vertices[(int)a - 1], vA);
                v3dSub(vertices[(int) c - 1], vertices[(int)a - 1], vB);
                
                v3dCross(vA, vB, vAB);
                
                v3dNormalize(vAB);
                
                fnormals[*nFaces][0] = vAB[0];
                fnormals[*nFaces][1] = vAB[1];
                fnormals[(*nFaces)++][2] = vAB[2];
                
                vnormals[(int)a - 1][0] += vAB[0];
                vnormals[(int)a - 1][1] += vAB[1];
                vnormals[(int)a - 1][2] += vAB[2];
                
                vnormals[(int)b - 1][0] += vAB[0];
                vnormals[(int)b - 1][1] += vAB[1];
                vnormals[(int)b - 1][2] += vAB[2];
                
                vnormals[(int)c - 1][0] += vAB[0];
                vnormals[(int)c - 1][1] += vAB[1];
                vnormals[(int)c - 1][2] += vAB[2];
                break;
                
            case 'E':
                printf(" Edge not used! \n");
                break;
                
            default:
                printf(" ERROR: Reading file \n");
                break;
        }
    }
    
    for(int i = 0; i < *nVertices; i++)
        v3dNormalize(vnormals[i]);
        
    for(int i = 0; i < *nFaces; i++)
        v3dNormalize(fnormals[i]);
    
    fclose(id);
}

void init() {
    
    int ids[3];
    
    t_mesh mesh[N_MESH];
    
    char* path[N_MESH];
    
    path[0] = "/Università/Magistrale/Fondamenti di CG/Lab/Lab 2/Lab2/data/pig.m";
    path[1] = "/Università/Magistrale/Fondamenti di CG/Lab/Lab 2/Lab2/data/cactus.m";
    path[2] = "/Università/Magistrale/Fondamenti di CG/Lab/Lab 2/Lab2/data/pig.m";
    
    for(int i = 0; i < N_MESH; i++) {
        loadMesh(path[i], &mesh[i].faces[0], &mesh[i].nFaces, &mesh[i].vertices[0], &mesh[i].nVert, &mesh[i].vnormals[0], &mesh[i].fnormals[0]);
        
        // crea la display list mesh
        printf("Creazione display list .. \n");
        listname[i] = glGenLists(1);  //ID della lista e' listname
        glNewList(listname[i],GL_COMPILE);
        
        for(int j = 0; j < mesh[i].nFaces; j++ ) {
            ids[2] = mesh[i].faces[j][0];
            ids[1] = mesh[i].faces[j][1];
            ids[0] = mesh[i].faces[j][2];
            
            //disegna triangoli coi vertici specificati
            glBegin( GL_TRIANGLES );
            glColor3f(1,0,0);
            for(int j = 2; j >= 0; j--) {
                glNormal3f( mesh[i].vnormals[ids[j]][0], mesh[i].vnormals[ids[j]][1], mesh[i].vnormals[ids[j]][2] );
                glVertex3f( mesh[i].vertices[ids[j]][0], mesh[i].vertices[ids[j]][1], mesh[i].vertices[ids[j]][2] );
            }
            glEnd();
        }
        
        glEndList();
    }

	myReusableQuadric = gluNewQuadric();  
	gluQuadricNormals( myReusableQuadric, GLU_SMOOTH );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void print_sys_status() {
	system("clear");
	printf("\nSystem status\n\n");
	printf("---------------------------------------------------\n");
	printf("Eye point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camE[0], camE[1], camE[2] );
	printf("---------------------------------------------------\n");
	printf("Reference point coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camC[0], camC[1], camC[2] );
	printf("---------------------------------------------------\n");
	printf("Up vector coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",camU[0], camU[1], camU[2] );
	printf("---------------------------------------------------\n");
	printf("Light position coordinates :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",lightPos[0], lightPos[1], lightPos[2] );
	printf("---------------------------------------------------\n");
	printf("Axis rotation (in degree) :\n");
	printf("x = %.2f, y = %.2f, z = %.2f\n",angle[0], angle[1], angle[2] );
	printf("---------------------------------------------------\n");
	printf("Field of view angle (in degree) = %.2f\n",fovy);
	printf("---------------------------------------------------\n");
	if(wireframe)
		printf("Wireframe = YES\n");
	else
		printf("Wireframe = NO\n");
    if(shading)
        printf("Shading = YES\n");
    else
        printf("Shading = NO\n");
    printf("---------------------------------------------------\n");	
}

void menu(int sel) {

	if(sel == MODE_CHANGE_EYE_POS || sel == MODE_CHANGE_REFERENCE_POS || sel == MODE_CHANGE_UP_POS || sel == MODE_CHANGE_LIGHT_POS || sel == MODE_ROTATE_MODEL)
		mode = sel;

    if (sel == MODE_CHANGE_ZOOM)
        mode = sel;
    
	if(sel == MODE_CHANGE_CULLING)
		cull = !cull;
    
	if(sel == MODE_CHANGE_WIREFRAME)
		wireframe = !wireframe;
    
	if(sel == MODE_CHANGE_PROJECTION)
		orpro = !orpro;
    
	if(sel == MODE_CHANGE_SHADING)
		shading = !shading;

	if(sel == MODE_CHANGE_MATERIAL)

		mater = (mater + 1) % 4;
	
	if(sel == MODE_RESET)
		reset();

	if(sel == MODE_QUIT)
		exit(0);

	if(sel == MODE_PRINT_SYSTEM_STATUS)
		print_sys_status();

	glutPostRedisplay();
}

int main (int argc, char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( WindowWidth, WindowHeight ); 
    glutInitWindowPosition (0, 0);
    glutCreateWindow("Model Viewer ");

    glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
    glutIdleFunc(idle);

	glutCreateMenu(menu); 
	glutAddMenuEntry("Menu",-1);
	glutAddMenuEntry("",-1);   
	glutAddMenuEntry("Change eye point (use x,y,z,X,Y,Z)", MODE_CHANGE_EYE_POS);
	glutAddMenuEntry("Change reference point (use x,y,z,X,Y,Z)", MODE_CHANGE_REFERENCE_POS);
	glutAddMenuEntry("Change up vector (use x,y,z,X,Y,Z)", MODE_CHANGE_UP_POS);
	glutAddMenuEntry("Change light position (use x,y,z,X,Y,Z)", MODE_CHANGE_LIGHT_POS);
	glutAddMenuEntry("Zoom (use f,F)", MODE_CHANGE_ZOOM);
	glutAddMenuEntry("Rotate Model (use x,y,z,X,Y,Z)", MODE_ROTATE_MODEL );

	glutAddMenuEntry("",-1);
	glutAddMenuEntry("Culling", MODE_CHANGE_CULLING);
	glutAddMenuEntry("Wireframe", MODE_CHANGE_WIREFRAME);
	glutAddMenuEntry("Ortographic or Prospective", MODE_CHANGE_PROJECTION);
	glutAddMenuEntry("Shading", MODE_CHANGE_SHADING);
	glutAddMenuEntry("Material", MODE_CHANGE_MATERIAL); 

	glutAddMenuEntry("",-1);   
	glutAddMenuEntry("Print system status", MODE_PRINT_SYSTEM_STATUS);    
	glutAddMenuEntry("Reset", MODE_RESET);     
	glutAddMenuEntry("Quit", MODE_QUIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	reset();
    init();

    glutMainLoop();

	return -1;
}
