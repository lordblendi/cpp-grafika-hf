//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

// 2D Vektor
struct Vector {
   float x, y;

   Vector() {
	x = y =  0;
   }
   Vector(float x0, float y0) {
	x = x0; y = y0;
   }
};

// Spektrum illetve szin
struct Color {
   float r, g, b;

   Color( ) {
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
};



//also es felso feher fal
//benne van a 40x40-es szoba teruleteben
class sideWall{
    private:
        Vector coord;
        float width, height;

    public:
        sideWall(){
            coord.x=6;
            coord.y=45;
            width=39;
            height=1;
        }
        //alapbol felso fal, ezzel alsora tudom varazsolni
        void changeToBottom(){
            coord.y=6;
        }

        void drawSideWalls(){
             glColor3f(1, 1, 1);
            glBegin(GL_QUADS);
                glVertex2f(coord.x, coord.y);
                glVertex2f(coord.x+width, coord.y);
                glVertex2f(coord.x+width, coord.y-height);
                glVertex2f(coord.x, coord.y-height);

            glEnd();
        }
};

//piros teglak a bal oldalt
class Brick{
    public:
        Vector coord;
        float speed, weight, height, width;
        bool touched;

        Brick(){
            setBrickCoords(45);
            height=8;
            width=1;
            weight=height*width;
            speed=0; //alapbol nincs sebesseg, mert nem mozdul a tegla
            touched=false;
        }
        //kulonbozo magassagokat ezzel allitom be -> tombot fogok alkalmazni
        void setBrickCoords(float y=45){
            coord.x=5;
            coord.y=y;
        }

        void drawBrick(){
            glColor3f(1, 0, 0);
            glBegin(GL_QUADS);
                glVertex2f(coord.x, coord.y);
                glVertex2f(coord.x+width, coord.y);
                glVertex2f(coord.x+width, coord.y-height);
                glVertex2f(coord.x, coord.y-height);
            glEnd();
        }

        void setSpeed(float newspeed){
            speed=newspeed;
        }

        void moveBrick(float dt=1){
            coord.x-=dt*speed;
        }
};

//uto
class Racket{
public:
    float width, height;
    Vector coord;

    Racket(){
        width=1;
        height=4;
        coord.x=35;
        coord.y=30;
    }

    void drawRacket(){
        glColor3f(0, 1, 0);
		glBegin(GL_QUADS);
            glVertex2f(coord.x, coord.y);
            glVertex2f(coord.x+width, coord.y);
            glVertex2f(coord.x+width, coord.y-height);
            glVertex2f(coord.x, coord.y-height);
		glEnd();
    }
    //az uto nem hagyhatja el a 40x40-es szobat
    void moveRacket(float x, float y){
        if((coord.x+x<45)&&(coord.x+x>5))coord.x+=x;
        if((coord.y+y<45)&&(coord.y-height+y>5))coord.y+=y;
    }
};


const int numberOfBricks = 5;   //teglak szama
sideWall top, bottom;
Brick bricks[numberOfBricks];
Racket racket;

class Ball{
public:
    Vector center, velocity;
    float radius, weight;

    Ball(){
        radius=1;
        weight=radius*radius*3.14;
        center.x=7.0f;
        center.y=23.0f;
        velocity.x=10.0f;
        velocity.y=2.0f;
    }

    void collisionWhiteWall(float dt=1){
        if((center.x>45)||(center.x<6)){
            return;
        }
        if(((center.y+dt*velocity.y+radius)>44)||((center.y+dt*velocity.y-radius)<6)){
              velocity.y*=-1;
        }
    }

    void collisionRacket(float dt=1){
        //with the Racket - left side - same height
        //megnezem egy magassagban vannak-e
        //ha igen, akkor beleer-e a labda
        //elorol es hatulrol is vizsgalom
        if((center.y<racket.coord.y)&&(center.y>(racket.coord.y-racket.height))){
            if(((center.x+dt*velocity.x+radius)>racket.coord.x)&&((center.x+dt*velocity.x+radius)<(racket.coord.x+racket.width))){
                velocity.x*=-1;
                speedUp();
            }
            else if(((center.x+dt*velocity.x-radius)<(racket.coord.x+racket.width))&&((center.x+dt*velocity.x-radius)>(racket.coord.x))){
                velocity.x*=-1;
                speedUp();
            }
        }//with the Racket - top and bottom - x intervallumat vizsgalom, majd y-et felulrol es alulrol is
        else if((center.x>racket.coord.x)&&(center.x<(racket.coord.x+racket.width))){
            if(((center.y+dt*velocity.y-radius)<racket.coord.y)&&((center.y+dt*velocity.y-radius)>(racket.coord.y-racket.height))){
                velocity.y*=-1;
                speedUp();
            }
            else if(((center.y+dt*velocity.y+radius)>(racket.coord.y-racket.height))&&((center.y+dt*velocity.y+radius)<racket.coord.y)){
                velocity.y*=-1;
                speedUp();
            }
        }//with the Racket - left upper corner - megvizsgalom, hogya labda alja a bal felso sarok alatt van, a jobb oldala pedig tole jobbra, majd megnezem, hogy a tavolsagok a sugar+lambdanal kisebb.e
        //ha igen akkor attol fuggoen, hogy melyik iranybol jon a labda, a sebesseg ket koordinatajat megcserelem, es az egyiket -al veszem.
        else if(((center.y+dt*velocity.y-radius)<racket.coord.y)&&(center.y>racket.coord.y)&&(center.x<racket.coord.x)&&((center.x+dt*velocity.x+radius)>racket.coord.x)){
            if(((center.y-racket.coord.y)<(radius+0.001))&&((center.x-racket.coord.x)<(radius+0.001))){
                float temp=velocity.x;
                if(velocity.y>0){
                    velocity.x=(-1)*velocity.y;
                }
                else{
                    velocity.x=velocity.y;
                    temp*=-1;
                }
                velocity.y=temp;
                speedUp();
            }
        }//left lower corner
        else if(((center.y+dt*velocity.y+radius)>(racket.coord.y-racket.height))&&(center.y<(racket.coord.y-racket.height))&&(center.x<racket.coord.x)&&((center.x+dt*velocity.x+radius)>racket.coord.x)){
            if(((racket.coord.y-racket.height-center.y)<(radius+0.001))&&((center.x-racket.coord.x)<(radius+0.001))){
                 float temp=velocity.x;
                if(velocity.y>0){
                    velocity.x=(-1)*velocity.y;
                }
                else{
                    velocity.x=velocity.y;
                    temp*=-1;
                }
                velocity.y=temp;
                speedUp();
            }
        }
    }

    void collisionBrick(float dt=1){
        float brickRightX=bricks[0].coord.x+bricks[0].width;
        float brickLeftX=bricks[0].coord.x;
        float i;
        int j;
        //mvˇx=MVx+mv'ˇx
        //1/2*m*(vˇx+vˇy)^2 = 1/2*M*Vx^2 + 1/2*m*(v'ˇx+vˇy)^2
        //impulzus es engergiamegmaradassal kiszamolom, hogy mi lesz a tegla es a labda sebessege utkozes utan. Vx-et kifejtem, beirom az alsoba, kijon egy masodfoku egynelet, es azzal kijon a labda utkozes utani sebessege (v'x) es a teglae (Vx).
        //nincs surlodas ezert nem foglalkozunk az y iranyu sebesseggel, ez ki is esik.
        //ha mar utkozott egyszer a teglaval, akkor nem utkozhetnek ujra
        //ha utkozik egy teglaval, akkor kilep a ciklusbol, masikkal nem utkozhet
        //sarokutkozest nem kezelem le
        if(((center.x+dt*velocity.x-radius)<brickRightX)&&((center.x+dt*velocity.x-radius)>brickLeftX)){
            for(i=37, j=0; j<numberOfBricks; i-=8, j++){
                if((bricks[j].touched==false)&&((center.y+dt*velocity.y)>(i-0.001))&&((center.y+dt*velocity.y)<(i+8+0.001))){
                    bricks[j].touched=true;
                    float a=1+bricks[j].weight/weight;
                    float b=2*velocity.x;
                    float c=(1-bricks[j].weight/weight)*velocity.x*velocity.x;
                    float x1 = (-b + sqrt(b * b - 4 * a * c)) / (2 *a);
                    bricks[j].setSpeed(weight/bricks[j].weight*(x1-velocity.x));
                    velocity.x=x1;
                    return;
                }
            }
        }
    }

    void moveBall(float dt=1){
        collisionWhiteWall(dt);
        collisionRacket(dt);
        collisionBrick(dt);
        center.x+=dt*velocity.x;
        center.y+=dt*velocity.y;
    }
    // 1,2*1/2mv^2=1/2mv'^2
    //1.2v^2=v'^2
    //sqrt(1.2)v=v'
	//a mozgasi energia no 20 %-al, es nem a sebesseg, ezert kiszamolom, hogy a sebesseg az mennyivel is valtozik
    void speedUp(){
           velocity.x*=sqrt(1.2);
           velocity.y*=sqrt(1.2);
    }

    void drawBall(){
        float pi=3.14;
        glColor3f(1, 1, 0);
		glBegin(GL_TRIANGLE_FAN);
            for(int i = 0; i<=360; i+=1.8){
                glVertex2f((center.x+radius*sin(i*pi/180)), (center.y+radius*cos(i*pi/180)));
            }
        glEnd();
    }
};



Ball ball;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glViewport(0, 0, 600, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,50,0,50);  //koordinata rendszer ballitasa [0,50] intervallumba mindket tengelyen

    bottom.changeToBottom();
    float i;
    int j;
    for(i=45, j=0; j<numberOfBricks; i-=8, j++) bricks[j].setBrickCoords(i);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
	ball.drawBall();
	racket.drawRacket();
	top.drawSideWalls();
	bottom.drawSideWalls();
	for(int i=0;i<numberOfBricks;i++){
        bricks[i].drawBrick();
	}
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'k') {
        racket.moveRacket(1,0);
    }
     else if (key == 'j') {
        racket.moveRacket(-1,0);
    }
     else if (key == 'i') {
        racket.moveRacket(0,1);
    }
     else if (key == 'm') {
        racket.moveRacket(0,-1);
    }
     glutPostRedisplay();
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
}

float minimum(float a, float b){
    if(a<b) return a;
    return b;
}

float actual_time=0, dt=0.01;
void SimulateWorld(float time_start, float time_end){
    for(float i=time_start; i<time_end; i+=dt){
        float time_to_animate=minimum(time_end, i+dt)-i;
        ball.moveBall(time_to_animate);
        for(int j=0; j<numberOfBricks;j++){
            bricks[j].moveBrick(time_to_animate);
        }
    }
}
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    float old_time=actual_time;
    actual_time=glutGet(GLUT_ELAPSED_TIME)/1000.0;
    SimulateWorld(old_time, actual_time);
    glutPostRedisplay();
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
