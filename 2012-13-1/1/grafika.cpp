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
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentitower_y, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glis_Hansel/Disable a kovetkezokre:
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

const int screenWidth = 600;	// alkalmazas ablak felbontasa
const int screenHeight = 600;
const float dt=100;
static float Rnd()
{
	// Random ertek a [-1,1] koordinatakon
     float lowest=0, highest=20;
     //plusz egy azert kell, mert igy [0,20] intervallumon fogja adni a szamokat, [0,19] helyett
     int range=(highest-lowest)+1;
	// a vegen az [0,20] intervallumbol [-1,1]et csinalunk
	//+1 a nullaval valo osztas elkerulese miatt van ott
     return (lowest+int(range*rand()/(RAND_MAX + 1.0))-10)/10;
}


//--------------------------------------------------------
// 2D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y;

   Vector() {
	x = y =  0;
   }
   Vector(float x0, float y0) {
	x = x0; y = y0;
   }
   Vector operator*(float a) {
	return Vector(x * a, y * a);
   }
   Vector operator/(float a) {
	return Vector(x / a, y / a);
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y);
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y);
   }
   Vector operator=(const Vector& v) {
 	return Vector(x = v.x, y = v.y);
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y );
   }
   float Length() { return sqrt(x * x + y * y ); }
   void Normalize() {float hossz=Length(); x/=hossz; y/=hossz;}
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) {
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
};

float Function(float X, float Y){
return 250.0+(sinf(X*Y/9.91/9.91*0.0001*cosf(X/99.5*0.015)*cosf(Y/99.5*0.015))+1.0)*382.0;
}
 //[0,10 000] -> mert ilyenkor szebb a hatterem, ezert itt ilyen intervallumban kell szamolni
 //[o,600]-> [0,10 000]
float ScreenToWorld(float x){
return x/600*10000;
}
//[-1,1]->[0,10 000]
float CoordsToWorld(float x){
return (x+1)*5000;
}

class Palya{
public:
    Color image[screenWidth*screenHeight];
    Palya(){
        for(int Y = 0; Y < screenHeight; Y++)
		for(int X = 0; X < screenWidth; X++)
			{
			   float background = Function(ScreenToWorld(X), ScreenToWorld(Y));
			   background/=1014;
               image[Y*screenWidth + X] = Color(background,background,background);
            }
    }
    void Draw(){
         glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    }
    float Get_Height(Vector v){
        return Function(CoordsToWorld(v.x),CoordsToWorld(v.y));
    }
};

Palya palya;

class Character{
   public:
     Vector position;
     Vector velocity;
     Vector direction;
     bool is_Hansel;

    Character(){
        is_Hansel=false;
        position.x=Rnd();
        position.y=Rnd();
        direction.x=Rnd();
        direction.y=Rnd();
        direction.Normalize();
     }

   float derivalt_x(float X, float Y){
    return Y*cosf(0.000150754*Y)*cosf(1.018245*pow(10,-6)*X*Y*cosf(0.000150754*X)*cosf(0.000150754*Y))*(0.00038897*cosf(0.000150754*X)
            -5.86387*pow(10,-8)*X*sinf(0.000150754*X));
    }
    float derivalt_y(float X, float Y){
    return X*cosf(0.000150754*X)*cosf(1.018245*pow(10,-6)*X*Y*cosf(0.000150754*X)*cosf(0.000150754*Y))*(0.00038897*cosf(0.000150754*Y)
            -5.86387*pow(10,-8)*Y*sinf(0.000150754*Y));
    }

    void Move(float eltelt_ido){
	//letrehozom a gradiens vektort, es a segitsegevel kiszamolom a meredekseget
        Vector grad(derivalt_x(CoordsToWorld(position.x),CoordsToWorld(position.y)),
                     derivalt_y(CoordsToWorld(position.x),CoordsToWorld(position.y)));
        float M=atanf(grad*direction)*180.0/3.14;

		//kiszamolom a terep menti sebesseget
        float speed_xyz=1000*(1-M/90);
        //kiszamolom a terep menti sebesseg felulnezeti komponenset (xyz->xy), cos nem adhat vissza negativat, mert [-90,90] az intervalluma
        float speed_xy=speed_xyz*cosf(M*3.14/180.0);
		//kiszamolom a megtett utat (felulnezetbol) s=v*t
		// 1000 millisecond = 1 second
		// [2012.09.28. 1:04:47] Zsolt Hornyak: msec => sec
		// [2012.09.28. 1:04:52] Zsolt Hornyak: 1000/1000 = 1
		float megtett_ut=speed_xy*eltelt_ido/1000;
		//megtett_ut hatarozza meg az iranyvektor hosszat, ezt meg atvaltom a vilagbol a sajat koordinatarendszerembe
		velocity=direction*megtett_ut;
		velocity.x=velocity.x/5000;
		velocity.y=velocity.y/5000;
		//mozgatas -> uj pozicio megadasa
        position=position+velocity;

		//megvizsgalom, hogy kiment-e a kepernyorol, ha igen, akkor visszadobom a kepernyo szelere, es az iranyvektor adott komponenset megforditom
        if(position.x>1.0){
            position.x=1.0;
            direction.x*=-1.0;
        }
        else if(position.x<-1.0){
            position.x=-1.0;
            direction.x*=-1.0;
        }

        if(position.y>1.0){
            position.y=1.0;
            direction.y*=-1.0;
        }
        else if(position.y<-1.0){
            position.y=-1.0;
            direction.y*=-1.0;
        }
    }
};

Character Tower;
Character Hansel;
Character Gretel;

bool triangle_is_enabled=false;
float triangle_life_time=0;


bool Check_Line(Character karakter){
     //karaktertol a Towerig mutato vektor es annak hossza
    Vector lower, dir = Tower.position-karakter.position;

    //eldontjuk, melyik az also, mert mindig, mindent az alsotol szamolunk.
    //also magassaga, es a ketto magassagkulonbsege -> Tower eseten +20, mert az 20 m magas
    float lower_z, height_difference;
    if(palya.Get_Height(karakter.position)<palya.Get_Height(Tower.position)+20){
        lower=karakter.position;
        lower_z=palya.Get_Height(lower);
        height_difference=palya.Get_Height(Tower.position)+20-lower_z;
    }
    else{
        lower=Tower.position;
        lower_z=palya.Get_Height(lower)+20;
        height_difference=palya.Get_Height(karakter.position)-lower_z;
    }

    //lecsokkentem az "lepkedos vektorom" hosszat 1-re
    Vector step=Tower.position-karakter.position;
    step.Normalize();

    //kiszamolom, hanyszor kell megvizsgalnom (100 m-enkent), hogy van-e koztuk valami -> also egesz resz kell, maradek <100 m nem izgat mar
    int hatar=(int)(dir.Length()/0.02);

    //5000 m*0,02=100m -> mindig i*100m-nyivel novelem az iranyvektor hosszat
    for(int i=1;i<hatar;i++){
		//iranyvektor meghosszabbitasa i-szeresere
		//ahol most vizsgalunk, ott a magassag		a haromszog aranyosan no -> fele akkora az iranyvektor - fele akkora lesz a z
        if(palya.Get_Height(lower+step*(i*0.02))>(lower_z+i*height_difference/hatar)) return false;
    }
	return true;
}


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ldirection. main() fv.)
void onInitialization( ) {
	//alap beallitas a karakterek megkulonboztetesere
	Hansel.is_Hansel=true;
}

///RAJZOLo FGV-EK
void Tower_New_Place(){
	//kerunk uj random helye a Towernak
	//ha eloszor fut le, akkor engedelyezzuk a tornyot-> igy kirajzolasnal megjelenik majd
    //beallitjuk a Tower uj koordinatait
    Tower.position.x=Rnd();
    Tower.position.y=Rnd();
}

void Draw_Tower(){
    float tower_x=Tower.position.x;
    float tower_y=Tower.position.y;
    glColor3f(1, 1, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(tower_x, tower_y+0.04f);
    glVertex2f(tower_x+0.04f, tower_y-0.04f);
    glVertex2f(tower_x-0.04f, tower_y-0.04f);
    glEnd();
}

void Draw_Triangle(){
	//Hansel es Gretel latjak- a tornyot
	//ha igen, akkor engedelyezem a haromszoget, ha nem akkor letiltom -> eletartam nem fog novekedni
	if(Check_Line(Hansel) && Check_Line(Gretel)){
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex2f(Tower.position.x, Tower.position.y);
		glVertex2f(Hansel.position.x, Hansel.position.y);
		glVertex2f(Gretel.position.x, Gretel.position.y);
		glEnd();
		triangle_is_enabled=true;
	}
    else triangle_is_enabled=false;
}

void Draw_Char(Character karakter){
    float pi=3.14;
	float r=0.035;

	//eldontom, hansel, vagy gretel kirajzolasat vegzem-e, hansel true, gretel false
	if(karakter.is_Hansel){
		//itt van hansel
		//kor, nyil hosszu resze, bal csik, also csik
		float Hansel1 = r*sin(45*3.14/180)+0.04;
		float Hansel2 = r*cos(45*3.14/180)+0.04;

		glColor3f(0, 0.74f, 1);
		glBegin(GL_LINE_STRIP);
        for(int i = 45; i<=405; i+=1.8){
            glVertex2f((karakter.position.x+r*sin(i*pi/180)), (karakter.position.y+r*cos(i*pi/180)));
        }
        glVertex2f((karakter.position.x+Hansel1), (karakter.position.y+Hansel2));
        glVertex2f((karakter.position.x+r*sin(45*pi/180)+0.015), (karakter.position.y+Hansel2));
        glVertex2f((karakter.position.x+Hansel1), (karakter.position.y+Hansel2));
        glVertex2f((karakter.position.x+Hansel1), (karakter.position.y+r*cos(45*pi/180)+0.010));
        glEnd();
	}
	else{
		//itt van gretel
		//kor, le a kereszt kozepeig, balra el, jobbra el, visza kereszt kozepeig, meg lejjebb menni.
		float Gretel1=r*cos(180*3.14/180)-0.05;
		float Gretel2=r*cos(180*3.14/180)-0.025;
		float Gretel3=r*sin(180*3.14/180);

		glColor3f(1, 0.07f, 0.57f);
		glBegin(GL_LINE_STRIP);
        for(int i = 180; i<=540; i+=1.8){
            glVertex2f((karakter.position.x+r*sin(i*pi/180)), (karakter.position.y+r*cos(i*pi/180)));
        }

        glVertex2f((karakter.position.x+Gretel3), (karakter.position.y+Gretel2));
        glVertex2f((karakter.position.x+Gretel3-0.0125), (karakter.position.y+Gretel2));
        glVertex2f((karakter.position.x+Gretel3+0.015), (karakter.position.y+Gretel2));
        glVertex2f((karakter.position.x+Gretel3), (karakter.position.y+Gretel2));
        glVertex2f((karakter.position.x+Gretel3), (karakter.position.y+Gretel1));
        glEnd();
        }
}

void Draw_Progress_bar(){
	//hatter
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(1.0, 0.95);
    glVertex2f(-1.0, 0.95);
    glEnd();

    //kiszamitjuk az ido hany szazalekaban volt lathato a piros haromszog
    //majd atalakitjuk azt a [-1,1] intervallumra a [0,1]rol

    float total_time=glutGet(GLUT_ELAPSED_TIME);
    float percent=triangle_life_time/total_time;
    percent=percent*2-1;

	//rajta levo resz
    glColor3f(0, 1, 0.49);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, 1.0);
    glVertex2f(percent, 1.0);
    glVertex2f(percent, 0.95);
    glVertex2f(-1.0, 0.95);
    glEnd();
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
	//kirajzolom a palyat, a harom karaktert es a haromszoget
	palya.Draw();
	Draw_Tower();
	Draw_Triangle();
	Draw_Char(Hansel);
	Draw_Char(Gretel);
	Draw_Progress_bar();

	// Buffercsere: rajzolas vege
    glutSwapBuffers();
}

// Billentower_yuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 't') {
	   Tower_New_Place();
    }
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
// A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
//atalakitom a kepernyo koordinata rendszeret ([0,599] intervallum) az altalam hasznalt koordinata rendszerbe
//karakterbol a kattintas iranyaba mutato vektor lesz az uj iranyvektor
//az uj iranyvektort normalizaljuk
//intervallum atvaltasa:
//        float window_x=x/299.5-1;
//		float window_y=-1*y/299.5+1;
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		Vector window(x/299.5-1,y*-1/299.5+1);
        Hansel.direction=window-Hansel.position;
        Hansel.direction.Normalize();
    }
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		Vector window(x/299.5-1,y*-1/299.5+1);
        Gretel.direction=window-Gretel.position;
        Gretel.direction.Normalize();
    }
}

float minimum(float a, float b){
    if(a<b) return a;
    return b;
}

float actual_time=0;
void SimulateWorld(float time_start, float time_end){
    for(float i=time_start; i<time_end; i+=dt){
		//kiszamolom mennyi idot kell animalnom-> ha a maradek ido <100 ms, azokat is animalni tudom igy
		// tegyuk fel, hogy 55 ms maradt hatra amit animalni kell
		// i+dt ugye mar nagyobb lenne, mint a time_end
		// akkor neked a time_end es az aktualis i kozotti reszt kell szimulalnod
		// ami time_end-i
		// ha meg i+dt-i, akkor a szokasos 100 ms-ot szimulalod le
		// i igazabol csak azt szamolja, hogy hanyszor 100-at kell leszimulalni
        float time_to_animate=minimum(time_end, i+dt)-i;
        //ha a program inditasa ota eltelt ido es a legutobbi frissites kulonbsege kisebb mint 100 ms, akkor varunk
        //majd mozgatjuk a ket karaktert, es frissitjuk a kepernyot
        //ha van haromszog, akkor is novelni kell az elettartamot, ha eppen nem mintavetelezek
        Hansel.Move(time_to_animate);
        Gretel.Move(time_to_animate);
        //ha a haromszog engedelyezve van, akkor noveljuk az elettartamat
        if(triangle_is_enabled){
            if(i+dt>=time_end) triangle_life_time+=(time_end-i);
            else triangle_life_time+=dt;
        }
    }
}


void onIdle( ) {
    float old_time=actual_time;
    actual_time=glutGet(GLUT_ELAPSED_TIME);
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
