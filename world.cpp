#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include "PPM.h"

using namespace std;

int gHeight = 400;
int gWidth = 400;
bool isTriangleMode = false;
bool isPaused = false;
bool isLightOn = false;
int wireframe = 0;
int maxHeight = 30;
int minHeight = -30;
bool isDrawingWireFrame = false;
int texCounter = 0;
bool isFlatShadingOn = true;
float displacement = static_cast<int>(rand()) % 5;

int planeSize = 1000;
// float eye[3] = {planeSize/2, planeSize/2 ,10};
GLdouble eye[3] = {0, 0, 60};
GLdouble lookAt[3];
GLdouble up[] = {0,0,1};

int numOfMountains = planeSize/8;
float heightMap[2000][2000];

float light_pos[2][4] = {{0,0,100,1}, {1,-5,100,1} };
float amb[2][4] = { {0,0,1,0.5} , {0,0.2,1,0.5} };
float diff[2][4] = { {0,0,1,1} , {1,0,0,1}  };
float spec[2][4] = { {1,1,1,1}, {1,0,1,1}  };


// void setMaterials(unsigned int index) {
    
//     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient[index]);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse[index]);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular[index]);
//     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiny[index]);

// }

//struct to hold image data 
struct Image 
{
    int mWidth;
    int mHeight;
    GLubyte * mImage;

    void load(char * filename) {
        mImage = LoadPPM(filename, &mWidth, &mHeight);
    }

    void draw(unsigned int x, unsigned int y) {
        glRasterPos2i(x+mWidth, y);

        //prevent mirroring?
        glPixelZoom(-1,1);
        glDrawPixels(mWidth,mHeight, GL_RGB, GL_UNSIGNED_BYTE, mImage);
    }

    void texture() {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            mWidth,
            mHeight,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            mImage);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
};

Image texture1;
Image texture2;
Image texture3;

void intializeHeightMap(){
    for ( int i = 0; i < planeSize; i++){
        for ( int j = 0; j < planeSize; j++){
            heightMap[i][j] = 0;
        }
    }
}

void circleAlgo(int x, int y){

    int radius = static_cast <int> (rand()) % 300 + 1;
    float disp = static_cast <int> (rand()) % maxHeight + 1;
    
    for ( int i = 0; i < planeSize; i++){
        for ( int j = 0; j < planeSize; j++){
            int dx = i - x;
            int dy = j - y;
            float distance = sqrtf((dx*dx) + (dy*dy));
            float pd = (distance*2)/radius; 
            if (fabs(pd) <= 1.0){   
                    heightMap[i][j] += (disp/2.0) + (cos(pd*3.14)*(disp/2.0));
                }

        }
    }
}

void fillHeightMap(){

    numOfMountains = planeSize/8;

    intializeHeightMap();

    // Create initial random hills and add them to vector
    for ( int i = 0; i < numOfMountains; i++){
        int x = static_cast <int> (rand()) % planeSize;
        int y = static_cast <int> (rand()) % planeSize; 
        // mountains.push_back({x, y});
        // cout << x << " " << y << "\n";
        circleAlgo(x,y); 
    }
}

void calculateNormal(float* i, float* i2, float* i3){

    float* v1 = new float[3];
    float* v2 = new float[3];

    float* r = new float[3];

    v1[0] = i2[0] - i[0];
    v1[1] = i2[1] - i[1];
    v1[2] = i2[2] - i[2];

    v2[0] = i3[0] - i[0];
    v2[1] = i3[1] - i[1];
    v2[2] = i3[2] - i[2];


    r[0] = v1[1]*v2[2] - v1[2]*v2[1];
    r[1] = v1[2]*v2[0] - v1[0]*v2[2];
    r[2] = v1[0]*v2[1] - v1[1]*v2[0];

    float sum = abs(r[0]) +  abs(r[1]) + abs(r[2]);


    r[0] /= sum;
    r[1] /= sum;
    r[2] /= sum;

    glNormal3f(r[0], r[1], r[2]);


}

void update(){}

void drawNormalPlane(){
    glPushMatrix();
    for ( int a = 1; a < planeSize; a++){
    
        for ( int b = 1; b < planeSize; b++){

            isTriangleMode ? glBegin(GL_TRIANGLES) : glBegin(GL_QUADS);

            if ( a+1 < planeSize && b+1 < planeSize ){
                
                if (isTriangleMode){

                    if ( isDrawingWireFrame){
                
                        glColor3f(0,0.7,0.1);
                        glVertex3f(a, b, heightMap[a][b]+2);
                        glVertex3f(a+1, b, heightMap[a+1][b]+2);
                        glVertex3f(a, b+1, heightMap[a][b+1]+2);

                        glVertex3f(a, b, heightMap[a][b]+2);
                        glVertex3f(a, b-1, heightMap[a][b-1]+2);
                        glVertex3f(a-1, b, heightMap[a-1][b]+2);
                    } else {

                        float a1[3] = {a,b,heightMap[a][b]};
                        float a2[3] = {a+1,b, heightMap[a+1][b]};
                        float a3[3] = {a,b+1, heightMap[a][b+1]};

                        float* v1 = a1;
                        float* v2 = a2;
                        float* v3 = a3; 

                        glColor3f(heightMap[a][b]/maxHeight, maxHeight/heightMap[a][b], 0.5f);
                        calculateNormal(v1,v2,v3);
                        glVertex3f(a, b, heightMap[a][b]);
                        
                        
                        glColor3f(heightMap[a+1][b]/maxHeight, maxHeight/heightMap[a+1][b], 0.5f);
                        calculateNormal(v2,v1,v3);
                        glVertex3f(a+1, b, heightMap[a+1][b]);
                        
                        
                        glColor3f(heightMap[a][b+1]/maxHeight, maxHeight/heightMap[a][b+1], 0.5f);
                        calculateNormal(v3,v1,v2);
                        glVertex3f(a, b+1, heightMap[a][b+1]);

                        float a4[3] = {a,b,heightMap[a][b]};
                        float a5[3] = {a - 1, b, heightMap[a - 1][b]};
                        float a6[3] = {a,b-1, heightMap[a][b-1]};

                        float* v4 = a4;
                        float* v5 = a5;
                        float* v6 = a6;


                        glColor3f(heightMap[a-1][b]/maxHeight, maxHeight/heightMap[a-1][b], 0.5f);
                        calculateNormal(v5,v6,v4);
                        glVertex3f(a-1, b, heightMap[a-1][b]);  

                        glColor3f(heightMap[a][b-1]/maxHeight, maxHeight/heightMap[a][b-1], 0.5f);
                        calculateNormal(v6,v4,v5);
                        glVertex3f(a, b-1, heightMap[a][b-1]);

                        glColor3f(heightMap[a][b] / maxHeight, maxHeight / heightMap[a][b], 0.5f);
                        calculateNormal(v1, v2, v3);
                        glVertex3f(a, b, heightMap[a][b]);
                    }
                } else {

                    float a1[3] = {a, b, heightMap[a][b]};
                    float a2[3] = {a + 1, b, heightMap[a + 1][b]};
                    float a3[3] = {a, b + 1, heightMap[a][b + 1]};
                    float a4[3] = {a+1,b+1,heightMap[a+1][b+1]};

                    float* v1 = a1;
                    float* v2 = a2;
                    float* v3 = a3;
                    float* v4 = a4;

                    if ( isDrawingWireFrame){
                        glColor3f(0,0.7,0.1);
                        glVertex3f(a, b, heightMap[a][b]+2);
                        glVertex3f(a, b+1, heightMap[a][b+1]+2);
                        glVertex3f(a+1, b+1, heightMap[a+1][b+1]+2);
                        glVertex3f(a+1, b , heightMap[a+1][b]+2);
                    } else {

                        glColor3f(heightMap[a + 1][b] / maxHeight, maxHeight / heightMap[a][b], 0.5f);
                        calculateNormal(v2, v1, v3);
                        glVertex3f(a + 1, b, heightMap[a + 1][b]);

                        glColor3f(heightMap[a + 1][b + 1] / maxHeight, maxHeight / heightMap[a + 1][b + 1], 0.5f);
                        calculateNormal(v4, v3, v2);
                        glVertex3f(a + 1, b + 1, heightMap[a + 1][b + 1]);

                        glColor3f(heightMap[a][b + 1] / maxHeight, maxHeight / heightMap[a][b + 1], 0.5f);
                        calculateNormal(v3,v1,v2);
                        glVertex3f(a, b + 1, heightMap[a][b + 1]);

                        glColor3f(heightMap[a][b] / maxHeight, maxHeight / heightMap[a][b], 0.5f);
                        calculateNormal(v1, v3, v2);
                        glVertex3f(a, b, heightMap[a][b]);
                    }
                }
            
            }
            glEnd();
        }
    }
    glPopMatrix();
}



void createPlane(){
   
    if ( wireframe == 0){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawNormalPlane();    
    }

    else if ( wireframe == 1 ){
        
        isDrawingWireFrame = true;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawNormalPlane();
        isDrawingWireFrame = false;
    }
    
    else if ( wireframe == 2){


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawNormalPlane();


        isDrawingWireFrame = true;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawNormalPlane();
        isDrawingWireFrame = false;
        
    }
}

void faultAlgo()
{
    
    for (int z = 0; z < 10; z++) {

        int v = static_cast<float>(rand());
        float a = sin(v);
        float b = cos(v);
        float d = sqrt(planeSize * planeSize + planeSize * planeSize);

        float c = (static_cast<float> (rand()) / static_cast<float> (RAND_MAX)) * d - d / 2;
        for (int i = 0; i < planeSize; i++)
        {
            for (int j = 0; j < planeSize; j++)
            {
                if (a * i + b * j - c > 0)
                {
                    heightMap[i][j] += displacement;
                    if(heightMap[i][j] > maxHeight) {
                        maxHeight = heightMap[i][j];
                    }
                }
                else
                {
                    heightMap[i][j] -= displacement;
                    if (heightMap[i][j] < minHeight)
                    {
                        minHeight = heightMap[i][j];
                    }
                }
            }
        }
    }
    
}

void midPointAlgo()
{
    int disp = 5;
    for (int i = 2; i < planeSize; i += 2)
    {
        for (int j = 2; j < planeSize; j += 2)
        {
            float &midpoint = heightMap[i][j];
            // std::cout << midpoint << std::endl;

            float a = heightMap[i-1][j-1];
            float b = heightMap[i-1][j+1];
            float c = heightMap[i+1][j-1];
            float d = heightMap[i+1][j+1];

            midpoint = (a + b + c + d) / 4 + rand() % disp;

            if(midpoint > maxHeight){
                maxHeight = midpoint;
            }    
        }
    }
}

void reshape( int w, int h){
    gWidth = w;
    gHeight = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,w,h);

    glutPostRedisplay();
}


void draw3DScene(){
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 2000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], (GLdouble)planeSize, (GLdouble)planeSize, 0, 0, 0, 1);

    //call texture on marbletexture to set our texture paramaters for wireframe.
    // texture1.texture();
    // texture2.texture();

    if(texCounter == 1){
        texture2.texture();
    } else if (texCounter == 2){
        texture3.texture();
    } else if(texCounter == 3){
        texture1.texture();
    }

    createPlane();
    
    

}


void FPS(int val){
    if (!isPaused){
        glutPostRedisplay();
    }
    glutTimerFunc(17, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw3DScene();

    glFlush();
    glutSwapBuffers();

}

void kbd(unsigned char key, int x, int y)
{
    switch(key){
        case 'f':
            faultAlgo();
            maxHeight = 30;
            break;
        case 'm':
            midPointAlgo();
            maxHeight = 30;
            break;
        case 'l':
            isLightOn = !isLightOn;
            isLightOn ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING); 
            break;
        case 'p':
            isFlatShadingOn = !isFlatShadingOn;
            isFlatShadingOn ? glShadeModel(GL_FLAT) : glShadeModel(GL_SMOOTH); 
            break;
        case 'r':
            fillHeightMap();
            break;
        case 's':
            isTriangleMode = !isTriangleMode;
            break;
        case 'w':
            if ( wireframe > 1){
                wireframe = 0;
            }
            else {
                wireframe++;
            }

            createPlane();
            break;
       case 't':
            //greater than or equal to three since we need to include terrain without texture
            if (texCounter > 2){
                texCounter = 0;
            } else {
                texCounter += 1;
            }
            break;
       case 'q':
       case 27:
           exit(0);
           break;
    }
    glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            eye[2]++;        
        break;

        case GLUT_KEY_DOWN:
            eye[2]--;
        break;

        case GLUT_KEY_LEFT:
            eye[0]--;        
        break;

        case GLUT_KEY_RIGHT:
            eye[0]++;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    printf("How big would you like the world to be?\n");
    scanf("%d", &planeSize);
    printf("\n"
           "R -> reset\n"
           "Q/esc -> quit\n"
           "L -> toggle lighting\n"
           "T -> switch between textures\n"
           "P -> toggle shading\n"
           "S -> toggle polygon mesh\n"
           "W -> switch between wireframe representations\n"
           "up arrow -> move camera up\n"
           "down arrow -> move camera down\n"
           "left arrow -> move camera left\n"
           "right arrow -> move camera right\n"
           "ADDITION CAPABILITIES:\n"
           "F -> Fault Algorithm Applied to Terrain\n"
           "M -> Midpoint Displacement Algorithm applied to Terrain\n");

    //load textures
    texture1.load((char*)"marble.ppm");
    texture2.load((char*)"carpet.ppm");
    texture3.load((char*)"snail_a.ppm");

    fillHeightMap();
    glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
    glutInitWindowPosition(300,300);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 4");
	glutTimerFunc(0, FPS, 0);
    
    glutReshapeFunc(reshape);

	glutKeyboardFunc(kbd);
    glutSpecialFunc(SpecialInput);
	glutDisplayFunc(display);

    glEnable(GL_DEPTH_TEST);
    // glColor3f(1, 1, 1);
    glEnable(GL_NORMALIZE);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos[0]);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec[0]);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos[1]);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb[1]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff[1]);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec[1]);

    glClearColor(0, 0, 0, 0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    

    glutMainLoop();

	return 0;
}




