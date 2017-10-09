#include <stdlib.h>
#include <math.h>
#include <limits.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "RgbImage.h"
#include "lab-05.h"

const float PI2 = 2.0f*3.1415926535;

enum AppMode {
	MODE_CUBE,
	MODE_TORUS,
	MODE_QUADS,
};
enum AppMode appMode = MODE_TORUS;

GLenum runMode = GL_TRUE;

GLenum shadeModel = GL_FLAT;		// Toggles between GL_FLAT and GL_SMOOTH
GLenum polygonMode = GL_LINE;		// Toggles between GL_LINE and GL_FILL

// Variables controlling the animation
float RotX = 0.0f;					// Rotational position around x-axis
float RotY = 0.0f;					// Rotational position around y-axis
float RotIncrementX = 0.00;			// Rotational increment, x-axis
float RotIncrementY = 0.00;			// Rotational increment, y-axis
const float RotIncFactor = 1.5;	// Factor change in rot rate per key stroke

// Variables controlling the fineness of the polygonal mesh
int NumWraps = 10;
int NumPerWrap = 8;

// Variables controlling the size of the torus
float MajorRadius = 3.0;
float MinorRadius = 1.0;

// Mode flags
int QuadMode = 1;				// Quad/Triangle toggling
GLenum LocalMode = GL_TRUE;		// Local viewer/non-local viewer mode
int Light0Flag = 1;				// Is light #0 on?
int Light1Flag = 1;				// Is light #1 on?

// Lighting values
float ambientLight[4] = {0.6, 0.6, 0.6, 1.0};
float Lt0amb[4] = {0.8, 0.8, 0.16, 1.0};
float Lt0diff[4] = {1.0, 1.0, 0.2, 1.0};
float Lt0spec[4] = {1.0, 1.0, 0.2, 1.0};
float Lt0pos[4] = {1.7*4.0, 0.0, 0.0, 1.0};			// 4 = MajorRadius + MinorRadius

float Lt1amb[4] = {0.0, 0.0, 0.5, 1.0};
float Lt1diff[4] = {0.0, 0.0, 0.5, 1.0};
float Lt1spec[4] = {0.0, 0.0, 1.0, 1.0};
float Lt1pos[4] = {0.0, 1.2*4.0, 0.0, 1.0};

// Material values
float Noemit[4] = {0.0, 0.0, 0.0, 1.0};
float Matspec[4] = {1.0, 1.0, 1.0, 1.0};
float Matnonspec[4] = {0.4, 0.4, 0.4, 1.0};
float Matshiny = 16.0;

const char* filenameArray[4] = {
	"/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/pall2.bmp",
	"/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/LightningTexture.bmp",
	"/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/acqua.bmp",
	"/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/sassi.bmp"
};

const char* cubeMappingFiles[6] = {
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/negx2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/posx2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/negy2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/posy2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/negz2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/posz2.bmp"
};

const char* sphereMappingFile = "/Università/Magistrale/Fondamenti di CG/Lab/Lab 5/Lab 5/textures/Reflect.bmp";

static GLuint textureName[4];
static GLuint checkerTexture;
static GLuint proceduralTexture;
static GLuint sphereName;
static GLuint cubeMapping;

int procedural = 0;

// glutKeyboardFunc is called below to set this function to handle
//		all "normal" key presses.
void keyboard( unsigned char key, int x, int y ) {
	switch ( key ) {
	case ' ':
		appMode = (appMode+1) % 3;

		switch ( appMode ) {
		case MODE_CUBE:
			glShadeModel( GL_SMOOTH );
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_LIGHTING);
		break;
		case MODE_TORUS:
			glEnable(GL_LIGHTING);
			glShadeModel( shadeModel );
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
		break;
		case MODE_QUADS:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glShadeModel( GL_FLAT );
			glDisable(GL_LIGHTING);
		break;
		}

	break;
	case 'a':
		runMode = !runMode;
	break;
	case 's':
		runMode = GL_TRUE;
		display();
		runMode = GL_FALSE;
	break;
	case 27:	// Escape key
		exit(1);
	case 'r':	// Reset the animation (resets everything)
		ResetAnimation();
	break;
	case '0':	// Zero the rotation rates
		ZeroRotation();
	break;
	case 'f':	// Shade mode toggles from flat to smooth
		ShadeModelToggle();
	break;
	case 'p':	// Polygon mode toggles between fill and line
		FillModeToggle();
	break;
	case 'w':	// Decrement number of wraps around torus
		WrapLess();
	break;
	case 'W':	// Increment number of wraps around torus
		WrapMore();
	break;
	case 'n':	// Decrement number of polys per wrap
		NumPerWrapLess();
	break;
	case 'N':	// Increment number of polys per wrap
		NumPerWrapMore();
	break;
	case 'q':	// Toggle between triangles and Quadrilaterals
		QuadTriangleToggle();
	break;
	case 'l':	// Toggle between local and non-local viewer ('l' is for 'local')
		LocalToggle();
	break;
	case '1':	// Toggle light #0 on and off
		Light0Toggle();
	break;
	case '2':	// Toggle light #1 on and off
		Light1Toggle();
	break;
	}

	glutPostRedisplay();
}

// glutSpecialFunc is called below to set this function to handle
//		all "special" key presses.  See glut.h for the names of
//		special keys.
void special( int key, int x, int y ) {
	switch ( key ) {
	case GLUT_KEY_UP:
		// Either increase upward rotation, or slow downward rotation
		KeyUp();
		break;
	case GLUT_KEY_DOWN:
		// Either increase downwardward rotation, or slow upward rotation
		KeyDown();
		break;
	case GLUT_KEY_LEFT:
		// Either increase left rotation, or slow down rightward rotation.
		KeyLeft();
		break;
	case GLUT_KEY_RIGHT:
		// Either increase right rotation, or slow down leftward rotation.
		KeyRight();
		break;
	}

	glutPostRedisplay();
}

// The routines below are coded so that the only way to change from
//	one direction of rotation to the opposite direction is to first
//  reset the animation,

void KeyUp() {
    if ( RotIncrementX == 0.0 ) {
		RotIncrementX = -0.1;		// Initially, one-tenth degree rotation per update
	}
	else if ( RotIncrementX < 0.0f) {
		RotIncrementX *= RotIncFactor;
	}
	else {
		RotIncrementX /= RotIncFactor;
	}
}

void KeyDown() {
    if ( RotIncrementX == 0.0 ) {
		RotIncrementX = 0.1;		// Initially, one-tenth degree rotation per update
	}
	else if ( RotIncrementX > 0.0f) {
		RotIncrementX *= RotIncFactor;
	}
	else {
		RotIncrementX /= RotIncFactor;
	}
}

void KeyLeft() {
    if ( RotIncrementY == 0.0 ) {
		RotIncrementY = -0.1;		// Initially, one-tenth degree rotation per update
	}
	else if ( RotIncrementY < 0.0) {
		RotIncrementY *= RotIncFactor;
	}
	else {
		RotIncrementY /= RotIncFactor;
	}
}

void KeyRight() {
    if ( RotIncrementY == 0.0 ) {
		RotIncrementY = 0.1;		// Initially, one-tenth degree rotation per update
	}
	else if ( RotIncrementY > 0.0) {
		RotIncrementY *= RotIncFactor;
	}
	else {
		RotIncrementY /= RotIncFactor;
	}
}


// Resets position and sets rotation rate back to zero.
void ResetAnimation() {
	RotX = RotY = RotIncrementX = RotIncrementY = 0.0;
}

// Sets rotation rates back to zero.
void ZeroRotation() {
	RotIncrementX = RotIncrementY = 0.0;
}

// Toggle between smooth and flat shading
void ShadeModelToggle() {
	if ( shadeModel == GL_FLAT ) {
		shadeModel = GL_SMOOTH;
	}
	else {
		shadeModel = GL_FLAT;
	}
	glShadeModel( shadeModel );	// Set the shading to flat or smooth.
}

// Toggle between line mode and fill mode for polygons.
void FillModeToggle() {
	if ( polygonMode == GL_LINE ) {
		polygonMode = GL_FILL;
	}
	else {
		polygonMode = GL_LINE;
	}
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);	// Set to be "wire" or "solid"
}

// Toggle quadrilaterial and triangle mode
void QuadTriangleToggle() {
	QuadMode = 1-QuadMode;
}

// Toggle from local to global mode
void LocalToggle() {
	LocalMode = !LocalMode;
	if ( LocalMode ) {
		Lt0pos[3] = Lt1pos[3] = 1.0;	// Put lights back at finite location.
	}
	else {
		Lt0pos[3] = Lt1pos[3] = 0.0;	// Put lights at infinity too.
	}
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, LocalMode);
}

// The next two routines toggle the lights on and off
void Light0Toggle() {
	Light0Flag = 1-Light0Flag;
}

void Light1Toggle() {
	Light1Flag = 1-Light1Flag;
}


// Increment number of wraps
void WrapMore() {
	NumWraps++;
}

// Decrement number of wraps
void WrapLess() {
	if (NumWraps>4) {
		NumWraps--;
	}
}

// Increment number of segments per wrap
void NumPerWrapMore() {
	NumPerWrap++;
}

// Decrement number segments per wrap
void NumPerWrapLess() {
	if (NumPerWrap>4) {
		NumPerWrap--;
	}
}

/*
 * issue vertex command for segment number j of wrap number i.
 */
// Assegna ad ogni punto passato un punto della texture con le coordinate parametriche
// a seconda di i e j trovo phi e theta con le equazioni parametriche
void putVert(int i, int j) {
	float phi = PI2 * j / NumPerWrap;
	float theta = PI2 * i / NumWraps;
	float sinphi = sin(phi);
	float cosphi = cos(phi);
	float sintheta = sin(theta);
	float costheta = cos(theta);
	float r = MajorRadius + MinorRadius * cosphi;

	// the normal is the cross product of the partial derivatives
    glNormal3f(sintheta * cosphi, sinphi, costheta * cosphi);

    // PARTE 3
    if(procedural == 1)
        glTexCoord2f((j / (float) NumPerWrap), (i / (float) NumWraps));
    // PARTE 1
    else
        glTexCoord2f((i / (float) NumWraps) * 2, (j / (float) NumPerWrap) * 2);

	// place the vertex
	glVertex3f(sintheta * r, MinorRadius * sinphi, costheta * r);
}

/*
 * Display the i-th texture.
 */
void drawTextureQuad( int i ) {
	glBindTexture(GL_TEXTURE_2D, textureName[i]);

	glScalef(2.0, 2.0, 1.0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glEnd();

}

GLfloat planes[]= {-1.0, 0.0, 1.0, 0.0};
GLfloat planet[]= {0.0, -1.0,  0.0, 1.0};

GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
	{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
	{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

GLfloat colors[][4] = {{0.0,0.0,0.0,0.5},{1.0,0.0,0.0,0.5},
	{1.0,1.0,0.0,0.5}, {0.0,1.0,0.0,0.5}, {0.0,0.0,1.0,0.5},
	{1.0,0.0,1.0,0.5}, {1.0,1.0,1.0,0.5}, {0.0,1.0,1.0,0.5}};

/* draw a polygon via list of vertices */
void polygon(int a, int b, int c , int d) {
	glBegin(GL_POLYGON);
	glColor4fv(colors[a]);
	glTexCoord2f(0.0,0.0);
	glVertex3fv(vertices[a]);
	glColor4fv(colors[b]);
	glTexCoord2f(0.0,1.0);
	glVertex3fv(vertices[b]);
	glColor4fv(colors[c]);
	glTexCoord2f(1.0,1.0);
	glVertex3fv(vertices[c]);
	glColor4fv(colors[d]);
	glTexCoord2f(1.0,0.0);
	glVertex3fv(vertices[d]);
	glEnd();
}

/* map vertices to faces */
void colorcube(void) {
	glBindTexture(GL_TEXTURE_2D, checkerTexture);

	polygon(0,3,2,1);
	polygon(2,3,7,6);
	polygon(0,4,7,3);
	polygon(1,2,6,5);
	polygon(4,5,6,7);
	polygon(0,1,5,4);
}

void initProceduralTextures() {
    GLubyte image[12][12][3];
    int i,  j, cont = 1;
    int colors[3][3] = {{0, 255, 0},{255, 255, 255},{255, 0, 0}}; // Italia
    
    procedural = 1;
    
    glGenTextures(1, &proceduralTexture);
    glBindTexture(GL_TEXTURE_2D, proceduralTexture);
    
    // O genero la texture come qua o la passo da file
    for(i = 0; i < 12; i ++) {
        for(j = 0; j < 12; j ++) {
            if(cont == 1) {
                image[i][j][0]= (GLubyte) colors[0][0];
                image[i][j][1]= (GLubyte) colors[0][1];
                image[i][j][2]= (GLubyte) colors[0][2];
            } else if(cont == 2) {
                image[i][j][0]= (GLubyte) colors[1][0];
                image[i][j][1]= (GLubyte) colors[1][1];
                image[i][j][2]= (GLubyte) colors[1][2];
            } else if(cont == 3) {
                image[i][j][0]= (GLubyte) colors[2][0];
                image[i][j][1]= (GLubyte) colors[2][1];
                image[i][j][2]= (GLubyte) colors[2][2];
            }
        }
        if(cont == 3)
            cont = 1;
        else
            cont++;
    }
    
    // Sposto la texture in memoria
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 12, 12, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
}

void initSphereMappingImage( const char* filename) {
    glGenTextures(1, &sphereName);
    glBindTexture(GL_TEXTURE_2D, sphereName);
    RgbImage theTexMap;
    if(!RgbImageInitFile(&theTexMap, filename) ) {
        exit(1);
    }
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, GetNumCols(&theTexMap), GetNumRows(&theTexMap), GL_RGB, GL_UNSIGNED_BYTE, ImageData(&theTexMap) );
}

void display( void ) {
	int i,j;

	// Clear the redering window
	glClearColor( 0.1, 0.1, 0.1, 1.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up lights
	if ( Light0Flag==1 || Light1Flag==1 ) {
		// Emissive spheres have no other color.
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Noemit);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Noemit);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0);
	}
	if ( Light0Flag==1 ) {
		glPushMatrix();
		glTranslatef(Lt0pos[0], Lt0pos[1], Lt0pos[2]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt0spec);
		glutSolidSphere(0.2,5,5);
		glPopMatrix();
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);
	}
	else {
		glDisable(GL_LIGHT0);
	}
	if ( Light1Flag==1 ) {
		glPushMatrix();
		glTranslatef(Lt1pos[0], Lt1pos[1], Lt1pos[2]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt1spec);
		glutSolidSphere(0.2,5,5);
		glPopMatrix();
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_POSITION, Lt1pos);
	}
	else {
		glDisable(GL_LIGHT1);
	}

	// Torus Materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Matnonspec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Matspec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Matshiny);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Noemit);

	glPushMatrix();		// Save to use again next time.

	// Update the orientation of the torus, if the animation is running.
	if ( runMode ) {
		RotY += RotIncrementY;
		if ( fabs(RotY)>360.0 ) {
			RotY -= 360.0*((int)(RotY/360.0));
		}
		RotX += RotIncrementX;
		if ( fabs(RotX)>360.0 ) {
			RotX -= 360.0*((int)(RotX/360.0));
		}
		glutPostRedisplay();
	}
	// Set the orientation.
	glRotatef( RotX, 1.0, 0.0, 0.0);
	glRotatef( RotY, 0.0, 1.0, 0.0);

	switch(appMode) {
        case MODE_CUBE:
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glScalef(2.0, 2.0, 2.0);
            colorcube();
            glDisable(GL_TEXTURE_2D);
            break;
            
        case MODE_TORUS:
            // Draw the torus
            glColor3f( 1.0, 0.5, 1.0 );

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureName[3]);
            
            // PARTE 2 - Cube Mapping
            glEnable(GL_TEXTURE_CUBE_MAP);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapping);
            
            // PARTE 2 - Sphere Mapping
//            glEnable(GL_TEXTURE_2D);
//            glTexGeni(GL_S,GL_TEXTURE_GEN_MODE ,GL_SPHERE_MAP);
//            glTexGeni(GL_T,GL_TEXTURE_GEN_MODE ,GL_SPHERE_MAP);
//            glEnable(GL_TEXTURE_GEN_S);
//            glEnable(GL_TEXTURE_GEN_T);
//            initSphereMappingImage(sphereMappingFile);
//            glBindTexture(GL_TEXTURE_2D, sphereName);
            
            // PARTE 3
//            initProceduralTextures();

            for (i = 0; i < NumWraps; i ++) {
                glBegin(QuadMode == 1 ? GL_QUAD_STRIP : GL_TRIANGLE_STRIP);
                for (j = 0; j <= NumPerWrap; j ++) {
                    putVert(i, j);
                    putVert(i + 1, j);
                }
                glEnd();
            }
            glDisable(GL_TEXTURE_2D);

            // Draw the reference pyramid
            glTranslatef( -MajorRadius-MinorRadius-0.3, 0.0, 0.0);
            glScalef( 0.2f, 0.2f, 0.2f );
            glColor3f( 1.0f, 1.0f, 0.0f );
            glBegin(GL_TRIANGLE_STRIP);
            glVertex3f( -0.5, 0.0, sqrt(3.0)*0.5 );
            glVertex3f( -0.5, 0.0, -sqrt(3.0)*0.5 );
            glVertex3f( 1.0, 0.0, 0.0);
            glVertex3f( 0.0, sqrt(2.0), 0.0);
            glVertex3f( -0.5, 0.0, sqrt(3.0)*0.5 );
            glVertex3f( -0.5, 0.0, -sqrt(3.0)*0.5 );
            glEnd();
            break;
            
        case MODE_QUADS:
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glPushMatrix();
            glTranslatef(-2.1f, 2.1f, 0.0f);
            drawTextureQuad(0);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(2.1f, 2.1f, 0.0f);
            drawTextureQuad(1);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(-2.1f, -2.1f, 0.0f);
            drawTextureQuad(2);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(2.1f, -2.1f, 0.0f);
            drawTextureQuad(3);
            glPopMatrix();

            glDisable(GL_TEXTURE_2D);
            break;
	}
	glPopMatrix();
	glutSwapBuffers();
}

// Initialize OpenGL
void initRendering() {
	glEnable( GL_DEPTH_TEST );

	glEnable(GL_LIGHTING);		// Enable lighting calculations
	glEnable(GL_LIGHT0);		// Turn on lights (unnecessary here, since also in Animate()
	glEnable(GL_LIGHT1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);	// Ambient light

	// Light 0 (Position is set in display)
	glLightfv(GL_LIGHT0, GL_AMBIENT, Lt0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Lt0spec);

	// Light 1 (Position is set in display)
	glLightfv(GL_LIGHT1, GL_AMBIENT, Lt1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Lt1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, Lt1spec);

	glShadeModel( shadeModel );	// Set the shading to flat or smooth.
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);	// Set to be "wire" or "solid"
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, LocalMode);
}

// Called when the window is resized
//		Sets up the projection view matrix (somewhat poorly, however)
void reshape(int w, int h) {
	float aspectRatio;
	glViewport( 0, 0, w, h );	// View port uses whole window
	h = (w == 0) ? 1 : h;
	aspectRatio = (float)w/(float)h;

	// Set up the proection view matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, aspectRatio, 1.0, 30.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	// Move system 10 units away to be able to view from the origin.
	glTranslatef(0.0, 0.0, -10.0);

	// Tilt system 15 degrees downward in order to view from above
	//   the xy-plane.
	glRotatef(15.0, 1.0,0.0,0.0);
}

/*
 * Read a texture map from a BMP bitmap file.
 */
void loadTextureFromFile(const char *filename) {
	RgbImage theTexMap;
	if(!RgbImageInitFile(&theTexMap, filename))
		exit(1);

	// Pixel alignment: each row is word aligned.  Word alignment is the default.
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4);		

	// Set the interpolation settings to best quality.
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, GetNumCols(&theTexMap), GetNumRows(&theTexMap), GL_RGB, GL_UNSIGNED_BYTE, ImageData(&theTexMap));
}

/*
 * Load the four textures, by repeatedly called loadTextureFromFile().
 */
// Caricare qua la texture per il toro
void initFour(const char* filenames[]) {
	int i;
	glGenTextures(4, textureName);	// Load four texture names into array
	for(i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, textureName[i]);	// Texture #i is active now
		loadTextureFromFile(filenames[i]);			// Load texture #i
	}
}

void initCheckerTextures() {
	GLubyte image[64][64][3];
	int i,  j, c;

    glGenTextures(1, &checkerTexture);
	glBindTexture(GL_TEXTURE_2D, checkerTexture);

    // O genero la texture come qua o la passo da file
	for(i = 0; i < 64; i ++) {
		for(j = 0; j < 64; j ++) {
			c = ( ( ( ( i & 0x8 ) == 0 ) ^ ( ( ( j & 0x8 ) ) == 0 ) ) ) * 255;
			image[i][j][0]= (GLubyte) c;
			image[i][j][1]= (GLubyte) c;
			image[i][j][2]= (GLubyte) c;
		}
	}

    // Sposto la texture in memoria
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

RgbImage loadTextureFromFileEnvironmental( const char *filename) {
    RgbImage theTexMap;
    if( !RgbImageInitFile(&theTexMap, filename) ) {
        exit(1);
    }
    return theTexMap;
}

void initCubeMapping() {
    glGenTextures(1, &cubeMapping);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapping);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    
    
    RgbImage img1 = loadTextureFromFileEnvironmental(cubeMappingFiles[0]);
    RgbImage img2 = loadTextureFromFileEnvironmental(cubeMappingFiles[1]);
    RgbImage img3 = loadTextureFromFileEnvironmental(cubeMappingFiles[2]);
    RgbImage img4 = loadTextureFromFileEnvironmental(cubeMappingFiles[3]);
    RgbImage img5 = loadTextureFromFileEnvironmental(cubeMappingFiles[4]);
    RgbImage img6 = loadTextureFromFileEnvironmental(cubeMappingFiles[5]);
    
    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img1));
    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img2));
    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img3));
    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img4));
    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img5));
    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img6));
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
}

// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv ) {
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	// Window position (from top corner), and size (width and hieght)
	glutInitWindowPosition( 10, 60 );
	glutInitWindowSize( 620, 160 );
	glutCreateWindow( "Light Torus demo" );

	// Initialize OpenGL rendering modes
	initRendering();

	// Initialize textures
	initCheckerTextures();
    // Init texture bitmap
	initFour(filenameArray);
    
    // PARTE 2 - Cube Mapping
    initCubeMapping();

	reshape(620,160);

	// Set up callback functions for key presses
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( special );

	// Set up the callback function for resizing windows
	glutReshapeFunc( reshape );

	// Call this whenever window needs redrawing
	glutDisplayFunc( display );

	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop(  );

	return(0);	// This line is never reached.
}

