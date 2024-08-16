#include<windows.h>
#include<gl/glut.h>
#include<stdlib.h>
#include <math.h>
#include <iostream>
#include <array>
#include <iomanip> // diperlukan untuk std::setprecision
#include <cstdio>

using namespace std;
#include "imageloader.h"

GLuint _textureId[5];
void init(void);
void tampil(void);
void ukuran(int, int);

float polarAngle = 90.0f;       // Sudut antara vektor kamera dan sumbu y (0 - 180 derajat)
float azimuthalAngle = 0.0f;

//float centerX = -3.35549f;
//float centerY = 29.3321f;
//float centerZ = 95.6915f;

float centerX = 0.0f;
float centerY = 0.0f;
float centerZ = 0.0f;

float radius = 50.0f;

int lastMouseX = -1;
int lastMouseY = -1;

float moveSpeed = 1.0f;

// Variabel global untuk menyimpan posisi kamera
float cameraX = 0.0f;// = 0.0f;
float cameraY = 0.0f; // Atur ketinggian kamera
float cameraZ = 0.0f;// Atur jarak depan bangunan
float angleSpeed = 0.1f;

bool mousePressed = false;

float zoomSpeed=1.0f;

//void mouseWheel(int button, int dir, int x, int y) {
//    if (dir > 0) { // Scroll ke atas (zoom in)
//        radius -= zoomSpeed; // Ubah radius sesuai zoomSpeed
//    } else { // Scroll ke bawah (zoom out)
//        radius += zoomSpeed; // Ubah radius sesuai zoomSpeed
//    }
//
//    // Batasan untuk zoom (misalnya, radius tidak boleh negatif)
//    if (radius < 1.0f) radius = 1.0f; // Atur zoom minimal sesuai kebutuhan
//
//    // Meminta pembaruan tampilan
//    glutPostRedisplay();
//}



void cameraLog(){
    std::cout << std::fixed << std::setprecision(4) << std::showpos << std::internal;
//    std::cout << std::setw(10) << "Camera (x, y, z): " << cameraX << ", " <<  cameraY << ", " <<  cameraZ << " | Center (x, y, z): " << centerX << ", " <<  centerY << ", " <<  centerZ << "\n";
//    printf("[INFO] Camera (x, y, z): %+10.4f, %+10.4f, %+10.4f | Center (x, y, z): %+10.4f, %+10.4f, %+10.4f\n",
//               cameraX, cameraY, cameraZ, centerX, centerY, centerZ);
}

void drawTetragonf(const GLfloat (&firstVertex)[3], const GLfloat (&secondVertex)[3], const GLfloat (&thirdVertex)[3], const GLfloat (&fourthVertex)[3] ){
    glVertex3f(firstVertex[0], firstVertex[1], firstVertex[2]);
    glVertex3f(secondVertex[0], secondVertex[1], secondVertex[2]);
    glVertex3f(thirdVertex[0], thirdVertex[1], thirdVertex[2]);
    glVertex3f(fourthVertex[0], fourthVertex[1], fourthVertex[2]);
}

void drawTetragonfWithGlBeginEnd(const GLfloat (&firstVertex)[3], const GLfloat (&secondVertex)[3], const GLfloat (&thirdVertex)[3], const GLfloat (&fourthVertex)[3] ){
    glBegin(GL_QUADS);
    glVertex3f(firstVertex[0], firstVertex[1], firstVertex[2]);
    glVertex3f(secondVertex[0], secondVertex[1], secondVertex[2]);
    glVertex3f(thirdVertex[0], thirdVertex[1], thirdVertex[2]);
    glVertex3f(fourthVertex[0], fourthVertex[1], fourthVertex[2]);
    glEnd();
}

void drawTetragonfWithTexture(const GLfloat (&firstVertex)[3], const GLfloat (&secondVertex)[3], const GLfloat (&thirdVertex)[3], const GLfloat (&fourthVertex)[3], GLuint texture, GLfloat s, GLfloat p ){
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, texture);

//    glColor3f(0.5, 0, 0.5);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(firstVertex[0], firstVertex[1], firstVertex[2]);
    glTexCoord2f(s, 0.0);glVertex3f(secondVertex[0], secondVertex[1], secondVertex[2]);
    glTexCoord2f(s, p);glVertex3f(thirdVertex[0], thirdVertex[1], thirdVertex[2]);
    glTexCoord2f(0.0, p);glVertex3f(fourthVertex[0], fourthVertex[1], fourthVertex[2]);
    glEnd();

    glDisable(GL_TEXTURE_2D);
//    glBegin(GL_QUADS);

//    glVertex3f(firstVertex[0], firstVertex[1], firstVertex[2]);
//    glVertex3f(secondVertex[0], secondVertex[1], secondVertex[2]);
//    glVertex3f(thirdVertex[0], thirdVertex[1], thirdVertex[2]);
//    glVertex3f(fourthVertex[0], fourthVertex[1], fourthVertex[2]);
}

//untuk daerah depan
float height = 70, jarak;
bool keys[256] = { false };
bool  ctrl = false;

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
//    if(glutGetModifiers() == GLUT_ACTIVE_CTRL){
//        std::cout<<"tes\n";
//        ctrl = true;
//    }
//    std::cout << " telah terjadi klik\n" ;

}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
//    if(glutGetModifiers() != GLUT_ACTIVE_CTRL){
//        ctrl = false;
//        std::cout<<"tes\n";
//    }
//    std::cout << " telah terjadi klik lepas\n" ;
}

void updateCameraPosition() {
    float directionX = sin(azimuthalAngle * 3.14159265 / 180) * sin(polarAngle * 3.14159265 / 180);
//    float directionY = cos(polarAngle * 3.14159265 / 180);
    float directionZ = cos(azimuthalAngle * 3.14159265 / 180) * sin(polarAngle * 3.14159265 / 180);

    float rightX = sin(azimuthalAngle * 3.14159265 / 180 + 3.14159265 / 2);
//    float rightY = 0;
    float rightZ = cos(azimuthalAngle * 3.14159265 / 180 + 3.14159265 / 2);

//    std::cout << "rightX, rightZ = " << rightX << ", " << rightZ << "\n";
    float moveSpeed = 0.5f; // Kecepatan gerakan kamera

    bool isChanged = false;

    if (keys['w']) {
        isChanged = true;
        centerX -= directionX * moveSpeed;
        centerZ -= directionZ * moveSpeed;
    }
    if (keys['s']) {
        isChanged = true;
        centerX += directionX * moveSpeed;
        centerZ += directionZ * moveSpeed;
    }
    if (keys['a']) {
        isChanged = true;
        centerX -= rightX * moveSpeed;
        centerZ -= rightZ * moveSpeed;
    }
    if (keys['d']) {
        isChanged = true;
        centerX += rightX * moveSpeed;
        centerZ += rightZ * moveSpeed;
    }
    if (keys[32]) { // Tombol Spasi
        isChanged = true;
        centerY += moveSpeed;
    }
    if (keys['x']) { // Tombol Ctrl
        isChanged = true;
        centerY -= moveSpeed;
    }
    if (keys['-']) { // Tombol - (misalnya, bisa disesuaikan dengan key yang Anda inginkan)
        radius += zoomSpeed; // zoomSpeed bisa merupakan nilai yang ditetapkan sesuai dengan kebutuhan Anda
    }
    if (keys['=']) { // Tombol + (misalnya, bisa disesuaikan dengan key yang Anda inginkan)
        radius -= zoomSpeed; // zoomSpeed bisa merupakan nilai yang ditetapkan sesuai dengan kebutuhan Anda
        if (radius < 0.001f) radius = 0.001f; // Batasan zoom in
    }

    if(isChanged){
        cameraLog();
    }

    // Update posisi kamera
    cameraX = centerX + radius * sin(polarAngle * 3.14159265 / 180) * sin(azimuthalAngle * 3.14159265 / 180);
    cameraY = centerY + radius * cos(polarAngle * 3.14159265 / 180);
    cameraZ = centerZ + radius * sin(polarAngle * 3.14159265 / 180) * cos(azimuthalAngle * 3.14159265 / 180);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            lastMouseX = -1;
            lastMouseY = -1;
        } else if (state == GLUT_UP) {
            mousePressed = false;
        }
    }
}

void motion(int x, int y) {
//    std::cout << "x, y = " << x << ", " << y << "\n";
    if (mousePressed) {
        if (lastMouseX == -1 && lastMouseY == -1) {
            // Jika ini adalah gerakan pertama setelah mouse ditekan, simpan posisi mouse
            lastMouseX = x;
            lastMouseY = y;
        } else {
            // Hitung perubahan sudut berdasarkan pergerakan mouse
            azimuthalAngle -= (x - lastMouseX) * angleSpeed;
            polarAngle += (lastMouseY - y) * angleSpeed;

            // Batasi sudut polar agar tidak melebihi batas atas dan bawah
            if (polarAngle > 180.0f) polarAngle = 180.0f;
            if (polarAngle < 0.0f) polarAngle = 0.0f;

            // Atur kembali posisi mouse sebelumnya ke posisi mouse saat ini
            lastMouseX = x;
            lastMouseY = y;
            updateCameraPosition();
            glutPostRedisplay();
        }
//        std::cout << "x, y = " << x << ", " << y << " | polar, azimuthal = " << polarAngle << ", " << azimuthalAngle << "\n";
        cameraLog();
    }
}

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	return textureId;
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    string direktori;
    for (int i = 1; i <= 2; i++){
        //ADJUST THE DIRECTORY BASED ON PATH IN YOUR DESKTOP
        direktori = "D:/ALL-ABOUT-PROGRAMMING/TEXT-EDITOR/GithubRepository/chilllikehuman/tomoro-salatiga-3d/textures/" + to_string(i) + ".bmp";
        Image* image = loadBMP(direktori.c_str());
        _textureId[i] = loadTexture(image);
        delete image;
    }
}



void mejaBundar() {
    GLUquadric* quad = gluNewQuadric();
   // glColor3f(0.55f, 0.27f, 0.07f); // Warna coklat

    // Kaki meja
    glPushMatrix();
    glColor3ub(105, 105, 105); // dim gray
    glTranslatef(0.0, -10.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0); // Mengatur orientasi kaki meja
    gluCylinder(quad, 1.0, 1.0, 20.0, 32, 32);
    glPopMatrix();

    // Permukaan meja
    glPushMatrix();
     glColor3ub(221,218,221);
    glTranslatef(0.0, 10.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0); // Mengatur orientasi permukaan meja
    gluCylinder(quad, 10.0, 10.0, 1.0, 32, 32);
    gluDisk(quad, 0.0, 10.0, 32, 1);
    glTranslatef(0.0, 0.0, 1.0);
    gluDisk(quad, 0.0, 10.0, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void bangkuKayu() {
    GLUquadric* quad = gluNewQuadric();
    //glColor3f(0.55f, 0.27f, 0.07f); // Warna coklat
    glColor3ub(205, 133, 63);
    // Kaki bangku
    for (float x = -2.0; x <= 2.0; x += 4.0) {
        for (float z = -2.0; z <= 2.0; z += 4.0) {
            glPushMatrix();
            glTranslatef(x, -10.0, z); // Posisi kaki bangku lebih rendah
            glRotatef(-90, 1.0, 0.0, 0.0); // Orientasi kaki bangku tegak
            gluCylinder(quad, 0.5, 0.5, 10.0, 32, 32);
            glPopMatrix();
        }
    }

    // Permukaan tempat duduk
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0); // Orientasi tempat duduk datar
    gluCylinder(quad, 5.0, 5.0, 1.0, 32, 32);
    gluDisk(quad, 0.0, 5.0, 32, 1);
    glTranslatef(0.0, 0.0, 1.0);
    gluDisk(quad, 0.0, 5.0, 32, 1);
    glPopMatrix();
    gluDeleteQuadric(quad);
}

void tempatDudukBesar(GLfloat length){

    GLfloat distance = length/2;
    //kursi
    glBegin(GL_QUADS);
    //glColor3f(0,0.5,0.7);
     glColor3ub(139, 69, 19);


    //sisi kiri
    drawTetragonf({-distance, 0.0, -9.0}, {-distance, 0.0, 0.0}, {-distance, 12.0, 0.0}, {-distance, 12.0, -9.0});

    //sisi depan
    drawTetragonf({-distance, 0.0, 0.0}, {distance, 0.0, 0.0}, {distance, 12.0, 0.0}, {-distance, 12.0, 0.0});

    //sisi kanan
    drawTetragonf({distance, 0.0, -9.0}, {distance, 0.0, 0.0}, {distance, 12.0, 0.0}, {distance, 12.0, -9.0});

    //sisi atas (permukaan kursi)
    drawTetragonf({-distance, 12, 0.0}, {distance, 12, 0.0}, {distance, 12, -10.0}, {-distance, 12, -10.0});

    //sisi belakang (tempat bersandar)
   // glColor3f(0.4, 0.5 , 0);
   glColor3ub(205, 133, 63);
    //sisi depan
    drawTetragonf({-distance, 0.0, -9.0}, {distance, 0.0, -9.0}, {distance, 24, -9.0}, {-distance, 24, -9.0});

    //sisi kanan
    drawTetragonf({distance, 0.0, -9.0}, {distance, 0.0, -10.0}, {distance, 12, -10.0}, {distance, 24, -9.0});

    //sisi atas
    drawTetragonf({-distance, 24.0, -9.0}, {distance, 24.0, -9.0}, {distance, 24, -10.0}, {-distance, 24, -10.0});

    //sisi kiri
    drawTetragonf({-distance, 0.0, -9.0}, {-distance, 0.0, -10.0}, {-distance, 24, -10.0}, {-distance, 24, -9.0});

    //sisi belakang
    drawTetragonf({distance, 0.0, -10.0}, {-distance, 0.0, -10.0}, {-distance, 24, -10.0}, {distance, 24, -10.0});

    //hiasan kursi besar (tempat stopkontak listrik)
   // glColor3f(1,0,0);
   glColor3ub(205, 133, 63);

    //sisi kiri
    drawTetragonf({-distance, 0.5, 0.0}, {-distance, 0.5, 1.0}, {-distance, 10, 1.0}, {-distance, 10, 0.0});

    //bagian depan
    drawTetragonf({-distance, 0.5, 1.0}, {distance, 0.5, 1.0}, {distance, 10, 1.0}, {-distance, 10, 1.0});

    //sisi kanan
    drawTetragonf({distance, 0.5, 0.0}, {distance, 0.5, 1.0}, {distance, 10, 1.0}, {distance, 10, 0.0});
    drawTetragonf({-distance, 10, 1.0}, {distance, 10.0, 1.0}, {distance, 10, 0.0}, {-distance, 10, 0.0});

    glEnd();
}

void pintuMasukCafe(){
    // Pintu masuk ke cafe
    glBegin(GL_QUADS);
    //glColor3f(0, 1, 0);
     glColor3ub(220, 220, 220);
    drawTetragonf({-70.0, height-9.0f, -68.0}, {70.0, height-9.0f, -68.0}, {70.0, height, -68.0}, {-70.0, height, -68.0});
    drawTetragonf({-70.0, height-9.0f, -68.0}, {70.0, height-9.0f, -68.0}, {70.0, height-9.0f, -72.0}, {-70.0, height-9.0f, -72.0});
    drawTetragonf({-70.0, height-9.0f, -72.0}, {70.0, height-9.0f, -72.0}, {70.0, height, -72.0}, {-70.0, height, -72.0});

    // kaca pada pintu masuk cafe
    float jarak;
    //glColor3f(0,0,0);
    glColor3ub(245, 245, 245);

    // frame kaca vertikal
    jarak = 0;
    for(int i = 0; i < 5; i++){
        drawTetragonf({-66.0f+jarak, -20.0, -69.0}, {-64.0f+jarak, -20.0, -69.0}, {-64.0f+jarak, height-10, -69.0}, {-66.0f+jarak, height-10, -69.0});
        drawTetragonf({-66.0f+jarak, -20.0, -72.0}, {-64.0f+jarak, -20.0, -72.0}, {-64.0f+jarak, height-10, -72.0}, {-66.0f+jarak, height-10, -72.0});
        jarak += 18;
    }

    // frame kaca horizontal
    //bagian bawah
    drawTetragonf({-70.0, -20.0, -69.0}, {70.0, -20.0, -69.0}, {70.0, -20+8, -69.0}, {-70.0, -20+8, -69.0});
    drawTetragonf({-70.0, -20.0, -71.0} ,{70.0, -20.0, -71.0}, {70.0, -20+8, -71.0}, {-70.0, -20+8, -71.0});

    //bagian tengah
    drawTetragonf({-70.0, 14.0, -69.0}, {70.0, 14.0, -69.0}, {70.0, 16, -69.0}, {-70.0, 16, -69.0});
    drawTetragonf({-70.0, 14.0, -71.0}, {70.0, 14.0, -71.0}, {70.0, 16, -71.0}, {-70.0, 16, -71.0});

    //bagian atas
    drawTetragonf({-70.0, height-10, -69.0}, {70.0, height-10, -69.0}, {70.0, height-9, -69.0}, {-70.0, height-9, -69.0});
    drawTetragonf({-70.0, height-10, -71.0}, {70.0, height-10, -71.0}, {70.0, height-9, -71.0}, {-70.0, height-9, -71.0});

    // tembok pilar
    //glColor3f(0.5,0.4,1);
        glColor3ub(220, 220, 220);
    //bagian kiri
    drawTetragonf({7.0, -20.0, -75.0}, {7.0, -20.0, -65.0}, {7.0, height, -65.0}, {7.0, height, -75.0});

    //bagian kanan
    drawTetragonf({17.0, -20.0, -75.0}, {17.0, -20.0, -65.0}, {17.0, height, -65.0}, {17.0, height, -75.0});

    //bagian depan
    drawTetragonf({7.0, -20.0, -65.0}, {17.0, -20.0, -65.0}, {17.0, height, -65.0}, {7.0, height, -65.0});
    drawTetragonf({7.0, -20.0, -75.0}, {17.0, -20.0, -75.0}, {17.0, height, -75.0}, {7.0, height, -75.0});

    //frame kaca 2
    //glColor3f(0,0,0);
     glColor3ub(245, 245, 245);
    jarak = 0;
    for(int i = 0; i < 4; i++){
        drawTetragonf({16.0f+jarak, -20.0, -69.0}, {18.0f+jarak, -20.0, -69.0}, {18.0f+jarak, height-10, -69.0}, {16.0f+jarak, height-10, -69.0});
        drawTetragonf({16.0f+jarak, -20.0, -71.0}, {18.0f+jarak, -20.0, -71.0}, {18.0f+jarak, height-10, -71.0}, {16.0f+jarak, height-10, -71.0});
        jarak += 17;
    }

    glColor4f(0.7, 0.7, 0.7, 0.5);
    drawTetragonf({-70.0, -20.0, -70.0},
                  {70.0, -20.0, -70.0},
                  {70.0, height-10, -70.0},
                  {-70.0, height-10, -70.0});

    glEnd();
}

void mejaKasir(){
    //glColor3f(0.8,0.8,0.8);
    glColor3ub(	248, 248, 255);
    glBegin(GL_QUADS);

    // samping kiri
    drawTetragonf({-70,-20,-74},
                {-14.5,-20, -74},
                {-14.5, 10, -74},
                {-70, 10, -74});

    drawTetragonf({-70,-20,-73},
                {-14.5,-20, -73},
                {-14.5, 10, -73},
                {-70, 10, -73});

    drawTetragonf({-70,10,-73},
                {-14.5,10, -73},
                {-14.5, 10, -74},
                {-70, 10, -74});

    drawTetragonf({-14.5,-20,-73},
                {-14.5,-20, -74},
                {-14.5, 10, -74},
                {-14.5, 10, -73});

    //untuk etalase
    //glColor3f(1,0,1);
    glColor3ub(	248, 248, 255);
    drawTetragonf({-16,-20,-74},
                {-16,-20, -100},
                {-16, -5, -100},
                {-16, -5, -74});

    //bagian atas meja etalase
    drawTetragonf({-30, -5,-74},
                {-30, -5, -99},
                {-16, -5, -99},
                {-16, -5, -74});

    // bagian dalam
    drawTetragonf({-30,-20,-74},
                {-30,-20, -100},
                {-30, -5, -100},
                {-30, -5, -74});

    //glColor3f(0,1,0);
    glColor3ub(	248, 248, 255);
    // bagian depan
    drawTetragonf({-16,-20,-100},
                {-16,-20, -158},
                {-16, 10, -158},
                {-16, 10, -100});

    // bagian dalam
    drawTetragonf({-30,-20,-100},
                {-30,-20, -158},
                {-30, 10, -158},
                {-30, 10, -100});

    // bagian atas
    drawTetragonf({-30,10,-100},
                {-30, 10, -158},
                {-16, 10, -158},
                {-16, 10, -100});

    // dekorasi
    //bagian kiri (setelah etalase)
   // glColor3f(0.8,0.8,0.8);
   glColor3ub(222, 184, 135);
    drawTetragonf({-31,-5,-100},
                {-15,-5, -100},
                {-15, 11, -100},
                {-31, 11, -100});

    drawTetragonf({-31,-5,-99},
                {-15,-5, -99},
                {-15, 11, -99},
                {-31, 11, -99});

    drawTetragonf({-15,-5,-99},
                {-15,-5, -100},
                {-15, 11, -100},
                {-15, 11, -99});

    drawTetragonf({-31,-5,-99},
                {-31,-5, -100},
                {-31, 11, -100},
                {-31, 11, -99});

    drawTetragonf({-31, 11, -99},
                {-31, 11, -100},
                {-15, 11, -100},
                {-15, 11, -99});

    // bagian depan etalase (dekorasi)
    drawTetragonf({-15, -5,-74},
                {-15, -5, -99},
                {-15, -4, -99},
                {-15, -4, -74});

    // bagian belakang
    drawTetragonf({-31, -5,-74},
                {-31, -5, -99},
                {-31, -4, -99},
                {-31, -4, -74});

    //bagian atas untuk yang depan
    drawTetragonf({-16, -4,-74},
                {-16, -4, -99},
                {-15, -4, -99},
                {-15, -4, -74});

    //bagian atas untuk yang belakang
    drawTetragonf({-31, -4,-74},
                {-31, -4, -99},
                {-30, -4, -99},
                {-30, -4, -74});

    //bagian depan untuk belakang
    drawTetragonf({-16, -5,-74},
                {-16, -5, -99},
                {-16, -4, -99},
                {-16, -4, -74});

    //bagian belakang untuk belakangnya
    drawTetragonf({-30, -5,-74},
                {-30, -5, -99},
                {-30, -4, -99},
                {-30, -4, -74});

    //glColor3f(0.8,0.8,0.8);
    glColor3ub(222, 184, 135);
    //dekorasi keseluruhan meja

    //bagian bawah (semacam garis)

    //sisi depan
    drawTetragonf({-15, -15,-74},
                {-15, -15, -158},
                {-15, -13, -158},
                {-15, -13, -74});

    //sisi atas
    drawTetragonf({-15, -13,-74},
                {-15, -13, -158},
                {-16, -13, -158},
                {-16, -13, -74});

    //sisi bawah
    drawTetragonf({-15, -15,-74},
                {-15, -15, -158},
                {-16, -15, -158},
                {-16, -15, -74});


    // bagian atas sisi depan (berbeda dengan bagian etalase)

    // sisi depan
    drawTetragonf({-15, 10,-99},
                {-15, 10, -158},
                {-15, 11, -158},
                {-15, 11, -99});

    // sisi atas
    drawTetragonf({-16, 11,-99},
                {-16, 11, -158},
                {-15, 11, -158},
                {-15, 11, -99});

    // sisi bawah
    drawTetragonf({-16, 10,-99},
                {-16, 10, -158},
                {-15, 10, -158},
                {-15, 10, -99});

    // sisi belakang
    drawTetragonf({-16, 10,-99},
                {-16, 10, -158},
                {-16, 11, -158},
                {-16, 11, -99});

    glEnd();
}

void balok(double panjang, double lebar, double tinggi) {
    glScalef(panjang, tinggi, lebar);
    glutSolidCube(1);
}

void cermin(){
    glPushMatrix();
    glColor3ub(224, 255, 255);
     glTranslatef(-13,20,-239.5);
    balok(2,70,50);
    glPopMatrix();
}

void etalase2(){
     glPushMatrix();

    glRotatef(90,1.0,0.0,0.0);
     glColor4f(0.7, 0.7, 0.7, 0.5);
   // balok(30,45,25);
   balok(14,20,25);
    glPopMatrix();

}

void displayEtalase(){
    glPushMatrix();
    glTranslatef(-23,5,-86.5);
    etalase2();
    glPopMatrix();
}

void mesinBelakangKasir(){
    glPushMatrix();
    glColor3ub(255, 255, 150);
      glTranslatef(-62,-5,-87);
   balok(14,20,25);
    glPopMatrix();

     glPushMatrix();
    glColor3ub(255, 210, 167);
      glTranslatef(-62,-5,-108);
   balok(14,10,25);
    glPopMatrix();

     glPushMatrix();
    glColor3ub(255, 210, 167);
      glTranslatef(-62,-5,-119);
   balok(14,15,25);
    glPopMatrix();

    glPushMatrix();
    glColor3ub(255, 210, 167);
      glTranslatef(-62,-5,-147);
   balok(10,21,21);
    glPopMatrix();

}

void drawCuboid(const float (&dimensions)[3], const float (&center)[3]) {
    float width = dimensions[0];
    float height = dimensions[1];
    float depth = dimensions[2];

    // Menentukan setengah dari dimensi untuk pusat balok di (0,0,0)
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    // Menyimpan matriks saat ini dan menerapkan translasi
    glPushMatrix();
    glTranslatef(center[0], center[1], center[2]);

    glBegin(GL_QUADS);

    // Sisi depan
    drawTetragonf({-w, -h,  d}, { w, -h,  d}, { w,  h,  d}, {-w,  h,  d});

    // Sisi belakang
    drawTetragonf({-w, -h, -d}, {-w,  h, -d}, { w,  h, -d}, { w, -h, -d});

    // Sisi kiri
    drawTetragonf({-w, -h, -d}, {-w, -h,  d}, {-w,  h,  d}, {-w,  h, -d});

    // Sisi kanan
    drawTetragonf({ w, -h, -d}, { w,  h, -d}, { w,  h,  d}, { w, -h,  d});

    // Sisi atas
    drawTetragonf({-w,  h, -d}, {-w,  h,  d}, { w,  h,  d}, { w,  h, -d});

    // Sisi bawah
    drawTetragonf({-w, -h, -d}, { w, -h, -d}, { w, -h,  d}, {-w, -h,  d});

    glEnd();

    // Mengembalikan matriks ke keadaan semula
    glPopMatrix();
}

void bangkuBiru(){
    glColor3f(0.0,0.0,1.0);
    glPushMatrix();
    glTranslatef(20.0,55.0,-310.0);
    balok(10,1,7);
    glPopMatrix();

     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(17.0,45.0,-311.0);
    balok(1,1,20);
    glPopMatrix();

     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(23.0,45.0,-311.0);
    balok(1,1,20);
    glPopMatrix();

    glColor3ub(119, 69, 29);
    glPushMatrix();
    glTranslatef(20.0,43.0,-306.0);
    balok(12,12,2);
    glPopMatrix();

    glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(17.0,38.0,-300.0);
    balok(1,1,10);
    glPopMatrix();

    glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(23.0,38.0,-300.0);
    balok(1,1,10);
    glPopMatrix();
}

void tembokDeketTangga(){
    glColor3ub(211, 211, 211);
    glPushMatrix();
    glTranslatef(15.8,30.0,-305);
    balok(10,15,100);
    glPopMatrix();

    glColor3ub(211, 211, 211);
    glPushMatrix();
    glTranslatef(15.8,30.0,-245.0);
    balok(10,16,100);
    glPopMatrix();
}

void monitor(){
    //monitor
    glPushMatrix();
    glColor3ub(0,0,0);
    glTranslatef(-25,15,-130);
    balok(1,20,15);
    glPopMatrix();

    //penyangga
    glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(-24.5,6,-128.5);
    balok(1,1,15);
    glPopMatrix();
}

void gagangPintuStaff(){
//      {-14.5-19,-20, -159}
//      {-15.5,-20, -158-31}
    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(40,20,-392);
    balok(1,1,7);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(39,21,-390);
    balok(1,3.5,1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(12,20,-400);
    balok(1,1,7);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(13,21,-398);
    balok(3.5,1,1);
    glPopMatrix();

    //pintu karyawan meja kasir(dalam)
    glPushMatrix();
    glTranslatef(-51.5,20,-158);
    balok(1,1,7);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-50.5,21,-157);
    balok(3.5,1,1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15,20,-169.5);
    balok(1,1,7);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-14,21,-170.5);
    balok(1,3.5,1);
    glPopMatrix();
}

void tangga(){
    float jarakTanggaY=0;
    float jarakTanggaZ=0;
    float jarakHandlerY=0;
    float jarakHandlerZ=0;

//   handler horizontal
    glColor3ub(0,0,0);

    glPushMatrix();
    glTranslatef(39.8, 50.0, -315.0);
    //glRotatef(120,1.0,1.0,0.0);
    glRotatef(135,1.0,0.0,0.0);
    balok(2,2,150);
    glPopMatrix();

    //handler vertikal
    for(int i=0; i<5;i++){
        glPushMatrix();
        glColor3ub(0,0,0);
        glTranslatef(39.8, 5.0 + jarakHandlerY, -280.0 + jarakHandlerZ);
        balok(3,3,30);
        glPopMatrix();
        jarakHandlerY += 20;
        jarakHandlerZ += -20;
    }

    //anak tangga
    for(int i=0; i<9;i++){
        if(i == 8){
            glPushMatrix();
            glColor3f(0.5,0.5,0.5);

            glTranslatef(53.8, -10.0 + jarakTanggaY, -280.0 + jarakTanggaZ -15);
            balok(32,49,10);
            glPopMatrix();

            glBegin(GL_QUADS);
            // Tembok tangga

            // Sisi kanan
            drawTetragonf({39, -20, -280+jarakTanggaZ},
                            {39, -20, -280+jarakTanggaZ-15},
                            {39, jarakTanggaY-5,-280+jarakTanggaZ-15},
                            {39, jarakTanggaY-5, -280+jarakTanggaZ});

            // sisi kiri
            drawTetragonf({39, -20, -280+jarakTanggaZ-35},
                            {39, -20, -280+jarakTanggaZ-40},
                            {39, jarakTanggaY-5,-280+jarakTanggaZ-40},
                            {39, jarakTanggaY-5, -280+jarakTanggaZ-35});

            // sisi atas
            drawTetragonf({39, jarakTanggaY-5, -280+jarakTanggaZ-15},
                            {39, jarakTanggaY-5, -280+jarakTanggaZ-35},
                            {39, jarakTanggaY-30,-280+jarakTanggaZ-35},
                            {39, jarakTanggaY-30, -280+jarakTanggaZ-15});
            glEnd();

            glBegin(GL_QUADS);


            // Pintu di tangga
            // sisi depan (pintu)
            glColor3f(1,0.94,0.94);
            drawTetragonf({40, -20, -280+jarakTanggaZ-15},
                          {40, -20, -280+jarakTanggaZ-35},
                          {40, jarakTanggaY-30, -280+jarakTanggaZ-35},
                          {40, jarakTanggaY-30, -280+jarakTanggaZ-15});

            glColor3f(0.8,0.8,0.8);
            // Sisi kanan
            drawTetragonf({40, -20, -280+jarakTanggaZ-15},
                          {39, -20, -280+jarakTanggaZ-15},
                          {39, jarakTanggaY-30, -280+jarakTanggaZ-15},
                          {40, jarakTanggaY-30, -280+jarakTanggaZ-15});
            // Sisi kiri
            drawTetragonf({40, -20, -280+jarakTanggaZ-35},
                          {39, -20, -280+jarakTanggaZ-35},
                          {39, jarakTanggaY-30, -280+jarakTanggaZ-35},
                          {40, jarakTanggaY-30, -280+jarakTanggaZ-35});
            // sisi atas
            drawTetragonf({40, jarakTanggaY-30, -280+jarakTanggaZ-15},
                          {40, jarakTanggaY-30, -280+jarakTanggaZ-35},
                          {39, jarakTanggaY-30, -280+jarakTanggaZ-35},
                          {39, jarakTanggaY-30, -280+jarakTanggaZ-15});
            glEnd();
        }
        else{
            glPushMatrix();
            glColor3f(0.5,0.5,0.5);
            glTranslatef(53.8, -10.0 + jarakTanggaY, -280.0 + jarakTanggaZ);
            balok(32,20,10);
            glPopMatrix();

            glBegin(GL_QUADS);
            glColor3f(0.5,0.5,0.5);
            drawTetragonf({39, -20, -280+jarakTanggaZ},
                            {39, -20, -290+jarakTanggaZ},
                            {39, jarakTanggaY-5,-290+jarakTanggaZ},
                            {39, jarakTanggaY-5, -280+jarakTanggaZ});
            glEnd();
        }

        jarakTanggaY += 10;
        jarakTanggaZ += -10;
    }
}

void mejaBundarKecil() {
    GLUquadric* quad = gluNewQuadric();
    glColor3f(0.55f, 0.27f, 0.07f); // Warna coklat

    // Kaki meja
    glPushMatrix();
    glTranslatef(0.0, -10.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0); // Mengatur orientasi kaki meja
    gluCylinder(quad, 1.0, 1.0, 10.0, 32, 32);
    glPopMatrix();

    // Permukaan meja
    glPushMatrix();
//    glTranslatef(0.0, 10.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0); // Mengatur orientasi permukaan meja
    glColor3ub(221,218,221);
    gluCylinder(quad, 7.0, 7.0, 1.0, 32, 32);
    gluDisk(quad, 0.0, 7.0, 32, 1);
    glTranslatef(0.0, 0.0, 1.0);
    gluDisk(quad, 0.0, 7.0, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void tv(){
    //tv
     glPushMatrix();
    glColor3ub(47, 79, 79);
      glTranslatef(-68,35,-87);
   balok(2,30,20);
    glPopMatrix();

    //penyangga
     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(-69.5,30,-84);
    balok(1,1,25);
    glPopMatrix();

    //penyangga
     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(-69.5,30,-91);
    balok(1,1,25);
    glPopMatrix();

     //tv
     glPushMatrix();
    glColor3ub(47, 79, 79);
      glTranslatef(-68,35,-120);
   balok(2,30,20);
    glPopMatrix();

    //penyangga
     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(-69.5,30,-117);
    balok(1,1,25);
    glPopMatrix();

    //penyangga
     glColor3ub(139, 69, 19);
    glPushMatrix();
    glTranslatef(-69.5,30,-124);
    balok(1,1,25);
    glPopMatrix();
}

void rectangleChairWithEdgingLegs(){
    glColor3ub(102,70,48);
    drawCuboid({12, 1, 12}, {0, 9.5, 0});

    glBegin(GL_QUADS);
    //alas

    //left front edging legs
    drawTetragonf({-5,0, 5}, {-4, 0, 5}, {-2, 9, 3}, {-3, 9, 3});
    drawTetragonf({-4,0, 5}, {-4, 0, 4}, {-2, 9, 2}, {-2, 9, 3});
    drawTetragonf({-5,0, 4}, {-4, 0, 4}, {-2, 9, 2}, {-3, 9, 2});
    drawTetragonf({-5,0, 4}, {-5, 0, 5}, {-3, 9, 3}, {-3, 9, 2});
    //right front edging legs
    drawTetragonf({5,0, 5}, {4, 0, 5}, {2, 9, 3}, {3, 9, 3});
    drawTetragonf({4,0, 5}, {4, 0, 4}, {2, 9, 2}, {2, 9, 3});
    drawTetragonf({5,0, 4}, {4, 0, 4}, {2, 9, 2}, {3, 9, 2});
    drawTetragonf({5,0, 4}, {5, 0, 5}, {3, 9, 3}, {3, 9, 2});
    //right back edging legs
    drawTetragonf({5,0, -5}, {4, 0, -5}, {2, 9, -3}, {3, 9, -3});
    drawTetragonf({4,0, -5}, {4, 0, -4}, {2, 9, -2}, {2, 9, -3});
    drawTetragonf({5,0, -4}, {4, 0, -4}, {2, 9, -2}, {3, 9, -2});
    drawTetragonf({5,0, -4}, {5, 0, -5}, {3, 9, -3}, {3, 9, -2});
    //left back edging legs
    drawTetragonf({-5,0, -5}, {-4, 0, -5}, {-2, 9, -3}, {-3, 9, -3});
    drawTetragonf({-4,0, -5}, {-4, 0, -4}, {-2, 9, -2}, {-2, 9, -3});
    drawTetragonf({-5,0, -4}, {-4, 0, -4}, {-2, 9, -2}, {-3, 9, -2});
    drawTetragonf({-5,0, -4}, {-5, 0, -5}, {-3, 9, -3}, {-3, 9, -2});

    glEnd();
    drawCuboid({12.1, 16, 1}, {0, 17, -6});
    glColor3ub(214,204,192);
    drawCuboid({12, 2, 12}, {0, 11, 0});

}

void pintuKaryawanDalamMejaKasir(){
    glColor3f(1,0.94,0.94);
    glBegin(GL_QUADS);
    // Pintu (bagian utama)
    drawTetragonf({-70+15,-20,-159},
                {-14.5-19,-20, -159},
                {-14.5-19, height-20, -159},
                {-70+15, height-20, -159});

    // Bingkai pintu (sisi kiri)
    glColor3f(0.8,0.8,0.8);
    drawTetragonf({-70+15,-20,-159},
                {-70+15,-20, -158},
                {-70+15, height-20, -158},
                {-70+15, height-20, -159});

    // Bingkai pintu (sisi kanan)
    drawTetragonf({-14.5-19,-20,-159},
                {-14.5-19,-20, -158},
                {-14.5-19, height-20, -158},
                {-14.5-19, height-20, -159});

    // Bagian atas
    drawTetragonf({-70+15,height-20,-158},
                {-14.5-19,height-20, -158},
                {-14.5-19, height-20, -159},
                {-70+15, height-20, -159});
    glEnd();
}

void pintuKaryawanDiLuar(){
    glColor3f(1,0.94,0.94);
    glBegin(GL_QUADS);
    // Pintu (bagian utama)
    drawTetragonf({-15.5,-20,-158-10},
                {-15.5,-20, -158-31},
                {-15.5, height-20, -158-31},
                {-15.5, height-20, -158-10});

    glColor3f(0.8,0.8,0.8);
    // Bingkai pintu (sisi kiri)
    drawTetragonf({-15.5,-20,-158-10},
                {-14.5,-20, -158-10},
                {-14.5, height-20, -158-10},
                {-15.5, height-20, -158-10});

    // Bingkai pintu (sisi kanan)
    drawTetragonf({-15.5,-20,-158-31},
                {-14.5,-20, -158-31},
                {-14.5, height-20, -158-31},
                {-15.5, height-20, -158-31});

    // Bagian atas
    drawTetragonf({-15.5,height-20,-158-10},
                {-15.5,height-20, -158-31},
                {-14.5, height-20, -158-31},
                {-14.5, height-20, -158-10});
    glEnd();
}

void ruangKaryawan(){
   // glColor3f(0,1,1);
   glColor3ub(220, 220, 220);
    // sisi kiri (ada pintu ke ruang karyawan)
    // sisi kiri
    drawTetragonfWithTexture({-70,-20,-158},{-70+15,-20, -158},{-70+15, height, -158},{-70, height, -158}, _textureId[2], 1.2, 4.5);
//    drawTetragonf({-70,-20,-158},
//                {-70+15,-20, -158},
//                {-70+15, height, -158},
//                {-70, height, -158});
    // sisi kanan
    drawTetragonfWithTexture({-14.5-19, -20,-158},
                {-14.5, -20, -158},
                {-14.5, height, -158},
                {-14.5-19, height, -158}, _textureId[2], 1.2, 4.5);
//    drawTetragonf({-14.5-19, -20,-158},
//                {-14.5, -20, -158},
//                {-14.5, height, -158},
//                {-14.5-19, height, -158});
    // sisi atas
    drawTetragonfWithTexture({-70+15,height-20,-158},
                {-14.5-10,height-20, -158},
                {-14.5-10, height, -158},
                {-70+15, height, -158}, _textureId[2], 1.2, 1.2);
//    drawTetragonf({-70+15,height-20,-158},
//                {-14.5-10,height-20, -158},
//                {-14.5-10, height, -158},
//                {-70+15, height, -158});

//    glBegin(GL_QUADS);
    // sisi depan (ada pintu menuju ruang karyawan)
    // sisi kiri
    drawTetragonfWithTexture({-14.5,-20,-158},
                {-14.5,-20, -158-10},
                {-14.5, height, -158-10},
                {-14.5, height, -158}, _textureId[2], 1.0, 4.5);
//    drawTetragonf({-14.5,-20,-158},
//                {-14.5,-20, -158-10},
//                {-14.5, height, -158-10},
//                {-14.5, height, -158});
    // sisi kanan
    drawTetragonfWithTexture({-14.5,-20,-158-31},
                {-14.5,-20, -320},
                {-14.5, height, -320},
                {-14.5, height, -158-31}, _textureId[2], 5.0, 5.0);
//    drawTetragonf({-14.5,-20,-158-31},
//                {-14.5,-20, -320},
//                {-14.5, height, -320},
//                {-14.5, height, -158-31});
    // sisi atas
    drawTetragonfWithTexture({-14.51,height-20,-158},
                {-14.51,height-20, -320},
                {-14.51, height, -320},
                {-14.51, height, -158}, _textureId[2], 10.0, 1.0);
//    drawTetragonf({-14.5,height-20,-158},
//                {-14.5,height-20, -320},
//                {-14.5, height, -320},
//                {-14.5, height, -158});
    // sisi kanan
   // glColor3f(1,0,1);
   drawTetragonfWithTexture({-70,-20,-320},
                {-14.5,-20, -320},
                {-14.5, height, -320},
                {-70, height, -320}, _textureId[2], 5.0, 5.0);
//   glColor3ub(220, 220, 220);
//    drawTetragonf({-70,-20,-320},
//                {-14.5,-20, -320},
//                {-14.5, height, -320},
//                {-70, height, -320});
//    glEnd();
}

void pilarWastafel(){
      glColor3ub(211, 211, 211);
    glPushMatrix();
    glTranslatef(-5,25,-390.0);
    balok(15,15,90);
    glPopMatrix();
}

void gagangPintu(){
   //   gannga luar
     glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,14.0,-67.0);
    balok(0.5,0.5,7);
    glPopMatrix();

      glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,17.0,-68.0);
    balok(0.5,2,0.5);
    glPopMatrix();

      glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,11.0,-68.0);
    balok(0.5,2,0.5);
    glPopMatrix();

    //gangga dalam
     glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,14.0,-65.0-9);
    balok(0.5,0.5,7);
    glPopMatrix();

      glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,17.0,-66.0-7);
    balok(0.5,2,0.5);
    glPopMatrix();

      glColor3ub(0, 0, 0);
    glPushMatrix();
    glTranslatef(-11,11.0,-66.0-7);
    balok(0.5,2,0.5);
    glPopMatrix();
}

void displayKursiEdgyDalem(){
    glPushMatrix();
    //glTranslatef(20.0, -15.0, 80.0);
    glTranslatef(-48.8, -15.0, -335.0);
    for(int counter=0; counter<2; counter++){
        glRotatef(180, 0, 1, 0);
        rectangleChairWithEdgingLegs();
        glTranslatef(13, 0.0, 0.0);
        rectangleChairWithEdgingLegs();
        glTranslatef(1, 10, 18);
        mejaBundarKecil();
        glTranslatef(-1, -10, -18);

        if(counter%2==0){
            //glTranslatef(-5.0, 0.0, 35.0);
            glTranslatef(0.0, 0.0, 35.0);
        }

    }
    glPopMatrix();
}

void displayKursiEdgyDalem2(){
    glPushMatrix();
    glTranslatef(29, -15.0, -180.0);
    for(int counter=0; counter<2; counter++){
        glRotatef(180, 0, 1, 0);
        rectangleChairWithEdgingLegs();
        glTranslatef(13, 0.0, 0.0);
        rectangleChairWithEdgingLegs();
        glTranslatef(1, 10, 18);
        mejaBundarKecil();
        glTranslatef(-1, -10, -18);

        if(counter%2==0){
            //glTranslatef(-5.0, 0.0, 35.0);
            glTranslatef(0.0, 0.0, 35.0);
        }

    }
    glPopMatrix();
}

void PintuDindingPalingBelakang(){
    glBegin(GL_QUADS);
    glColor3f(1,0.94,0.94);
    // pintu utama
    drawTetragonf({10.0, -15.0, -400.0-1},
                  {26.0, -15.0, -400.0-1},
                  {26.0,height-20, -400.0-1},
                  {10.0, height-20, -400.0-1});

    // frame pintu
    glColor3f(0.8,0.8,0.8);
    // sisi kiri
    drawTetragonf({10.0, -15.0, -400.0},
                  {10.0, -15.0, -400.0-1},
                  {10.0, height-20, -400.0-1},
                  {10.0, height-20, -400.0});

    // sisi kanan
    drawTetragonf({26.0, -15.0, -400.0},
                  {26.0, -15.0, -400.0-1},
                  {26.0, height-20, -400.0-1},
                  {26.0, height-20, -400.0});
    // sisi atas
    drawTetragonf({10.0, height-20, -400.0},
                  {10.0, height-20, -400.0-1},
                  {26.0, height-20, -400.0-1},
                  {26.0, height-20, -400.0});
    // sisi bawah
    drawTetragonf({10.0, -15, -400.0},
                  {10.0, -15, -400.0-1},
                  {26.0, -15, -400.0-1},
                  {26.0, -15, -400.0});
    glEnd();
}

void lantai1BagianDepan(void) {
    // Dinding ruangan
    glBegin(GL_QUADS);

    //dinding kanan belakang
    glColor3f(0.5,0.5,0.5);
    drawTetragonf({68.5, -20.0, -70.0}, {68.5, -20.0, -60.0}, {68.5, height, -60.0}, {68.5, height, -70.0});
    drawTetragonf({68.5, -20.0, -60.0}, {70.0, -20.0, -60.0}, {70.0, height, -60.0}, {68.5, height, -60.0});

    //dinding kanan depan
    drawTetragonf({68.5, -20.0, 50.0}, {70.0, -20.0, 50.0}, {70.0, height, 50.0}, {68.5, height, 50.0});
    drawTetragonf({68.5, -20.0, 50.0}, {68.5, -20.0, 60.0}, {68.5, height, 60.0}, {68.5, height, 50.0});

    glColor3f(0.9,0.9, 0.9);
    //Pintu depan

    //bagian bawah
    drawTetragonf({-70.0, -20.0, 60.0}, {70.0, -20.0, 60.0}, {70.0, -15.0, 60.0}, {-70.0, -15.0, 60.0});

    //bagian tengah 1 (gerbang pojok kiri)
    drawTetragonf({-70.0, -20.0, 60.0}, {-50.0, -20.0, 60.0}, {-50.0, height, 60.0}, {-70.0, height, 60.0});

    //bagian tengah 2 (pilar tengah)
    drawTetragonf({-4.0, -20.0, 60.0}, {4.0, -20.0, 60.0}, {4.0, height, 60.0}, {-4.0, height, 60.0});

    //bagian tengah 3 (gerbang kanan)
    drawTetragonf({50.0, -20.0, 60.0}, {70.0, -20.0, 60.0}, {70.0, height, 60.0}, {50.0, height, 60.0});

    //bagian tengah 4 (Atasan)
    drawTetragonf({-70.0, height-4, 60.0},  {70.0, height-4, 60.0}, {70.0, height, 60.0}, {-70.0, height, 60.0});

    //hiasan pada dinding belakang paling kiri
    glColor3f(0.5, 0.5, 0.5);
    drawTetragonf({-70.0, -20.0, -65.0}, {-66.0, -20.0, -65.0}, {-66.0, height, -65.0}, {-70.0, height, -65.0});
    drawTetragonf({-66.0, -20.0, -70.0}, {-66.0, -20.0, -65.0}, {-66.0, height, -65.0}, {-66.0, height, -70.0});

    glEnd();

    // Dinding kiri
    drawTetragonfWithTexture({-70.0, -20.0, -70.0}, {-70.0, -20.0, 60.0}, {-70.0, height, 60.0}, {-70.0, height, -70.0}, _textureId[2], 8.0, 4.0);
//    drawTetragonf({-70.0, -20.0, -70.0}, {-70.0, -20.0, 60.0}, {-70.0, height, 60.0}, {-70.0, height, -70.0});

    // Dinding kanan
    drawTetragonfWithTexture({70.0, -20.0, -70.0}, {70.0, -20.0, 60.0}, {70.0, height, 60.0}, {70.0, height, -70.0}, _textureId[2], 8.0, 4.0);
    drawTetragonf({70.0, -20.0, -70.0}, {70.0, -20.0, 60.0}, {70.0, height, 60.0}, {70.0, height, -70.0});

        glColor3f(0.9,0.9, 0.9);
    glBegin(GL_QUADS);
    // Lantai
    glColor3f(0.5, 0.5, 0.5);
    drawTetragonf({-70.0, -20.0, 120.0}, {70.0, -20.0, 120.0}, {70.0, -20.0, -400.0}, {-70.0, -20.0, -400.0});

    //atap
    drawTetragonf({-70.0, height, 60}, {70.0, height, 60.0}, {70.0, height, -400.0}, {-70.0, height, -400.0});

    glEnd();

    // Lantai 2 (lantai setelah gerbang)
    drawTetragonfWithTexture({-70.0, -15.0, 60.0}, {70.0, -15.0, 60.0}, {70.0, -15.0, -70.0}, {-70.0, -15.0, -70.0}, _textureId[1], 10.0, 10.0);
//    glColor3f(0.9, 0.8, 0.8);
//    drawTetragonf({-70.0, -15.0, 60.0}, {70.0, -15.0, 60.0}, {70.0, -15.0, -70.0}, {-70.0, -15.0, -70.0});
//    glEnd();

    jarak = 28.0;
    for(int i = 0; i < 2; i++){
        // Bangku kayu 1
        glPushMatrix();
        glTranslatef(-40.0, -5.0, jarak);
        bangkuKayu();
        glPopMatrix();

        // Meja bundar
        glPushMatrix();
        glTranslatef(-50.0, -5.0, jarak);
        mejaBundar();
        glPopMatrix();

        // Bangku kayu 2
        glPushMatrix();
        glTranslatef(-60.0, -5.0, jarak);
        bangkuKayu();
        glPopMatrix();

        jarak -= 50;
    }

    jarak = 0;
    for(int i = 0; i < 3; i++){
        // Meja bundar kanan
        glPushMatrix();
        glTranslatef(50.0, -5.0, 32-jarak);
        mejaBundar();
        glPopMatrix();

        // Bangku kanan kayu
        glPushMatrix();
        glTranslatef(43.0, -5.0, 27-jarak);
        bangkuKayu();
        glPopMatrix();

        // Bangku kanan kayu
        glPushMatrix();
        glTranslatef(44.0, -5.0, 41-jarak);
        bangkuKayu();
        glPopMatrix();

        jarak += 40;
    }

    //Bagian Tengah
    //Meja bundar tengah 1
    glPushMatrix();
    glTranslatef(0.0, -5.0, -20);
    mejaBundar();
    glPopMatrix();

    // Bangku tengah kayu 1
    glPushMatrix();
    glTranslatef(-2.0, -5.0, -10);
    bangkuKayu();
    glPopMatrix();

    // Bangku tengah kayu 2
    glPushMatrix();
    glTranslatef(1.0, -5.0, -40);
    bangkuKayu();
    glPopMatrix();

    //Meja bundar tengah 2
    glPushMatrix();
    glTranslatef(0, -5.0, 20);
    mejaBundar();
    glPopMatrix();

    // Bangku tengah kayu 3
    glPushMatrix();
    glTranslatef(-2.0, -5.0, 10);
    bangkuKayu();
    glPopMatrix();

    // Bangku tengah kayu 4
    glPushMatrix();
    glTranslatef(2.0, -5.0, 30);
    bangkuKayu();
    glPopMatrix();

    //tempat duduk sisi kanan
//    tempatDudukBesar();

    glPushMatrix();
    glTranslatef(59.9, -15.0, -5.0);
    glRotatef(-90.0, 0.0, 1.0, 0.0);
    tempatDudukBesar(110.0) ;
    glPopMatrix();

}

void lantai1BagianDalam(){
    // Lantai
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, _textureId[1]);

//    glColor3f(0.5, 0, 0.5);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-70.0, -15.0, -70.0);
    glTexCoord2f(10.0, 0.0);glVertex3f(70.0, -15.0, -70.0);
    glTexCoord2f(10.0, 25.0);glVertex3f(70.0, -15.0, -400.0);
    glTexCoord2f(0.0, 25.0);glVertex3f(-70.0, -15.0, -400.0);
    glEnd();

//    drawTetragonf({-70.0, -15.0, -70.0}, {70.0, -15.0, -70.0}, {70.0, -15.0, -400.0}, {-70.0, -15.0, -400.0});
    glDisable(GL_TEXTURE_2D);


    // Dinding paling belakang

    // Sisi kiri
    drawTetragonfWithTexture({-70.0, -20.0, -400.0}, {10.0, -20.0, -400.0}, {10.0,height, -400.0}, {-70.0, height, -400.0}, _textureId[2], 4.0, 4.0);
//    drawTetragonf({-70.0, -20.0, -400.0}, {10.0, -20.0, -400.0}, {10.0,height, -400.0}, {-70.0, height, -400.0});
    // Sisi kanan

//    glColor3f(0.9,0.9, 0.9);
//    glBegin(GL_QUADS);
    drawTetragonfWithTexture({26.0, -20.0, -400.0}, {70.0, -20.0, -400.0}, {70.0,height, -400.0}, {26.0, height, -400.0}, _textureId[2], 4.5, 4.0);
//    drawTetragonf({26.0, -20.0, -400.0}, {70.0, -20.0, -400.0}, {70.0,height, -400.0}, {26.0, height, -400.0});
    // Sisi atas
    drawTetragonfWithTexture({-70.0, height-20, -400.01}, {70.0, height-20, -400.01}, {70.0,height, -400.01}, {-70.0, height, -400.01}, _textureId[2], 10.5, 1.5);
//    drawTetragonf({-70.0, height-20, -400.0}, {70.0, height-20, -400.0}, {70.0,height, -400.0}, {-70.0, height, -400.0});

    // Dinding kiri
    drawTetragonfWithTexture({-70.0, -20.0, -70.0}, {-70.0, -20.0, -400.0}, {-70.0, height, -400.0}, {-70.0, height, -70.0}, _textureId[2], 4.0, 1.0);
//    drawTetragonf({-70.0, -20.0, -70.0}, {-70.0, -20.0, -400.0}, {-70.0, height, -400.0}, {-70.0, height, -70.0});

    // Dinding kanan
    drawTetragonfWithTexture({70.0, -20.0, -70.0}, {70.0, -20.0, -400.0}, {70.0, height, -400.0}, {70.0, height, -70.0}, _textureId[2], 20.0, 4.0);
    drawTetragonf({70.0, -20.0, -70.0}, {70.0, -20.0, -400.0}, {70.0, height, -400.0}, {70.0, height, -70.0});
//    glEnd();

    PintuDindingPalingBelakang();
    glPushMatrix();
    glTranslatef(59.9, -15.0, -150.0);
    glRotatef(-90.0, 0.0, 1.0, 0.0);
    tempatDudukBesar(150.0) ;
    glPopMatrix();

	 //meja dan bangku pojok kanan
    //bangku kanan
        glPushMatrix();
        glTranslatef(48.8, -5.0, -205.0);
        bangkuKayu();
        glPopMatrix();
    //meja
        glPushMatrix();
        glTranslatef(49.8, -5.0, -215.0);
        mejaBundar();
        glPopMatrix();
        //bangku kiri
        glPushMatrix();
        glTranslatef(48.8, -5.0, -225.0);
        bangkuKayu();
        glPopMatrix();

        int jarakBkanan=0;
        int jarakMeja=0;
        int jarakBkiri=0;

        for(int i = 0; i < 4; i++){
             glPushMatrix();
        glTranslatef(48.8, -5.0, -205.0-jarakBkanan);
        bangkuKayu();
        glPopMatrix();
    //meja
        glPushMatrix();
        glTranslatef(49.8, -5.0, -215.0 - jarakMeja);
        mejaBundar();
        glPopMatrix();
        //bangku kiri
        glPushMatrix();
        glTranslatef(48.8, -5.0, -225.0 - jarakBkiri);
        bangkuKayu();
        glPopMatrix();

        jarakBkanan-=36;
        jarakMeja-=35;
        jarakBkiri-=36;
        }


    mesinBelakangKasir();
    tangga();
    tembokDeketTangga();
    tv();
    monitor();
    mejaKasir();
    ruangKaryawan();
    pintuKaryawanDalamMejaKasir();
    pintuKaryawanDiLuar();

    // Bangku kayu
    glPushMatrix();
    glTranslatef(24.0, -5.0, -90);
    bangkuKayu();
    glPopMatrix();

    // Meja bundar
    glPushMatrix();
    glTranslatef(20.0, -5.0, -100);
    mejaBundar();
    glPopMatrix();

    // Bangku kayu
    glPushMatrix();
    glTranslatef(20.0, -5.0, -109);
    bangkuKayu();
    glPopMatrix();

    displayKursiEdgyDalem2();
    pilarWastafel();
    displayKursiEdgyDalem();
    displayEtalase();
    gagangPintu();
    gagangPintuStaff();
    cermin();
    pintuMasukCafe();
}

void tampil(){
    updateCameraPosition();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();

    gluLookAt(cameraX, cameraY, cameraZ,  // Posisi kamera
              centerX, centerY, centerZ,    // Titik pusat kamera
              0.0f, 1.0f, 0.0f);       // Vektor up
    lantai1BagianDepan();
    lantai1BagianDalam();
    glDisable(GL_BLEND);
    glutSwapBuffers();
}

void ukuran(int lebar, int tinggi) {
    glMatrixMode(GL_PROJECTION);
    gluPerspective(50.0, 1, 5.0, 500.0);
    glTranslatef(0.0, -5.0, -150.0);//atur ukuran
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(250, 80);
    glutCreateWindow("TR_GRAFKOM_50/50 ENJOYER");
    init();

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutDisplayFunc(tampil);
    glutIdleFunc(tampil);
    glutReshapeFunc(ukuran);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp); // Menambahkan fungsi keyboard

    glutMainLoop();
    return 0;
}
