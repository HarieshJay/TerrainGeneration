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




void update(){


   
}


void drawNormalPlane(){

    for ( int a = 0; a < planeSize; a++){
    
        for ( int b = 0; b < planeSize; b++){


            glBegin(GL_QUADS);
            if ( a+1 < planeSize && b+1 < planeSize){

                // cout << a << " " << b << " " << heightMap[a][b] << "\n";
                
                glVertex3f(a, b, heightMap[a][b]);
                glVertex3f(a, b+1, heightMap[a][b+1]);
                glVertex3f(a+1, b+1, heightMap[a+1][b+1]);
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
       

    }


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

    
    glutReshapeFunc(reshape);
	// glEnable(GL_DEPTH_TEST);

    // glEnable(GL_CULL_FACE);
    glCullFace( GL_BACK );
    

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glClearColor(0, 0, 0, 0);

	glutKeyboardFunc(kbd);
    glutSpecialFunc(SpecialInput);
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}




