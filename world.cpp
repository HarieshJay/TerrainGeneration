#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
// #include <bits/stdc++.h> 

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;


int gHeight = 400;
int gWidth = 400;
bool isPaused = false;
int wireframe = 0;

const int planeSize = 1000;
// float eye[3] = {planeSize/2, planeSize/2 ,10};
float eye[3] = {0, 0, 40};
int numOfMountains = planeSize/8;
// vector<int[2]> mountains = vector<int[2]>();
float heightMap[planeSize][planeSize];

float light_pos[2][4] = {{3,5,800,1}, {1,-5,400,1} };
float amb[2][4] = { {0,0,1,0.5} , {0,0.2,1,0.5} };
float diff[2][4] = { {0,0,1,1} , {1,0,0,1}  };
float spec[2][4] = { {1,1,1,1}, {1,0,1,1}  };


// void setMaterials(unsigned int index) {
    
//     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient[index]);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse[index]);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular[index]);
//     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiny[index]);

// }

//global texture values
//an array for image data
GLubyte *tex1;
GLubyte *tex2;
// GLubyte *tex3;

GLuint textures[2];
int texCounter = 0;

int width, height, maximum;
int width2, height2, maximum2;
// int width3, height3, maximum3;

void intializeHeightMap(){
    for ( int i = 0; i < planeSize; i++){
        for ( int j = 0; j < planeSize; j++){
            heightMap[i][j] = 0;
        }
    }
}

void circleAlgo(int x, int y){

    int radius = static_cast <int> (rand()) % 300 + 1;
    float disp = static_cast <int> (rand()) % 30 + 1;
    
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

void update(){}

void drawNormalPlane(){

    for ( int a = 0; a < planeSize; a++){
    
        for ( int b = 0; b < planeSize; b++){


            glBegin(GL_QUADS);
            if ( a+1 < planeSize && b+1 < planeSize){

                // cout << a << " " << b << " " << heightMap[a][b] << "\n";
                
                // glTexCoord2f(0,0);
                glVertex3f(a, b, heightMap[a][b]);

                // glTexCoord2f(0, 1);
                glVertex3f(a, b+1, heightMap[a][b+1]);

                // glTexCoord2f(1, 1);
                glVertex3f(a+1, b+1, heightMap[a+1][b+1]);

                // glTexCoord2f(1, 0);
                glVertex3f(a+1, b , heightMap[a+1][b]);
            }
            glEnd();
        }
    }
}

void createPlane(){
   
    if ( wireframe == 0){
        glColor3f(1,0,0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawNormalPlane();    
    }

    else if ( wireframe == 1 ){
        glColor3f(1,0,0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawNormalPlane();
    }
    
    else if ( wireframe == 2){
        glColor3f(1,0,0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawNormalPlane();
        glColor3f(1,1,1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawNormalPlane();
    }
}

//setting the textures
void setTex()
{
    if(texCounter == 0){
        //wireframe setting
        // glColor3f(1, 0, 0);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // drawNormalPlane();
        // glColor3f(1,1,1);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // drawNormalPlane();
    } else if (texCounter == 1) {
        glColor3f(0,0,0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        drawNormalPlane();
    } else if (texCounter == 2) {
        glColor3f(0,1,1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        drawNormalPlane();
    } //else if (texCounter == 3) {
    //     glColor3f(0, 0, 1);
    //     glBindTexture(GL_TEXTURE_2D, textures[2]);
    //     drawNormalPlane();
    // }
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
    gluPerspective(45, 1, 1, planeSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], planeSize, planeSize, 0, 0, 0, 1);
    
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

    draw3DScene();
    glutSwapBuffers();

}

void kbd(unsigned char key, int x, int y)
{
    switch(key){

        case 'r':
            fillHeightMap();
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
            if (texCounter > 1){
                texCounter = 0;
            } else {
                texCounter += 1;
            }
            setTex();
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

/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 *  usage: GLubyte myImg = LoadPPM("myImg.ppm", &width, &height, &max);
 */
GLubyte *LoadPPM(char *file, int *width, int *height, int *max)
{
    GLubyte *img;
    FILE *fd;
    int n, m;
    int k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;

    fd = fopen(file, "r");
    fscanf(fd, "%[^\n] ", b);
    if (b[0] != 'P' || b[1] != '3')
    {
        printf("%s is not a PPM file!\n", file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c", &c);
    while (c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);

    printf("%d rows  %d columns  max value= %d\n", n, m, k);

    nm = n * m;

    img = (GLubyte *)malloc(3 * sizeof(GLuint) * nm);

    s = 255.0 / k;

    for (i = 0; i < nm; i++)
    {
        fscanf(fd, "%d %d %d", &red, &green, &blue);
        img[3 * nm - 3 * i - 3] = red * s;
        img[3 * nm - 3 * i - 2] = green * s;
        img[3 * nm - 3 * i - 1] = blue * s;
    }

    *width = n;
    *height = m;
    *max = k;

    return img;
}

void init(void)
{

    glMatrixMode(GL_TEXTURE);
    // glScalef(1, 1, 1);

    tex1 = LoadPPM("carpet.ppm", &width, &height, &maximum);

    tex2 = LoadPPM("marble.ppm", &width2, &height2, &maximum2);

    // tex3 = LoadPPM("carpet.ppm", &width3, &height3, &maximum3);

    glEnable(GL_TEXTURE_2D);

    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex1);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, tex2);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glBindTexture(GL_TEXTURE_2D, textures[2]);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, tex3);

    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glClearColor(0, 0, 0, 0);
    // glColor3f(1, 1, 1);

    glEnable(GL_COLOR_MATERIAL);
    // glEnable(GL_LIGHTING);
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(45, 1, 1, 100);
}

int main(int argc, char** argv)
{
    // printf("\n"
    //     "r -> reset\n"
    //     "q -> quit\n"
    //     "space -> stop simulation\n"
    //     "l -> toggle lights\n"
    //     "f -> add friction\n"
    //     "p -> pause simulation\n"
    //     "r -> reset\n"
    //     "up arrow -> move camera up\n"
    //     "down arrow -> move camera down\n"
    //     "left arrow -> move camera left\n"
    //     "right arrow -> move camera right\n"
    //     "LIGHT1:\n"
    //     "1 -> move x in positive\n"
    //     "2 -> move x in negative\n"
    //     "3 -> move y in positive\n"
    //     "4 -> move y in negative\n"
    //     "5 -> move z in positive\n"
    //     "6 -> move z in negative\n"
    //     "LIGHT2:\n"
    //     "z -> move x in positive\n"
    //     "x -> move x in negative\n"
    //     "c -> move y in positive\n"
    //     "v -> move y in negative\n"
    //     "b -> move z in positive\n"
    //     "n -> move z in negative\n"
    //     "ADDITION CAPABILITIES:\n"
    //     "m -> follow a particle\n"
    //     "t -> add various types of particles and momentums\n");

    // for (int i = 0; i < planeSize; ++i)
    // {
    //     for (int j = 0; j < planeSize; ++j)
    //     {
    //         std::cout << heightMap[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
	fillHeightMap();
	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 4");
	glutTimerFunc(0, FPS, 0);
    
    glutReshapeFunc(reshape);
	

    // glEnable(GL_CULL_FACE);
    glCullFace( GL_BACK );

	glutKeyboardFunc(kbd);
    glutSpecialFunc(SpecialInput);
	glutDisplayFunc(display);

    glEnable(GL_DEPTH_TEST);
    init();

    glutMainLoop();

	return 0;
}




