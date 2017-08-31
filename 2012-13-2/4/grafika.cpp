//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fuggvenyt kiveve, a lefoglalt adat korrekt felszabaditasa nelkul
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : SZEPES NORA
// Neptun : N2WIN2
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

struct Vector {
   float x, y, z;

   Vector( ) {
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) {
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) {
	return Vector(x * a, y * a, z * a);
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z);
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z);
   }
   float operator*(const Vector& v) {
	return (x * v.x + y * v.y + z * v.z);
   }
   Vector operator%(const Vector& v) {
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
};

struct Color {
   float r, g, b;

   Color( ) {
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) {
	return Color(r * a, g * a, b * a);
   }
   Color operator*(const Color& c) {
	return Color(r * c.r, g * c.g, b * c.b);
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b);
   }
};

const int screenWidth = 600;
const int screenHeight = 600;
int camera_pos=34;
float MyLightPos[]={0,0,0,1};
float MyLightInt[]={1,1,1,0};

GLuint floor_texture;
GLuint ball_texture;
int texture_size = 256;

//legeneralom az oran tanult modszer szerint a texturakat
void generate_floor_texture(GLuint* id){
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);

    Color* mytexture=new Color[texture_size*texture_size];
    for(int x=0;x<texture_size;x++){
        for(int y=0;y<texture_size; y++){
            Color c(1, 0.87, 0.68);
            if((((y%128)>64)&&((x%128)<64))||(((x%128)>64)&&((y%128)<64))) c=Color(0.8,0.68,0.58);
            mytexture[x*texture_size+y]=c;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, mytexture);
    delete[] mytexture;
}

void generate_ball_texture(GLuint* id){
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);

    Color* mytexture=new Color[texture_size*texture_size];
    for(int x=0;x<texture_size;x++){
        for(int y=0;y<texture_size; y++){
            Color c(1, 0.84, 0);
            if((y%32)>16) c=Color(0.69,0.13,0.13);
            mytexture[x*texture_size+y]=c;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_FLOAT, mytexture);
    delete[] mytexture;
}

void draw_wall(float x=10, float y=10){

    float ambient[]={0.1,0.1,0.1,1.0};
    float specular[]={0.2,0.2,0.2,1.0};
    float diffuse[]={0.83,0.73,0.61,1.0};//beallitjuk a szint

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS,20);

    glNormal3f(0,0,1);
    glBegin(GL_QUADS);
        glVertex3f(-x,y,0);
        glVertex3f(-x,0,0);
        glVertex3f(0,0,0);
        glVertex3f(0,y,0);

        glVertex3f(0,y,0);
        glVertex3f(0,0,0);
        glVertex3f(x,0,0);
        glVertex3f(x,y,0);

        glVertex3f(0,0,0);
        glVertex3f(0,-y,0);
        glVertex3f(x,-y,0);
        glVertex3f(x,0,0);

        glVertex3f(-x,0,0);
        glVertex3f(-x,-y,0);
        glVertex3f(0,-y,0);
        glVertex3f(0,0,0);
    glEnd();
}

void do_walls(){
    glPushMatrix();
        glTranslatef(0,0,-10);
        draw_wall();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,10,0);
        glRotatef(90,1,0,0);
        draw_wall();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(10,0,0);
        glRotatef(270,0,1,0);
        draw_wall();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-10,0,0);
        glRotatef(90,0,1,0);
        draw_wall();
    glPopMatrix();
}

void do_floor(){
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, floor_texture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glNormal3f(0,1,0);
            glBegin (GL_QUADS);
                    glTexCoord2f (0.0, 0.0);
                glVertex3f (-10.0, -10.0, -10);
                    glTexCoord2f (3.0, 0.0);
                glVertex3f (-10.0, -10.0, 0);
                    glTexCoord2f (3.0, 3.0);
                glVertex3f (0, -10.0, 0);
                    glTexCoord2f (0.0, 3.0);
                glVertex3f (0, -10.0, -10);

                    glTexCoord2f (0.0, 0.0);
                glVertex3f (0, -10.0, -10);
                    glTexCoord2f (3.0, 0.0);
                glVertex3f (0, -10.0, 0);
                    glTexCoord2f (3.0, 3.0);
                glVertex3f (10.0, -10.0, 0);
                    glTexCoord2f (0.0, 3.0);
                glVertex3f (10.0, -10.0, -10);

                    glTexCoord2f (0.0, 0.0);
                glVertex3f (-10.0, -10.0, 0);
                    glTexCoord2f (3.0, 0.0);
                glVertex3f (-10.0, -10.0, 10);
                    glTexCoord2f (3.0, 3.0);
                glVertex3f (0, -10.0, 10);
                    glTexCoord2f (0.0, 3.0);
                glVertex3f (0, -10.0, 0);

                    glTexCoord2f (0.0, 0.0);
                glVertex3f (0, -10.0, 0);
                    glTexCoord2f (3.0, 0.0);
                glVertex3f (0, -10.0, 10);
                    glTexCoord2f (3.0, 3.0);
                glVertex3f (10.0, -10.0, 10);
                    glTexCoord2f (0.0, 3.0);
                glVertex3f (10.0, -10.0, 0);
            glEnd ();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void ball(){
    float R=2;
    int quality=30;
    float pi=3.14159265359;

    float ambient[]={1, 1, 1, 1.0};
    float specular[]={0.2,0.2,0.2,1.0};
    float diffuse[]={0.7, 0.7, 0.7,1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS,20);

    glPushMatrix();
        glTranslatef(-6,-2,-6);
        glRotatef(60,0.5,1,0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, ball_texture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBegin(GL_QUAD_STRIP);
                for(int i=0; i<quality; i++){
                    float line1=i*R*2/quality;
                    float line2=(i+1)*R*2/quality;

                    float r1=sqrt(R*R-(R-line1)*(R-line1));
                    float r2=sqrt(R*R-(R-line2)*(R-line2));

                    float normal1=line1/2/R;
                    float normal2=line2/2/R;

                for(int j =0; j<quality; j++){
                        float normal3=j/quality;

                        glTexCoord2f (normal1, normal3);
                        glNormal3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);
                        glVertex3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);

                        glTexCoord2f (normal2, normal3);
                        glNormal3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                        glVertex3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                    }
                }
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

float seal_function(float t){
    return 0.5/(1.5* sqrt(2*3.14))* (exp(-1*((t-3.5)*(t-3.5))/(2*1.5*1.5)));
}

Vector Get_Normal(Vector a, Vector b, Vector c){
    Vector normal, temp1, temp2;
    temp1=a-b;
    temp2=c-b;
    normal=temp1%temp2;
    return normal;
}

void seal_body(){
    float ambient[]={0.2, 0.2, 0.2, 1.0};
    float specular[]={0.8,0.8,0.8,1.0};
    float diffuse[]={0.3, 0.3, 0.3,1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS,70);

    int quality = 300;
    float pi=3.14159265359;

    glPushMatrix();
        glTranslatef(-9,-8,-8);
        glRotatef(90,0,1,0);
        glScalef(15,15,15);
            glBegin(GL_QUAD_STRIP);
                for(int i=0;i<=quality;i++){
                    float line1=i/(float)quality;
                    float line2=(i+1)/(float)quality;

                    float param1=i*pi/quality*2;
                    float param2=(i+1)*pi/quality*2;

                    float r1=seal_function(param1);
                    float r2=seal_function(param2);

                    for(int j=0; j<=quality; j++){
                        Vector v1(cosf(j*2.0/quality*pi)*r1, sinf(j*2.0/quality*pi)*r1, line1);
                        Vector v2(cosf(j*2.0/quality*pi)*r2, sinf(j*2.0/quality*pi)*r2, line2);
                        Vector v3(cosf((j+1)*2.0/quality*pi)*r1, sinf((j+1)*2.0/quality*pi)*r1, line1);

                        v3=Get_Normal(v1,v2,v3);
                        glNormal3f(v3.x, v3.y, v3.z);
                        glVertex3f(v1.x,v1.y,v1.z);
                        glVertex3f(v2.x,v2.y,v2.z);
                    }
                }
            glEnd();
    glPopMatrix();
}

void seal_eye(){
    float R=0.3;
    int quality=30;
    float pi=3.14159265359;

    float ambient[]={0.1,0.1,0.1, 1.0};
    float specular[]={0.2,0.2,0.2,1.0};
    float diffuse[]={0.7,0.7,0.7,1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS,20);

    glPushMatrix();
        glDisable(GL_LIGHTING);
        glBegin(GL_QUAD_STRIP);
            for(int i=0; i<quality; i++){
                float line1=i*R*2/quality;
                float line2=(i+1)*R*2/quality;

                float r1=sqrt(R*R-(R-line1)*(R-line1));
                float r2=sqrt(R*R-(R-line2)*(R-line2));

                float normal1=line1/2/R;
                float normal2=line2/2/R;

            for(int j =0; j<quality; j++){
                    float normal3=j/quality;

                    glNormal3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);
                    glVertex3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);

                    glNormal3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                    glVertex3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                }
            }
        glEnd();
        glEnable(GL_LIGHTING);
    glPopMatrix();
}

void seal_head(){
    float R=1;
    int quality=30;
    float pi=3.14159265359;

    float ambient[]={0.2, 0.2, 0.2, 1.0};
    float specular[]={0.8,0.8,0.8,1.0};
    float diffuse[]={0.3, 0.3, 0.3,1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS,70);

    glPushMatrix();
        glTranslatef(6,-8,-9);
        glScalef(1.5,0.9,0.9);
            glBegin(GL_QUAD_STRIP);
                for(int i=0; i<quality; i++){
                    float line1=i*R*2/quality;
                    float line2=(i+1)*R*2/quality;

                    float r1=sqrt(R*R-(R-line1)*(R-line1));
                    float r2=sqrt(R*R-(R-line2)*(R-line2));

                for(int j =0; j<quality; j++){
                        glNormal3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);
                        glVertex3f(sinf(j*360.0/quality*pi/180)*r1, cosf(j*360.0/quality*pi/180)*r1, line1);

                        glNormal3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                        glVertex3f(sinf(j*360.0/quality*pi/180)*r2, cos(j*360.0/quality*pi/180)*r2, line2);
                    }
                }
            glEnd();
    glPopMatrix();
}

void pufoka(){
    glPushMatrix();
        glTranslatef(6,-7.7,-7.6);
        seal_eye();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(6,-7.5,-9);
        seal_eye();
    glPopMatrix();

    seal_head();
    seal_body();
}



void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    generate_floor_texture(&floor_texture);
    generate_ball_texture(&ball_texture);
}

void onDisplay() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0, 1.0, 0.1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,camera_pos,0,0,camera_pos-1,0,1,0);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, MyLightInt);
    glLightfv(GL_LIGHT0,GL_POSITION, MyLightPos);

    do_walls();
    do_floor();
    ball();

    glPushMatrix();
        glTranslatef(-9,2,0);
        glRotatef(240,0,1,0.2);
        pufoka();
    glPopMatrix();

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'f'){
        camera_pos-=1;
        glutPostRedisplay( );
    }
    else if (key == 'b'){
        camera_pos+=1;
        glutPostRedisplay( );
    }

}

void onMouse(int button, int state, int x, int y) {
}

void onIdle( ) {
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();					// Esemenykezelo hurok

    return 0;
}
