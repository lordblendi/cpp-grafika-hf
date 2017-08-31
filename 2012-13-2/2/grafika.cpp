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
//vilag alapkoordinatai
double world_left=-100.0, world_right=100.0, world_top=100.0, world_bottom=-100.0;

class Zoom{
    public:
    double camera_left, camera_right, camera_top, camera_bottom;

        Zoom(){//kamera alapertekei a vilag alapertekei
            camera_left=world_left;
            camera_right=world_right;
            camera_top=world_top;
            camera_bottom=world_bottom;
        }
		//atvaltok, hogy a kattintas koordinatai modell koordinatak legyenek
        float screenToModellX(int x){
            return camera_left+(float)x*(camera_right-camera_left)/600.0;
        }
        float screenToModellY(int y){
            return camera_top-(float)y*(camera_top-camera_bottom)/600.0;
        }
		//zoom eseten a kameraban latott kep feleakkora lesz, vagyis a szelesseg es a magassag felezodik
        void zoom_It(){
            double width=(camera_right-camera_left)/2;
            double height=(camera_top-camera_bottom)/2;
            camera_left+=width/2;
            camera_right-=width/2;
            camera_top-=height/2;
            camera_bottom+=height/2;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(camera_left, camera_right, camera_bottom, camera_top);
            glutPostRedisplay( );
        }

		//kiszamolom a szelesseg es a magassag aranyait, aztan annyival eltolom a kepet balra es felfele.
        void pan_It(){
            double x_rate=(camera_right-camera_left)*0.1;
            double y_rate=(camera_top-camera_bottom)*0.2;
            camera_left-=x_rate;
            camera_right-=x_rate;
            camera_top+=y_rate;
            camera_bottom+=y_rate;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(camera_left, camera_right, camera_bottom, camera_top);
            glutPostRedisplay( );
        }
};

Zoom zoom;

class Splines{
    public:
        int number_of_coords;
        Vector coords[100], arrow_CR[2], arrow_CRI[2];
        double parameter[100], arrow_current_parameter_CR, arrow_current_parameter_CRI;
        float arrow_speed;

        Splines(){
            number_of_coords=0;
            for(int i=0;i<100;i++){
                coords[i]=Vector(0,0);
                parameter[i]=i+sin(i/5.0)*0.4;//ezt a parametersorozatot allitjuk be
            }//a nyilak alaphossza 0
            arrow_CR[0]=Vector(0,0);
            arrow_CR[1]=Vector(0,0);
            arrow_CRI[0]=Vector(0,0);
            arrow_CRI[1]=Vector(0,0);
            reset_Arrows();
        }

        void reset_Arrows(){//a nyilak sebessege attol fugg, hogy hany koordinata van, es hogy mekkora az elso es az utolso parameter kozott a tavolsag
            arrow_speed=(float)(number_of_coords/(parameter[number_of_coords-1]-parameter[0]));
            arrow_current_parameter_CR=parameter[0];
			//mivel a CRI-n visszafele megy a nyil, ezert csak akkor hozzuk letre, ha legalabb 1 pontunk van, igy vedekezve a tombbol valo kicimezes ellen
            if(number_of_coords>0)arrow_current_parameter_CRI=parameter[number_of_coords-1];
        }

        void add_Point(float x, float y){//max 100 koordinatat engedunk meg, es a nyilakat ujrainditjuk
            if(number_of_coords<100){
                coords[number_of_coords++]=Vector(x,y);
                reset_Arrows();
            }
        }

        Vector Get_CR_Vi(int i){
            if(i==0 || i == (number_of_coords-1)) return Vector(1,1);
             return (((coords[i+1]-coords[i])/(parameter[i+1]-parameter[i])+(coords[i]-coords[i-1])/(parameter[i]-parameter[i-1]))*0.5);
        }

        Vector Get_CRI_Vi(int i){//specifikacio alapjan
            if(i==0 || i == (number_of_coords-1)) return Vector(1,1);
             return ((coords[i+1]-coords[i-1])/(parameter[i+1]-parameter[i-1]));
        }

        void draw_Splines(bool is_cr=true){
            if(number_of_coords>2){
                glBegin(GL_LINE_STRIP);
                if(is_cr)glColor3f(1, 0, 0);
                else glColor3f(0, 1, 0);

                //Az algoritmus megertese es implementalasa a
                //Szirmay-Kalos Laszlo, Antal Gyorgy es Csonka Ferenc: Haromdimenzios grafika, animacio es jatekfejlesztes c. konyv segitsegevel tortent
                for(int i=0;i<(number_of_coords-1);i++){
                    Vector ai, bi, ci, di;
                    if(is_cr){
                        ai=(Get_CR_Vi(i+1)+Get_CR_Vi(i))/(pow(parameter[i+1]-parameter[i],2))-(coords[i+1]-coords[i])*2/(pow(parameter[i+1]-parameter[i],3));
                        bi=(coords[i+1]-coords[i])*3/(pow(parameter[i+1]-parameter[i],2))-(Get_CR_Vi(i+1)+Get_CR_Vi(i)*2)/(parameter[i+1]-parameter[i]);
                        ci=Get_CR_Vi(i);
                    }
                    else{
                        ai=(Get_CRI_Vi(i+1)+Get_CRI_Vi(i))/(pow(parameter[i+1]-parameter[i],2))-(coords[i+1]-coords[i])*2/(pow(parameter[i+1]-parameter[i],3));
                        bi=(coords[i+1]-coords[i])*3/(pow(parameter[i+1]-parameter[i],2))-(Get_CRI_Vi(i+1)+Get_CRI_Vi(i)*2)/(parameter[i+1]-parameter[i]);
                        ci=Get_CRI_Vi(i);
                    }
                    di=coords[i];
					//a splinet ezer reszre osztva abrazolom
                    double between_coords=1000/number_of_coords;
                    double delta_parameter;
                    if(i==0) delta_parameter = parameter[i+1]/(3*between_coords);
                    else if (i==(number_of_coords-1)) delta_parameter = parameter[i-1]/(3*between_coords);
                    else delta_parameter = (parameter[i+1]-parameter[i-1])/(3*between_coords);

                    for(double j=parameter[i];j<parameter[i+1];j+=delta_parameter){
                        Vector r=ai*pow(j-parameter[i],3)+bi*pow(j-parameter[i],2)+ci*(j-parameter[i])+di;
                        glVertex2f(r.x, r.y);
                    }
                }
                glEnd();
            }
        }

        void draw_Points(){
            glColor3f(0.93f, 0.91f, 0.66f);
            float size_of_points=1.0f;
            for(int i=0;i<number_of_coords;i++){
                glBegin(GL_QUADS);
                    glVertex2f(coords[i].x-size_of_points,coords[i].y+size_of_points);
                    glVertex2f(coords[i].x+size_of_points,coords[i].y+size_of_points);
                    glVertex2f(coords[i].x+size_of_points,coords[i].y-size_of_points);
                    glVertex2f(coords[i].x-size_of_points,coords[i].y-size_of_points);
                glEnd();
            }
        }

        void draw_Arrows(){
            Vector w;
            w.x=arrow_CR[1].y;
            w.y=arrow_CR[1].x*(-1);
            glColor3f(1,1,1);
            glBegin(GL_LINE_STRIP);
                glVertex2f(arrow_CR[0].x, arrow_CR[0].y);
                glVertex2f(arrow_CR[0].x+arrow_CR[1].x, arrow_CR[0].y+arrow_CR[1].y);
                glVertex2f(arrow_CR[0].x+arrow_CR[1].x*0.9 + w.x*0.1, arrow_CR[0].y+arrow_CR[1].y*0.9 + w.y*0.1);
                glVertex2f(arrow_CR[0].x+arrow_CR[1].x, arrow_CR[0].y+arrow_CR[1].y);
                glVertex2f(arrow_CR[0].x+arrow_CR[1].x*0.9 - w.x*0.1, arrow_CR[0].y+arrow_CR[1].y*0.9 - w.y*0.1);
            glEnd();

            w.x=arrow_CRI[1].y;
            w.y=arrow_CRI[1].x*(-1);
            glColor3f(0,1,1);
            glBegin(GL_LINE_STRIP);
                glVertex2f(arrow_CRI[0].x, arrow_CRI[0].y);
                glVertex2f(arrow_CRI[0].x-arrow_CRI[1].x, arrow_CRI[0].y-arrow_CRI[1].y);
                glVertex2f(arrow_CRI[0].x-arrow_CRI[1].x*0.9 + w.x*0.1, arrow_CRI[0].y-arrow_CRI[1].y*0.9 + w.y*0.1);
                glVertex2f(arrow_CRI[0].x-arrow_CRI[1].x, arrow_CRI[0].y-arrow_CRI[1].y);
                glVertex2f(arrow_CRI[0].x-arrow_CRI[1].x*0.9 - w.x*0.1, arrow_CRI[0].y-arrow_CRI[1].y*0.9 - w.y*0.1);
            glEnd();
        }

        void move_arrow_CR(float time_to_animate=1.0f){
		//csak akkor abrazoljuk, ha legalabb ket pont van
		if(number_of_coords>2){
                for(int i=0; i<(number_of_coords-1);i++){
					//megkeressuk, hogy hol van a nyil most
                    if(arrow_current_parameter_CR<parameter[i+1]){

                        arrow_current_parameter_CR+=arrow_speed*time_to_animate;
                        if(arrow_current_parameter_CR>=parameter[number_of_coords-1]){
                            arrow_current_parameter_CR=parameter[0];
                            i=0;
                        }
                        else if(arrow_current_parameter_CR>=parameter[i+1]) i++;
						//kiszamolom, hogy annak a spline darabnak mi a koordinataja
                        Vector ai, bi, ci, di;
                        ai=(Get_CR_Vi(i+1)+Get_CR_Vi(i))/(pow(parameter[i+1]-parameter[i],2))-(coords[i+1]-coords[i])*2/(pow(parameter[i+1]-parameter[i],3));
                        bi=(coords[i+1]-coords[i])*3/(pow(parameter[i+1]-parameter[i],2))-(Get_CR_Vi(i+1)+Get_CR_Vi(i)*2)/(parameter[i+1]-parameter[i]);
                        ci=Get_CR_Vi(i);
                        di=coords[i];
                        arrow_CR[0]=ai*pow(arrow_current_parameter_CR-parameter[i],3)+bi*pow(arrow_current_parameter_CR-parameter[i],2)+ci*(arrow_current_parameter_CR-parameter[i])+di;
						//a hosszt az adott pont sebessegvektora adja meg, ami a pont koordinatajat megado polinom derivaltja, es ezt skalazom 0.2-vel
                        arrow_CR[1]=(ai*(pow(arrow_current_parameter_CR-parameter[i],2)*3)+bi*(arrow_current_parameter_CR-parameter[i])*2+ci)*0.2;
                        return;
                    }
                }
            }
        }

        void move_arrow_CRI(float time_to_animate=1.0f){
            if(number_of_coords>2){
                for(int i=number_of_coords-1; i>0;i--){
                    if(arrow_current_parameter_CRI>parameter[i-1]){

                        arrow_current_parameter_CRI-=arrow_speed*time_to_animate;
                        if(arrow_current_parameter_CRI<=parameter[0]){
                            arrow_current_parameter_CRI=parameter[number_of_coords-1];
                            i=number_of_coords-1;
                        }
                        else if(arrow_current_parameter_CRI<=parameter[i-1]) i--;
                        Vector ai, bi, ci, di;
                        ai=(Get_CRI_Vi(i)+Get_CRI_Vi(i-1))/(pow(parameter[i]-parameter[i-1],2))-(coords[i]-coords[i-1])*2/(pow(parameter[i]-parameter[i-1],3));
                        bi=(coords[i]-coords[i-1])*3/(pow(parameter[i]-parameter[i-1],2))-(Get_CRI_Vi(i)+Get_CRI_Vi(i-1)*2)/(parameter[i]-parameter[i-1]);
                        ci=Get_CRI_Vi(i-1);
                        di=coords[i-1];
                        arrow_CRI[0]=ai*pow(arrow_current_parameter_CRI-parameter[i-1],3)+bi*pow(arrow_current_parameter_CRI-parameter[i-1],2)+ci*(arrow_current_parameter_CRI-parameter[i-1])+di;
                        arrow_CRI[1]=(ai*(pow(arrow_current_parameter_CRI-parameter[i-1],2)*3)+bi*(arrow_current_parameter_CRI-parameter[i-1])*2+ci)*0.2;

                        return;
                    }
                }
            }
        }
};

Splines splines;


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glViewport(0, 0, 600, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(world_left, world_right, world_bottom, world_top);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    splines.draw_Points();
    splines.draw_Splines(true);
    splines.draw_Splines(false);
    splines.draw_Arrows();
    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'Z'){
        zoom.zoom_It();
    }
    else if (key == 'P'){
        zoom.pan_It();
    }
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT && state == GLUT_DOWN){
        splines.add_Point(zoom.screenToModellX(x), zoom.screenToModellY(y));
		glutPostRedisplay( );
    }
}

float minimum(float a, float b){
    if(a<b) return a;
    return b;
}
float actual_time=0.0, dt=0.1;
void SimulateWorld(float time_start, float time_end){
    for(float i=time_start; i<time_end; i+=dt){
        float time_to_animate=minimum(time_end, i+dt)-i;
        splines.move_arrow_CRI(time_to_animate);
        splines.move_arrow_CR(time_to_animate);
        glutPostRedisplay();
    }
}
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
