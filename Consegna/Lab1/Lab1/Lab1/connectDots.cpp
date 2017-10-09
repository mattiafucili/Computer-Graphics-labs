/*
 * ConnectDots.c
 *
 *     This program draws straight lines connecting dots placed with mouse clicks.
 *
 *
 * Usage:
 *   Left click to place a control point.
 *		Maximum number of control points allowed is currently set at 64.
 *	 Press "f" to remove the first control point
 *	 Press "l" to remove the last control point.
 *	 Press escape to exit.
 */

#include <stdlib.h>
#include <GLUT/glut.h>
#include <stdio.h>
#include <math.h>

#define MAX_CV 64
#define GAP 0.01

/* prototypes */
void removeFirstPoint ();
void removeLastPoint ();
void addNewPoint (float x, float y, float z);
bool checkDistance(float x1, float x2, float y1, float y2, float xp, float yp);
bool checkPoints(float points[MAX_CV][3], int n);
void straightLine(float points[MAX_CV][3], int n);
void adaptiveDeCasteljau(float points[MAX_CV][3], int n, float t, float firstHalfPoints[MAX_CV][3], float secondHalfPoints[MAX_CV][3]);
void decasteljau(float t, float* result);

/* global variables */
float CV[MAX_CV][3]; // Control vertexes
int numCV = 0;
bool flag = false;
int pos = -1;

bool adaptive = false;

// Window size in pixels
int WindowHeight;
int WindowWidth;

/* function definitions */

void keyboard (unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'f':
            removeFirstPoint ();
            glutPostRedisplay ();
            break;
        case 'l':
            removeLastPoint ();
            glutPostRedisplay ();
            break;
        case 'a':
            adaptive = !adaptive;
            glutPostRedisplay();
            break;
        case 27:						   // Escape key
            exit (0);
            break;
    }
}

void removeFirstPoint ()
{
    int i;
    if (numCV > 0)
    {
        // Remove the first point, slide the rest down
        numCV--;
        for (i = 0; i < numCV; i++)
        {
            CV[i][0] = CV[i + 1][0];
            CV[i][1] = CV[i + 1][1];
        }
    }
}

// Left button presses place a control point.
void mouse (int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        yPos = 1.0f - yPos;			   // Flip value since y position is from top row.
        float zPos = 0;
        addNewPoint (xPos, yPos, zPos);
        glutPostRedisplay ();
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        yPos = 1.0f - yPos;			   // Flip value since y position is from top row.
        float zPos = 0;
        
//        GLfloat* size;
//        glGetFloatv(GL_POINT_SIZE, size);
        for(int i = 0; i < numCV; i++) {
//            float square_root = sqrtf(pow(CV[i][0] - xPos, 2) - pow(CV[i][1] - yPos, 2));
//            GLfloat* value = new GLfloat(square_root);
//            if(&value < &size) {
//                flag = true;
//                pos = i;
//            }
            
            if(CV[i][0] <= (xPos + 0.01) && CV[i][0] >= (xPos - 0.01) && CV[i][1] <= (yPos + 0.01) && CV[i][1] >= (yPos - 0.01) && CV[i][2] <= (zPos + 0.01) && CV[i][2] >= (zPos - 0.01)) {
                flag = true;
                pos = i;
                break;
            }
        }
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        flag = false;
        pos = -1;
    }
}

void mouseMotion(int x, int y) {
    if(flag) {
        float xPos = ((float) x) / ((float) (WindowWidth - 1));
        float yPos = ((float) y) / ((float) (WindowHeight - 1));
        yPos = 1.0f - yPos;			   // Flip value since y position is from top row.
        float zPos = 0;
        CV[pos][0] = xPos;
        CV[pos][1] = yPos;
        CV[pos][2] = zPos;
        glutPostRedisplay();
    }
}

// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void removeLastPoint ()
{
    if (numCV > 0)
        numCV--;
}


// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void addNewPoint (float x, float y, float z)
{
    if (numCV >= MAX_CV)
        removeFirstPoint ();
    
    CV[numCV][0] = x;
    CV[numCV][1] = y;
    CV[numCV][2] = z; //unused
    numCV++;
}

void display (void)
{
    int i;
    glClear (GL_COLOR_BUFFER_BIT);
    
    // Draw the line segments
    glColor3f (1.0f, 0.0f, 0.8f);	   // Reddish/purple lines
    glEnable (GL_LINE_STIPPLE); // Line types
    glLineStipple (1, 0x00FF);
    if (numCV > 1) // If 2 or more points...
    {
        glBegin (GL_LINE_STRIP);
        for (i = 0; i < numCV; i++)
        {
            glVertex2f (CV[i][0], CV[i][1]);
        }
        glEnd ();
        
        glDisable (GL_LINE_STIPPLE);
        glColor3f (0.0f, 0.0f, 0.0f);	   // Draw points in black
        
        glBegin(GL_LINE_STRIP);
//        // De Casteljau normale
        if(!adaptive) {
            float result[2];
            for(int t = 0; t < 100; t++) {
                decasteljau((GLfloat) t/100, &result[0]);
                glVertex3f(result[0], result[1], 0.0);
            }
        } else {
            straightLine(CV, numCV);
        }
        
//        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, numCV, &CV[0][0]);
        
//        for(int i = 0; i < 100; i++)
//            glEvalCoord1f((GLfloat) i / 100.0);


        glEnd();
        
    }
    
    // Draw the interpolated points second.
    glBegin (GL_POINTS);
    for (i = 0; i < numCV; i++)
    {
        glVertex2f (CV[i][0], CV[i][1]);
    }
    glEnd ();
    
    glFlush ();
}

bool checkDistance(float x1, float x2, float y1, float y2, float xp, float yp) {
    float a = -(y2 - y1);
    float b = (x2 - x1);
    float c = x1*(y2 - y1) - y2*(x2 - x1);
    
    float d = (fabsf(a*xp + b*yp + c) / sqrtf(powf(a, 2) + powf(b, 2)));
    
    return d <= GAP;
}

bool checkPoints(float points[MAX_CV][3], int n) {
    bool result = true;
    int i = 1;
    
    do {
        result = result && checkDistance(points[0][0], points[n - 1][0], points[0][1], points[n - 1][1], points[i][0], points[i][1]);
        if(!result)
            return result;
        i++;
    } while(i < (n - 1));
    
    return result;
}

void straightLine(float points[MAX_CV][3], int n) {
    if(checkPoints(points, n)) {
        glVertex3f(points[0][0], points[0][1], points[0][2]);
        glVertex3f(points[n - 1][0], points[n - 1][1], points[n - 1][2]);
    } else {
        float firstHalfPoints[MAX_CV][3], secondHalfPoints[MAX_CV][3];
        
        adaptiveDeCasteljau(points, n, 0.5, firstHalfPoints, secondHalfPoints);
        
        straightLine(firstHalfPoints, n);
        float reversedSecondHalfPoints[MAX_CV][3];
        int index = 0;
        for(int i = n - 1; i >= 0; i--) {
            reversedSecondHalfPoints[index][0] = secondHalfPoints[i][0];
            reversedSecondHalfPoints[index][1] = secondHalfPoints[i][1];
            reversedSecondHalfPoints[index][2] = secondHalfPoints[i][2];
            
            index++;
        }
        straightLine(reversedSecondHalfPoints, n);
    }
}

void adaptiveDeCasteljau(float points[MAX_CV][3], int n, float t, float firstHalfPoints[MAX_CV][3], float secondHalfPoints[MAX_CV][3]) {
    float coords[MAX_CV][3];
    int j;
    
    for(int i = 0; i < n; i++) {
        coords[i][0] = points[i][0];
        coords[i][1] = points[i][1];
        coords[i][2] = points[i][2];
    }
    
    firstHalfPoints[0][0] = coords[0][0];
    firstHalfPoints[0][1] = coords[0][1];
    firstHalfPoints[0][2] = coords[0][2];
    
    // da ciclare al contrario però
    secondHalfPoints[0][0] = coords[n - 1][0];
    secondHalfPoints[0][1] = coords[n - 1][1];
    secondHalfPoints[0][2] = coords[n - 1][2];
    
    for(int i = 1; i < n; i++) {
        for(j = 0; j < (n - i); j ++) {
            coords[j][0] = (1 - t) * coords[j][0] + t * coords[j + 1][0];
            coords[j][1] = (1 - t) * coords[j][1] + t * coords[j + 1][1];
            coords[j][2] = 0;
        }
        
        firstHalfPoints[i][0] = coords[0][0];
        firstHalfPoints[i][1] = coords[0][1];
        firstHalfPoints[i][2] = coords[0][2];
        
        // da ciclare al contrario però
        secondHalfPoints[i][0] = coords[j - 1][0];
        secondHalfPoints[i][1] = coords[j - 1][1];
        secondHalfPoints[i][2] = coords[j - 1][2];
    }
}

void decasteljau(float t, float* result) {
    float coords[MAX_CV][2];
    int i, j;
    
    for(i = 0; i < numCV; i++) {
        coords[i][0] = CV[i][0];
        coords[i][1] = CV[i][1];
    }
    
    for(i = 1; i < numCV; i++) {
        for(j = 0; j < (numCV - i); j ++) {
            coords[j][0] = (1 - t) * coords[j][0] + t * coords[j + 1][0];
            coords[j][1] = (1 - t) * coords[j][1] + t * coords[j + 1][1];
        }
    }
    
    result[0] = coords[0][0];
    result[1] = coords[0][1];
}

void initRendering ()
{
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
    
    // Make big points and wide lines.  (This may be commented out if desired.)
    glPointSize (5);
    glLineWidth (1);
    
    // The following commands should induce OpenGL to create round points and
    //  antialias points and lines.  (This is implementation dependent unfortunately, and
    //  may slow down rendering considerably.)
    //  You may comment these out if you wish.
    glEnable (GL_POINT_SMOOTH);
    glEnable (GL_LINE_SMOOTH);
    glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);	// Antialias the lines
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_MAP1_VERTEX_3);
}

void reshape (int w, int h)
{
    WindowHeight = (h > 1) ? h : 2;
    WindowWidth = (w > 1) ? w : 2;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (0.0f, 1.0f, 0.0f, 1.0f);	// Always view [0,1]x[0,1].
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
} 

int main (int argc, char **argv) 
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    
    initRendering ();
    
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutKeyboardFunc (keyboard);
    glutMouseFunc (mouse);
    glutMotionFunc(mouseMotion);
    
    glutMainLoop ();
    return 0;						   // This line is never reached
}
