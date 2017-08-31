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
float previous_x, previous_y;
int chosen;

struct Color {
   float r, g, b;

   Color(float r0, float g0, float b0) {
	r = r0; g = g0; b = b0;
   }
};

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
};

class Zoom{
    public:
    float balx, jobbx, lenty, fenty;

    Zoom(){
        balx=100.0;
        lenty=100.0;
        jobbx=500.0;
        fenty=500.0;
    }

    float ScreenToModellX(int x){
        return balx+(float)x*(jobbx-balx)/600.0;
    }
    float ScreenToModellY(int y){
        return fenty-(float)y*(fenty-lenty)/600.0;
    }

    void ZoomIt(float egerx, float egery){
/*

Van az aktualis nezet, ez a vilagbol kivag egy negyzetet,
es ezen belul valahol van az eger.

Nagyitas utan a vilagban ugyanott van az eger,
de a vilagbol kisebb negyzet vagodik ki a viewportba.

Tehat a
(vilag.eger.x-vilag.viewport.balx)/(vilag.viewport.jobbx-vilag.viewport.balx)
ugyanaz kell legyen a ket nagyitasban.*/
        float szelesseg=jobbx-balx;
        float magassag=fenty-lenty;
        float x_arany=(egerx-balx)/szelesseg;
        float y_arany=(egery-lenty)/magassag;

        balx=egerx-x_arany*szelesseg/10.0;
        jobbx=egerx+(1-x_arany)*szelesseg/10.0;
        lenty=egery-y_arany*magassag/10.0;
        fenty=egery+(1-y_arany)*magassag/10.0;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(balx, jobbx, lenty, fenty);
    }
};

Zoom zoomin;

class Points{
    public:
        int pontok_szama;
        //x, y koordinatak es a parameterezes
        Vector coords[100];
        double parameter[100];
     Points(){
        pontok_szama=0;
        for(int i=0;i<101;i++){
            coords[i].x=0.0;
            coords[i].y=0.0;
        }
        Parameterezes();
    }

    void Add_Point(float x, float y){
        if(pontok_szama<100){
            pontok_szama++;
            coords[pontok_szama].x=x;
            coords[pontok_szama].y=y;
        }
    }

    void Draw_Points(){
       //glBegin(GL_POINTS);
       // glBegin(GL_LINE_STRIP);
       glBegin(GL_QUADS);
        glColor3f(1, 1, 0);
        for(int i=1;i<=pontok_szama;i++){
            glVertex2f(coords[i].x-5, coords[i].y+5);
            glVertex2f(coords[i].x+5, coords[i].y+5);
            glVertex2f(coords[i].x+5, coords[i].y-5);
            glVertex2f(coords[i].x-5, coords[i].y-5);
        }
         glEnd();
    }

    Vector Get_Vi(int i){
        Vector ret;
        if(i==1 || i == pontok_szama) return ret;
         ret=((coords[i+1]-coords[i])/(parameter[i+1]-parameter[i])+(coords[i]-coords[i-1])/(parameter[i]-parameter[i-1]))*0.5;
         return ret;
    }
    void Draw_CatMull_Rom(){
        if(pontok_szama>2){
            glBegin(GL_LINE_STRIP);
            glColor3f(0, 1, 0);

            //Az algoritmus megertese es implementalasa a
            //Szirmay-Kalos Laszlo, Antal Gyorgy es Csonka Ferenc: Haromdimenzios grafika, animacio es jatekfejlesztes c. konyv segitsegevel tortent
            for(int i=1;i<pontok_szama;i++){
                Vector ai, bi, ci, di;
                ai=(Get_Vi(i+1)+Get_Vi(i))/(pow(parameter[i+1]-parameter[i],2))-(coords[i+1]-coords[i])*2/(pow(parameter[i+1]-parameter[i],3));
                bi=(coords[i+1]-coords[i])*3/(pow(parameter[i+1]-parameter[i],2))-(Get_Vi(i+1)+Get_Vi(i)*2)/(parameter[i+1]-parameter[i]);
                ci=Get_Vi(i);
                di=coords[i];

                double pontok_kozott=1000/pontok_szama;
                double novekmeny = (parameter[i+1]-parameter[i-1])/(3*pontok_kozott);

                for(double j=parameter[i];j<parameter[i+1];j+=novekmeny){
                    Vector r=ai*pow(j-parameter[i],3)+bi*pow(j-parameter[i],2)+ci*(j-parameter[i])+di;
                    glVertex2f(r.x, r.y);
                }
            }
            glEnd();
        }
    }
    bool CR_Vagas(int x, int y){
        if(pontok_szama>2){
            Vector elozo=coords[1];
            //Az algoritmus megertese es implementalasa a
            //Szirmay-Kalos Laszlo, Antal Gyorgy es Csonka Ferenc: Haromdimenzios grafika, animacio es jatekfejlesztes c. konyv segitsegevel tortent
             for(int i=1;i<pontok_szama;i++){
                Vector ai, bi, ci, di;
                ai=(Get_Vi(i+1)+Get_Vi(i))/(pow(parameter[i+1]-parameter[i],2))-(coords[i+1]-coords[i])*2/(pow(parameter[i+1]-parameter[i],3));
                bi=(coords[i+1]-coords[i])*3/(pow(parameter[i+1]-parameter[i],2))-(Get_Vi(i+1)+Get_Vi(i)*2)/(parameter[i+1]-parameter[i]);
                ci=Get_Vi(i);
                di=coords[i];

                double pontok_kozott=1000/pontok_szama;
                double novekmeny = (parameter[i+1]-parameter[i-1])/(3*pontok_kozott);

                for(double j=parameter[i];j<parameter[i+1];j+=novekmeny){
                    Vector r=ai*pow(j-parameter[i],3)+bi*pow(j-parameter[i],2)+ci*(j-parameter[i])+di;
                   if(Cohen_Sutherland(x,y,elozo, r)) return true;
                   elozo=r;
                }
            }
        }
        return false;
    }

    bool Cohen_Sutherland(int x, int y, Vector elozo, Vector kovetkezo ){
        //-2 -1 x +1 +2
        //Az algoritmus megertese es implementalasa a Szirmay-Kalos Laszlo : Szamitogepes Grafika konyv alapjan keszult
        float xmin=zoomin.ScreenToModellX((float)x-2.0);
        float xmax=zoomin.ScreenToModellX((float)x+2.0);
        float ymin=zoomin.ScreenToModellY((float)y+2.0);
        float ymax=zoomin.ScreenToModellY((float)y-2.0);

        int C1=0;
        int C2=0;

        if(elozo.x < xmin)
            C1=C1|(1<<1);
        else if(elozo.x > xmax)
            C1=C1|(1<<3);

        if(elozo.y < ymin)
            C1=C1|(1<<0);
        else if(elozo.y > ymax)
            C1=C1|(1<<2);

        if(kovetkezo.x < xmin)
            C2=C2|(1<<1);
        else if(kovetkezo.x > xmax)
            C2=C2|(1<<3);

         if(kovetkezo.y < ymin)
            C2=C2|(1<<0);
        else if(kovetkezo.y > ymax)
            C2=C2|(1<<2);


        if(C1==0 || C2==0) return true;


        for (int i = 0; i < 4; i++)
            if ((C1 & (1<<i)) && (C2 & (1<<i))) return false;


        switch(C1)
        {
            case 1:
                    elozo.y=ymin;
                    break;
            case 2:
                    elozo.x=xmin;
                    break;
            case 4:
                    elozo.y=ymax;
                    break;
            case 8:
                    elozo.x=xmax;
                    break;
            case 3:
                    elozo.y=ymin;
                    elozo.x=xmin;
                    break;
            case 6:
                    elozo.y=ymax;
                    elozo.x=xmin;
                    break;
            case 9:
                    elozo.y=ymin;
                    elozo.x=xmax;
                    break;
            case 12:
                    elozo.y=ymax;
                    elozo.x=xmax;
                    break;
        }

        switch(C2)
        {
            case 1:
                    kovetkezo.y=ymin;
                    break;
            case 2:
                    kovetkezo.x=xmin;
                    break;
            case 4:
                    kovetkezo.y=ymax;
                    break;
            case 8:
                    kovetkezo.x=xmax;
                    break;
            case 3:
                    kovetkezo.y=ymin;
                    kovetkezo.x=xmin;
                    break;
            case 6:
                    kovetkezo.y=ymax;
                    kovetkezo.x=xmin;
                    break;
            case 9:
                    kovetkezo.y=ymin;
                    kovetkezo.x=xmax;
                    break;
            case 12:
                    kovetkezo.y=ymax;
                    kovetkezo.x=xmax;
                    break;
        }

        return Cohen_Sutherland(x, y, elozo, kovetkezo );
    }

    //A Lagrange polinom es a KK-spline megertese es implementalasa a
    //Fletcher Dunn, Ian Parberry: 3D Math primer for graphics and game developement c. konyv segitsegevel tortent.
    double Lagrange_polinom(double t, int aktualis_koordinata, int melyik_pont){
        double ret=1.0;
        for(int j=aktualis_koordinata; j<=(aktualis_koordinata+2);j++){
            if(melyik_pont!=j) ret*=(t-parameter[j])/(parameter[melyik_pont]-parameter[j]);
        }
       return ret;
    }

    void Draw_KK(){
        if(pontok_szama>2){
            glBegin(GL_LINE_STRIP);
            glColor3f(1, 0, 0);
                for(int i=1;(i+1)<=(pontok_szama);i++){
                    double pontok_kozott=1000/pontok_szama;
                    double novekmeny;
                    if(i==1) novekmeny = (parameter[i+2]-parameter[i])/(3*pontok_kozott);
                    else if (i==(pontok_szama-1)) novekmeny = (parameter[i+1]-parameter[i-1])/(3*pontok_kozott);
                    else novekmeny = (parameter[i+2]-parameter[i-1])/(4*pontok_kozott);

                    for(double t=parameter[i];t<=parameter[i+1];t+=novekmeny)
                    {
                        double arany=(t-parameter[i])/(parameter[i+1]-parameter[i]);
                        Vector r11,r12,r21,r22;
                        if(i==1)
                        {
                            for(int j=i;j<=i+2;j++){
                                r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                                if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                                else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                            }
                            glVertex2f(r11.x, r11.y);
                            glVertex2f(r12.x, r12.y);
                        }
                        else if(i==(pontok_szama-1)){
                            for(int j=i-1;j<=i+1;j++){
                                r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                                if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                                else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                            }
                            glVertex2f(r11.x, r11.y);
                            glVertex2f(r12.x, r12.y);
                        }
                        else{
                            for(int j=i;j<=i+2;j++){
                                r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                                if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                                else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                            }

                            for(int j=i-1;j<=i+1;j++){
                                r21=r21+coords[j]*Lagrange_polinom(t,i-1,j);
                                if((t+novekmeny)>parameter[i+1]) r22=r22+coords[j]*Lagrange_polinom(parameter[i+1],i-1,j);
                                else  r22=r22+coords[j]*Lagrange_polinom(t+novekmeny,i-1,j);
                            }

                            if(arany<0)arany=0.0;

                            Vector r=r11*arany+r21*(1-arany);
                            glVertex2f(r.x, r.y);
                            r=r12*arany+r22*(1-arany);
                            glVertex2f(r.x, r.y);
                        }
                    }
                }
            glEnd();
        }
    }

    bool KK_Vagas(int x, int y){
        if(pontok_szama>2){
            for(int i=1;(i+1)<=(pontok_szama);i++){
                double pontok_kozott=1000/pontok_szama;
                double novekmeny;
				if(i==1) novekmeny = (parameter[i+2]-parameter[i])/(3*pontok_kozott);
				else if (i==(pontok_szama-1)) novekmeny = (parameter[i+1]-parameter[i-1])/(3*pontok_kozott);
				else novekmeny = (parameter[i+2]-parameter[i-1])/(4*pontok_kozott);
                for(double t=parameter[i];t<=parameter[i+1];t+=novekmeny)
                {
                    double arany=(t-parameter[i])/(parameter[i+1]-parameter[i]);
                    Vector r11,r12,r21,r22;
                    if(i==1)
                    {
                        for(int j=i;j<=i+2;j++){
                            r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                            if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                            else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                        }
                        if(Cohen_Sutherland(x,y,r11, r12)) return true;
                    }
                    else if(i==(pontok_szama-1)){
                        for(int j=i-1;j<=i+1;j++){
                            r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                            if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                            else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                        }
                        if(Cohen_Sutherland(x,y,r11, r12)) return true;
                    }
                    else{
                        for(int j=i;j<=i+2;j++){
                            r11=r11+coords[j]*Lagrange_polinom(t,i,j);
                            if((t+novekmeny)>parameter[i+1]) r12=r12+coords[j]*Lagrange_polinom(parameter[i+1],i,j);
                            else  r12=r12+coords[j]*Lagrange_polinom(t+novekmeny,i,j);
                        }

                        for(int j=i-1;j<=i+1;j++){
                            r21=r21+coords[j]*Lagrange_polinom(t,i-1,j);
                            if((t+novekmeny)>parameter[i+1]) r22=r22+coords[j]*Lagrange_polinom(parameter[i+1],i-1,j);
                            else  r22=r22+coords[j]*Lagrange_polinom(t+novekmeny,i-1,j);
                        }

                        if(arany<0)arany=0.0;
                        Vector r1=r11*arany+r21*(1-arany);
                        Vector r2=r12*arany+r22*(1-arany);
                        if(Cohen_Sutherland(x,y,r1, r2)) return true;
                    }
                }
            }
            return false;
        }
    }

    void Parameterezes(){
        double j=1.0;
        for(int i=0;i<100;i++,j+=3*i){
            parameter[i+1]=j;
        }
    }

    void Move_The_Spline(int x, int y){
        for(int i=1;i<=pontok_szama;i++){
            coords[i].x=coords[i].x+x;
            coords[i].y=coords[i].y+y;
        }
    }

};
Points CR;
Points KK;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glViewport(0, 0, 600, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(100,500,100,500);
    previous_x=0.0;
    previous_y=0.0;
    chosen=0;
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CR.Draw_Points();
    KK.Draw_Points();
    CR.Draw_CatMull_Rom();
    KK.Draw_KK();
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'z'){
        zoomin.ZoomIt(zoomin.ScreenToModellX(x),zoomin.ScreenToModellY(y));
        glutPostRedisplay();	// d beture rajzold ujra a kepet
    }
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) { // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        CR.Add_Point(zoomin.ScreenToModellX(x),zoomin.ScreenToModellY(y));
        KK.Add_Point(zoomin.ScreenToModellX(x),zoomin.ScreenToModellY(y));
        glutPostRedisplay();
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        if(chosen==0){
            if(KK.KK_Vagas(x,y)){
                chosen=1;
                previous_x=zoomin.ScreenToModellX(x);
                previous_y=zoomin.ScreenToModellY(y);
            }
            else if(CR.CR_Vagas(x,y)){
                chosen=2;
                previous_x=zoomin.ScreenToModellX(x);
                previous_y=zoomin.ScreenToModellY(y);
            }
        }
        else if(chosen==1){
            chosen=0;
            KK.Move_The_Spline(zoomin.ScreenToModellX(x)-previous_x, zoomin.ScreenToModellY(y)-previous_y);
        }
        else if(chosen==2){
            chosen=0;
            CR.Move_The_Spline(zoomin.ScreenToModellX(x)-previous_x, zoomin.ScreenToModellY(y)-previous_y);
        }
        glutPostRedisplay();
    }

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
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
