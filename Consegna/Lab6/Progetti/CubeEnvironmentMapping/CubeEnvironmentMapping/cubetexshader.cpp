#include "../common/common.h"

// *********************** RgbImage.h
#ifndef RGBIMAGE_H
#define RGBIMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Include the next line to turn off the routines that use OpenGL
#define RGBIMAGE_DONT_USE_OPENGL

#define bool int
#define true 1
#define false 0

struct RgbImage {
    unsigned char* ImagePtr;    // array of pixel values (integers range 0 to 255)
    long NumRows;                // number of rows in image
    long NumCols;                // number of columns in image
    int ErrorCode;                // error code
};
typedef struct RgbImage RgbImage;

void RgbImageInit(RgbImage* image);
bool RgbImageInitFile(RgbImage* image, const char* filename );
void RgbImageInitSize(RgbImage* image, int numRows, int numCols );    // Initialize a blank bitmap of this size.
void RgbImageDelete(RgbImage* image);

bool LoadBmpFile(RgbImage* image, const char *filename );        // Loads the bitmap from the specified file
bool WriteBmpFile(RgbImage* image, const char* filename );        // Write the bitmap to the specified file
#ifndef RGBIMAGE_DONT_USE_OPENGL
bool LoadFromOpenglBuffer(RgbImage* image);                    // Load the bitmap from the current OpenGL buffer
#endif

long GetNumRows(RgbImage* image);
long GetNumCols(RgbImage* image);

// Rows are word aligned
long GetNumBytesPerRow(RgbImage* image);
const void* ImageData(RgbImage* image);

unsigned char* GetRgbPixel(RgbImage* image, long row, long col );

/*
 const unsigned char* GetRgbPixel( long row, long col );
 void GetRgbPixel( long row, long col, float* red, float* green, float* blue );
 void GetRgbPixel( long row, long col, double* red, double* green, double* blue );
 */

void SetRgbPixelf( RgbImage* image, long row, long col, double red, double green, double blue );
void SetRgbPixelc( RgbImage* image, long row, long col,
                  unsigned char red, unsigned char green, unsigned char blue );

// Error reporting. (errors also print message to stderr)
int GetErrorCode(RgbImage* image);
enum {
    NoError = 0,
    OpenError = 1,            // Unable to open file for reading
    FileFormatError = 2,    // Not recognized as a 24 bit BMP file
    MemoryError = 3,        // Unable to allocate memory for image data
    ReadError = 4,            // End of file reached prematurely
    WriteError = 5            // Unable to write out data (or no date to write out)
};
bool ImageLoaded(RgbImage* image);

void Reset(RgbImage* image);            // Frees image data memory

static short readShort( FILE* infile );
static long readLong( FILE* infile );
static void skipChars( FILE* infile, int numChars );
static void writeLong( long data, FILE* outfile );
static void writeShort( short data, FILE* outfile );

static unsigned char doubleToUnsignedChar( double x );

#endif // RGBIMAGE_H

// *********************** RgbImage.c
#ifndef RGBIMAGE_DONT_USE_OPENGL
#include <windows.h>
#include "GL/gl.h"
#endif

void RgbImageSize(RgbImage* image, int numRows, int numCols )
{
    unsigned char* c;
    int rowLen, i, j;
    image->NumRows = numRows;
    image->NumCols = numCols;
    image->ImagePtr = (unsigned char*) malloc( sizeof(unsigned char) * image->NumRows*GetNumBytesPerRow(image) );
    if ( !image->ImagePtr ) {
        fprintf(stderr, "Unable to allocate memory for %ld x %ld bitmap.\n",
                image->NumRows, image->NumCols);
        Reset(image);
        image->ErrorCode = MemoryError;
    }
    // Zero out the image
    c = image->ImagePtr;
    rowLen = GetNumBytesPerRow(image);
    for ( i=0; i<image->NumRows; i++ ) {
        for ( j=0; j<rowLen; j++ ) {
            *(c++) = 0;
        }
    }
}

bool LoadBmpFile(RgbImage* image, const char* filename )
{
    FILE* infile = fopen( filename, "rb" );        // Open for reading binary data
    bool fileFormatOK = false;
    int bChar, mChar, bitsPerPixel, i, j, k;
    unsigned char* cPtr;
    
    Reset(image);
    if ( !infile ) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        image->ErrorCode = OpenError;
        return false;
    }
    
    bChar = fgetc( infile );
    mChar = fgetc( infile );
    if ( bChar=='B' && mChar=='M' ) {            // If starts with "BM" for "BitMap"
        skipChars( infile, 4+2+2+4+4 );            // Skip 4 fields we don't care about
        image->NumCols = readLong( infile );
        image->NumRows = readLong( infile );
        skipChars( infile, 2 );                    // Skip one field
        bitsPerPixel = readShort( infile );
        skipChars( infile, 4+4+4+4+4+4 );        // Skip 6 more fields
        
        if ( image->NumCols>0 && image->NumCols<=100000 && image->NumRows>0 && image->NumRows<=100000
            && bitsPerPixel==24 && !feof(infile) ) {
            fileFormatOK = true;
        }
    }
    if ( !fileFormatOK ) {
        Reset(image);
        image->ErrorCode = FileFormatError;
        fprintf(stderr, "Not a valid 24-bit bitmap file: %s.\n", filename);
        fclose ( infile );
        return false;
    }
    
    // Allocate memory
    image->ImagePtr = (unsigned char*) malloc( sizeof(unsigned char) * image->NumRows*GetNumBytesPerRow(image) );
    if ( !image->ImagePtr ) {
        fprintf(stderr, "Unable to allocate memory for %ld x %ld bitmap: %s.\n",
                image->NumRows, image->NumCols, filename);
        Reset(image);
        image->ErrorCode = MemoryError;
        fclose ( infile );
        return false;
    }
    
    cPtr = image->ImagePtr;
    for ( i=0; i<image->NumRows; i++ ) {
        for ( j=0; j<image->NumCols; j++ ) {
            *(cPtr+2) = fgetc( infile );    // Blue color value
            *(cPtr+1) = fgetc( infile );    // Green color value
            *cPtr = fgetc( infile );        // Red color value
            cPtr += 3;
        }
        k=3*image->NumCols;                    // Num bytes already read
        for ( ; k<GetNumBytesPerRow(image); k++ ) {
            fgetc( infile );                // Read and ignore padding;
            *(cPtr++) = 0;
        }
    }
    if ( feof( infile ) ) {
        fprintf( stderr, "Premature end of file: %s.\n", filename );
        Reset(image);
        image->ErrorCode = ReadError;
        fclose ( infile );
        return false;
    }
    fclose( infile );    // Close the file
    return true;
}

short readShort( FILE* infile )
{
    // read a 16 bit integer
    unsigned char lowByte, hiByte;
    short ret;
    lowByte = fgetc(infile);            // Read the low order byte (little endian form)
    hiByte = fgetc(infile);            // Read the high order byte
    
    // Pack together
    ret = hiByte;
    ret <<= 8;
    ret |= lowByte;
    return ret;
}

long readLong( FILE* infile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1, byte2, byte3;
    long ret;
    byte0 = fgetc(infile);            // Read bytes, low order to high order
    byte1 = fgetc(infile);
    byte2 = fgetc(infile);
    byte3 = fgetc(infile);
    
    // Pack together
    ret = byte3;
    ret <<= 8;
    ret |= byte2;
    ret <<= 8;
    ret |= byte1;
    ret <<= 8;
    ret |= byte0;
    return ret;
}

void skipChars( FILE* infile, int numChars )
{
    int i;
    for ( i=0; i<numChars; i++ ) {
        fgetc( infile );
    }
}

bool WriteBmpFile( RgbImage* image, const char* filename )
{
    int rowLen, i, j, k;
    unsigned char* cPtr;
    FILE* outfile = fopen( filename, "wb" );        // Open for reading binary data
    if ( !outfile ) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        image->ErrorCode = OpenError;
        return false;
    }
    
    fputc('B',outfile);
    fputc('M',outfile);
    rowLen = GetNumBytesPerRow(image);
    writeLong( 40+14+image->NumRows*rowLen, outfile );    // Length of file
    writeShort( 0, outfile );                    // Reserved for future use
    writeShort( 0, outfile );
    writeLong( 40+14, outfile );                // Offset to pixel data
    writeLong( 40, outfile );                    // header length
    writeLong( image->NumCols, outfile );                // width in pixels
    writeLong( image->NumRows, outfile );                // height in pixels (pos for bottom up)
    writeShort( 1, outfile );        // number of planes
    writeShort( 24, outfile );        // bits per pixel
    writeLong( 0, outfile );        // no compression
    writeLong( 0, outfile );        // not used if no compression
    writeLong( 0, outfile );        // Pixels per meter
    writeLong( 0, outfile );        // Pixels per meter
    writeLong( 0, outfile );        // unused for 24 bits/pixel
    writeLong( 0, outfile );        // unused for 24 bits/pixel
    
    // Now write out the pixel data:
    cPtr = image->ImagePtr;
    i;
    for ( i=0; i<image->NumRows; i++ ) {
        // Write out i-th row's data
        for ( j=0; j<image->NumCols; j++ ) {
            fputc( *(cPtr+2), outfile);        // Blue color value
            fputc( *(cPtr+1), outfile);        // Blue color value
            fputc( *(cPtr+0), outfile);        // Blue color value
            cPtr+=3;
        }
        // Pad row to word boundary
        k=3*image->NumCols;                    // Num bytes already read
        for ( ; k<GetNumBytesPerRow(image); k++ ) {
            fputc( 0, outfile );                // Read and ignore padding;
            cPtr++;
        }
    }
    
    fclose( outfile );    // Close the file
    return true;
}

void writeLong( long data, FILE* outfile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1, byte2, byte3;
    byte0 = (unsigned char)(data&0x000000ff);        // Write bytes, low order to high order
    byte1 = (unsigned char)((data>>8)&0x000000ff);
    byte2 = (unsigned char)((data>>16)&0x000000ff);
    byte3 = (unsigned char)((data>>24)&0x000000ff);
    
    fputc( byte0, outfile );
    fputc( byte1, outfile );
    fputc( byte2, outfile );
    fputc( byte3, outfile );
}

void writeShort( short data, FILE* outfile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1;
    byte0 = data&0x000000ff;        // Write bytes, low order to high order
    byte1 = (data>>8)&0x000000ff;
    
    fputc( byte0, outfile );
    fputc( byte1, outfile );
}

void SetRgbPixelf(RgbImage* image,  long row, long col, double red, double green, double blue )
{
    SetRgbPixelc( image, row, col, doubleToUnsignedChar(red),
                 doubleToUnsignedChar(green),
                 doubleToUnsignedChar(blue) );
}

void SetRgbPixelc( RgbImage* image, long row, long col,
                  unsigned char red, unsigned char green, unsigned char blue )
{
    unsigned char* thePixel;
    assert ( row<image->NumRows && col<image->NumCols );
    thePixel = GetRgbPixel( image, row, col );
    *(thePixel++) = red;
    *(thePixel++) = green;
    *(thePixel) = blue;
}

unsigned char doubleToUnsignedChar( double x )
{
    if ( x>=1.0 ) {
        return (unsigned char)255;
    }
    else if ( x<=0.0 ) {
        return (unsigned char)0;
    }
    else {
        return (unsigned char)(x*255.0);        // Rounds down
    }
}

#ifndef RGBIMAGE_DONT_USE_OPENGL

bool LoadFromOpenglBuffer()                    // Load the bitmap from the current OpenGL buffer
{
    int viewportData[4];
    glGetIntegerv( GL_VIEWPORT, viewportData );
    int& vWidth = viewportData[2];
    int& vHeight = viewportData[3];
    
    if ( ImagePtr==0 ) { // If no memory allocated
        NumRows = vHeight;
        NumCols = vWidth;
        ImagePtr = malloc( sizeof(unsigned char) * NumRows*GetNumBytesPerRow() );
        if ( !ImagePtr ) {
            fprintf(stderr, "Unable to allocate memory for %ld x %ld buffer.\n",
                    NumRows, NumCols);
            Reset(image);
            ErrorCode = MemoryError;
            return false;
        }
    }
    assert ( vWidth>=NumCols && vHeight>=NumRows );
    int oldGlRowLen;
    if ( vWidth>=NumCols ) {
        glGetIntegerv( GL_UNPACK_ROW_LENGTH, &oldGlRowLen );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, NumCols );
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    // Get the frame buffer data.
    glReadPixels( 0, 0, NumCols, NumRows, GL_RGB, GL_UNSIGNED_BYTE, ImagePtr);
    
    // Restore the row length in glPixelStorei  (really ought to restore alignment too).
    if ( vWidth>=NumCols ) {
        glPixelStorei( GL_UNPACK_ROW_LENGTH, oldGlRowLen );
    }
    return true;
}
#endif

long GetNumRows(RgbImage* image) { return image->NumRows; }
long GetNumCols(RgbImage* image) { return image->NumCols; }

long GetNumBytesPerRow(RgbImage* image) { return ((3*image->NumCols+3)>>2)<<2; }
const void* ImageData(RgbImage* image) { return (void*)image->ImagePtr; }


void RgbImageInit(RgbImage* image)
{
    image->NumRows = 0;
    image->NumCols = 0;
    image->ImagePtr = 0;
    image->ErrorCode = 0;
}

bool RgbImageInitFile(RgbImage* image, const char* filename )
{
    image->NumRows = 0;
    image->NumCols = 0;
    image->ImagePtr = 0;
    image->ErrorCode = 0;
    return LoadBmpFile( image, filename );
}

void RgbImageDelete(RgbImage* image)
{
    free(image->ImagePtr);
}

unsigned char* GetRgbPixel(RgbImage* image, long row, long col )
{
    unsigned char* ret = image->ImagePtr;
    long i = row*GetNumBytesPerRow(image) + 3*col;
    assert ( row<image->NumRows && col<image->NumCols );
    ret += i;
    return ret;
}

void Reset(RgbImage* image)
{
    image->NumRows = 0;
    image->NumCols = 0;
    if( image->ImagePtr != 0 )
        free(image->ImagePtr);
    image->ImagePtr = 0;
    image->ErrorCode = 0;
}

int GetErrorCode(RgbImage* image) { return image->ErrorCode; }
bool ImageLoaded(RgbImage* image) { return (image->ImagePtr!=0); }  // Is an image loaded?

// ***********************
GLuint program;
GLuint tex;

// RICHIESTA
const char* cubeMappingFiles[6] = {
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/negx2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/posx2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/negy2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/posy2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/negz2.bmp",
    "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/textures/posz2.bmp"
};

RgbImage loadTextureFromFileEnvironmental( const char *filename) {
    RgbImage theTexMap;
    if( !RgbImageInitFile(&theTexMap, filename) ) {
        exit(1);
    }
    return theTexMap;
}
// FINE

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_CUBE_MAP);
    
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    
    // RICHIESTA
    RgbImage img1 = loadTextureFromFileEnvironmental(cubeMappingFiles[0]);
    RgbImage img2 = loadTextureFromFileEnvironmental(cubeMappingFiles[1]);
    RgbImage img3 = loadTextureFromFileEnvironmental(cubeMappingFiles[2]);
    RgbImage img4 = loadTextureFromFileEnvironmental(cubeMappingFiles[3]);
    RgbImage img5 = loadTextureFromFileEnvironmental(cubeMappingFiles[4]);
    RgbImage img6 = loadTextureFromFileEnvironmental(cubeMappingFiles[5]);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img2));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img1));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img4));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img3));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img6));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ,0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData(&img5));
    // FINE
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    glClearColor(1.0,1.0,1.0,1.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    

    program = initShader("/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/CubeEnvironmentMapping/CubeEnvironmentMapping/v.glsl", "/Università/Magistrale/Fondamenti di CG/Lab/Lab 6/CubeEnvironmentMapping/CubeEnvironmentMapping/f.glsl");
    
    GLuint texMapLocation;
    texMapLocation = glGetUniformLocation(program, "texMap");
    glUniform1i(texMapLocation,0);
}

void draw(void) {
    double t = (double)glutGet(GLUT_ELAPSED_TIME);
    double k = 0.05 * 360.0/1000.0;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(k*t, 1.0, 0.0, 0.0);
    glRotatef(k*t, 0.0, 1.0, 0.0);
    glutSolidTeapot(1.0);
    glPopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("colorcube environment");
    
    glutDisplayFunc(draw);
    glutKeyboardFunc(commonKeyboard);
    
    init();
    
    glutMainLoop();
    return 0;
}
