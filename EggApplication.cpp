/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacją osi 
//  układu współrzednych

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>

# define M_PI           3.14159265358979323846
typedef float point3[3];

static GLfloat viewer[] = { 0.0, 0.0, 10.0 };

static GLfloat pix2angleX;     // przelicznik pikseli na stopnie
static GLfloat pix2angleY;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
                               // 0 - nie naciśnięto żadnego klawisza
                               // 1 - naciśnięty zostać lewy klawisz

static GLint rotationStatus = 0; //typ obrotu
                                //0 - obrót obiektu
                                //1 - obrót kamery

//zmienna pomocna w niwelowaniu zjawiska gimball lock
static int up = 1;

// kąty obrotu kamery
static float cameraTheta;
static float cameraPhi;

// odległość kamery od początku układu współrzędnych
static int R = 4;

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą
static int y_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_y = 0;        // różnica pomiędzy pozycją bieżącą
                                      // i poprzednią kursora myszy


/*************************************************************************************/

//struktura punktu wraz z kolorem
struct Point {
    float x;
    float y;
    float z;

    float R;
    float G;
    float B;

    float NVectorX;
    float NVectorY;
    float NVectorZ;
};

//dzielna podziału boku kwadratu jednostkowego
static int layers = 4;
static int N = layers * 2 - 2;

//tablica przechowująca punkty modelu jajka
Point** EggPoints;
//zmienna odpowiedzialna za aktualny model
int model = 1;  // 1- punkty, 2- siatka, 3 - wypełnione trójkąty, 4 - teapot

static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu

float step = 1.0 / N;


void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora
        y_pos_old = y;
        // jako pozycji poprzedniej
        status = 1;          // wcięnięty został lewy klawisz myszy
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        y_pos_old = y;
        status = 2;
    }
    else
        status = 0;          // nie został wcięnięty żaden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{

    delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy

    x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie

    delta_y = y - y_pos_old;     // obliczenie różnicy położenia kursora myszy

    y_pos_old = y;            // podstawienie bieżącego położenia jako poprzednie

    glutPostRedisplay();     // przerysowanie obrazu sceny
}


void Axes(void)
{

    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // początek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // początek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  początek i koniec obrazu osi y

    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x

    glVertex3fv(x_min);
    glVertex3fv(x_max);

    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);

    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);

    glEnd();
}

/*************************************************************************************/

void calculateAngles() {
    cameraTheta += delta_x;
    cameraPhi += delta_y;

    if (cameraTheta < 0.0) {
        cameraTheta = 360.0;
    }
    if (cameraTheta > 360) {
        cameraTheta = 0;
    }

    if (cameraPhi < 0.0) {
        cameraPhi = 360.0;
    }
    if (cameraPhi > 360) {
        cameraPhi = 0;
    }
}


float generateX(float u, float v) {
    float vPi = M_PI * v;
    return ((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(vPi));
}

float generateY(float u, float v) {
    return (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
}

float generateZ(float u, float v) {
    float vPi = M_PI * v;
    return ((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(vPi));
}

float generateXu(float u, float v) {
    return ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v));
}

float generateYu(float u, float v) {
    return 650 * pow(u, 3) - 960 * pow(u, 2) * 320 * u;
}

float generateZu(float u, float v) {
    return ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v));
}

float generateXv(float u, float v) {
    return M_PI * (90 * pow(u, 5) - 255 * pow(u, 4) + 270 * pow(u, 3) - 100 * pow(u, 2) + 45 * u) * sin(M_PI * v);
}

float generateYv(float u, float v) {
    return 0;
}

float generateZv(float u, float v) {
    return M_PI * (90 * pow(u, 5) - 255 * pow(u, 4) + 270 * pow(u, 3) - 100 * pow(u, 2) + 45 * u) * sin(M_PI * v);
}

void GenerateEggPoints() {


    EggPoints = new Point * [N];
    for (int i = 0; i < N; i++)
        EggPoints[i] = new Point[N];



    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EggPoints[i][j].x = generateX(i * step, j * step);
            EggPoints[i][j].y = generateY(i * step, j * step);
            EggPoints[i][j].z = generateZ(i * step, j * step);

            EggPoints[i][j].R = ((double)rand() / (RAND_MAX));
            EggPoints[i][j].G = ((double)rand() / (RAND_MAX));
            EggPoints[i][j].B = ((double)rand() / (RAND_MAX));

            GLfloat Xu, Xv, Yu, Yv, Zu, Zv;
            Xu = generateXu(i * step, j * step);
            Xv = generateXu(i * step, j * step);
            Yu = generateYu(i * step, j * step);
            Yv = generateXu(i * step, j * step);
            Zu = generateZu(i * step, j * step);
            Zv = generateXu(i * step, j * step);

            float vectorX, vectorY, vectorZ;
            vectorX = Yu * Zv - Zu * Yu;
            vectorY = Zu * Xv - Xu * Zv;
            vectorZ = Xu * Yv - Yu * Xv;

            float vectorLen = sqrt(pow(vectorX, 2) + pow(vectorY, 2) + pow(vectorZ, 2));
            if (vectorLen == 0) {
                vectorLen = 1;
            }
            EggPoints[i][j].NVectorX = vectorX / vectorLen;
            EggPoints[i][j].NVectorY = vectorY / vectorLen;
            EggPoints[i][j].NVectorZ = vectorZ / vectorLen;
        }
    }
}




void spinEgg()
{

    if (status != 1 && status != 2) {
        theta[0] += 0.05;
        if (theta[0] > 360.0) theta[0] = 0.0;

        theta[1] += 0.05;
        if (theta[1] > 360.0) theta[1] = 0.0;

        theta[2] += 0.05;
        if (theta[2] > 360.0) theta[2] = 0.0;



        glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
    }
}

void Egg() {
    switch (model)
    {
    case 1:
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                glBegin(GL_POINTS);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                glEnd();
            }
        }
        break;
    case 2:

        glBegin(GL_LINES);
        for (int i = 0; i < N; i++) {
            int solver = N - i;
            for (int j = 0; j < N; j++) {
                //Linie Poziome

                if (j < N - 1) {
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                }

                //Linie Pionowe
                glColor3f(0.0f, 1.0f, 0.0f);
                if (i == N - 1) {
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                }
                else {
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glVertex3f(EggPoints[i + 1][j].x, EggPoints[i + 1][j].y - 5, EggPoints[i + 1][j].z);
                }

            }
            if (solver != N) {

                //Domykanie koła w liniach poziomych
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                glVertex3f(EggPoints[solver][N - 1].x, EggPoints[solver][N - 1].y - 5, EggPoints[solver][N - 1].z);


                glColor3f(0.0f, 1.0f, 0.0f);
                if (i == N - 1) {
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                }
                glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                glVertex3f(EggPoints[i - 1][0].x, EggPoints[i - 1][0].y - 5, EggPoints[i - 1][0].z);

            }
        }

        //Linie skośne 
        glColor3f(0.0f, 0.0f, 1.0f);
        for (int i = N / 2; i < N; i++) {
            int solver = N - i;
            for (int j = 0; j < N - 1; j++) {
                if (i == N - 1) {
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                }
                else {
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glVertex3f(EggPoints[i + 1][j + 1].x, EggPoints[i + 1][j + 1].y - 5, EggPoints[i + 1][j + 1].z);
                }
                if (solver != N) {
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glVertex3f(EggPoints[solver + 1][N - 1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);
                }
            }
        }

        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glVertex3f(EggPoints[1][N - 1].x, EggPoints[1][N - 1].y - 5, EggPoints[1][N - 1].z);

        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glVertex3f(EggPoints[N - 1][N - 1].x, EggPoints[N - 1][N - 1].y - 5, EggPoints[N - 1][N - 1].z);


        for (int i = 1; i < N / 2 + 1; i++) {
            int solver = N - i;
            for (int j = 0; j < N - 1; j++) {

                glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                glVertex3f(EggPoints[i - 1][j + 1].x, EggPoints[i - 1][j + 1].y - 5, EggPoints[i - 1][j + 1].z);

                if (solver != N) {
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glVertex3f(EggPoints[solver - 1][N - 1].x, EggPoints[solver - 1][N - 1].y - 5, EggPoints[solver - 1][N - 1].z);
                }
            }
        }
        glEnd();
        break;
    case 3:

        glBegin(GL_TRIANGLES);
        for (int i = N / 2; i < N; i++) {
            int solver = N - i;
            for (int j = 0; j < N - 1; j++) {
                if (i == N - 1) {
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glNormal3f(EggPoints[i][j + 1].NVectorX, EggPoints[i][j + 1].NVectorY, EggPoints[i][j + 1].NVectorZ);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                    glNormal3f(EggPoints[0][0].NVectorX, EggPoints[0][0].NVectorY, EggPoints[0][0].NVectorZ);

                }
                else {
                    
                    
                    glVertex3f(EggPoints[i + 1][j + 1].x, EggPoints[i + 1][j + 1].y - 5, EggPoints[i + 1][j + 1].z);
                    glNormal3f(EggPoints[i + 1][j + 1].NVectorX, EggPoints[i + 1][j + 1].NVectorY, EggPoints[i + 1][j + 1].NVectorZ);
                    
                    glVertex3f(EggPoints[i + 1][j].x, EggPoints[i + 1][j].y - 5, EggPoints[i + 1][j].z);
                    glNormal3f(EggPoints[i + 1][j].NVectorX, EggPoints[i + 1][j].NVectorY, EggPoints[i + 1][j].NVectorZ);

                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);


                    //glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    //glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);
                    //glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    //glNormal3f(EggPoints[i][j + 1].NVectorX, EggPoints[i][j + 1].NVectorY, EggPoints[i][j + 1].NVectorZ);
                    //glVertex3f(EggPoints[i + 1][j + 1].x, EggPoints[i + 1][j + 1].y - 5, EggPoints[i + 1][j + 1].z);
                    //glNormal3f(EggPoints[i + 1][j + 1].NVectorX, EggPoints[i + 1][j + 1].NVectorY, EggPoints[i + 1][j + 1].NVectorZ);

                }
                if (solver != N) {
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glNormal3f(EggPoints[i][0].NVectorX, EggPoints[i][0].NVectorY, EggPoints[i][0].NVectorZ);
                    glVertex3f(EggPoints[solver][N - 1].x, EggPoints[solver][N - 1].y - 5, EggPoints[solver][N - 1].z);
                    glNormal3f(EggPoints[solver][N - 1].NVectorX, EggPoints[solver][N - 1].NVectorY, EggPoints[solver][N - 1].NVectorZ);
                    glVertex3f(EggPoints[solver + 1][N - 1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);
                    glNormal3f(EggPoints[solver + 1][N - 1].NVectorX, EggPoints[solver + 1][N - 1].NVectorY, EggPoints[solver + 1][N - 1].NVectorZ);

                    if (i != N / 2) {
                        glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                        glNormal3f(EggPoints[i][0].NVectorX, EggPoints[i][0].NVectorY, EggPoints[i][0].NVectorZ);
                        glVertex3f(EggPoints[i - 1][0].x, EggPoints[i - 1][0].y - 5, EggPoints[i - 1][0].z);
                        glNormal3f(EggPoints[i - 1][0].NVectorX, EggPoints[i - 1][0].NVectorY, EggPoints[i - 1][0].NVectorZ);
                        glVertex3f(EggPoints[solver + 1][N - 1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);
                        glNormal3f(EggPoints[solver + 1][N - 1].NVectorX, EggPoints[solver + 1][N - 1].NVectorY - 5, EggPoints[solver + 1][N - 1].NVectorZ);

                    }
                }
            }
        }
        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glNormal3f(EggPoints[0][0].NVectorX, EggPoints[0][0].NVectorY, EggPoints[0][0].NVectorZ);
        glVertex3f(EggPoints[N - 1][0].x, EggPoints[N - 1][0].y - 5, EggPoints[N - 1][0].z);
        glNormal3f(EggPoints[N - 1][0].NVectorX, EggPoints[N - 1][0].NVectorY, EggPoints[N - 1][0].NVectorZ);
        glVertex3f(EggPoints[1][N - 1].x, EggPoints[1][N - 1].y - 5, EggPoints[1][N - 1].z);
        glNormal3f(EggPoints[1][N - 1].NVectorX, EggPoints[1][N - 1].NVectorY, EggPoints[1][N - 1].NVectorZ);



        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glNormal3f(EggPoints[0][0].NVectorX, EggPoints[0][0].NVectorY, EggPoints[0][0].NVectorZ);
        glVertex3f(EggPoints[1][0].x, EggPoints[1][0].y - 5, EggPoints[1][0].z);
        glNormal3f(EggPoints[1][0].NVectorX, EggPoints[1][0].NVectorY, EggPoints[1][0].NVectorZ);
        glVertex3f(EggPoints[N - 1][N - 1].x, EggPoints[N - 1][N - 1].y - 5, EggPoints[N - 1][N - 1].z);
        glNormal3f(EggPoints[N - 1][N - 1].NVectorX, EggPoints[N - 1][N - 1].NVectorY, EggPoints[N - 1][N - 1].NVectorZ);




        for (int i = 0; i < N / 2 + 1; i++) {
            int pom = N - i;
            for (int j = 0; j < N - 1; j++) {
                if (i == 1) {
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glNormal3f(EggPoints[i][j + 1].NVectorX, EggPoints[i][j + 1].NVectorY, EggPoints[i][j + 1].NVectorZ);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                    glNormal3f(EggPoints[0][0].NVectorX, EggPoints[0][0].NVectorY, EggPoints[0][0].NVectorZ);

                }
                else {
                    if (i != 0) {
                        glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                        glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);
                        glVertex3f(EggPoints[i - 1][j].x, EggPoints[i - 1][j].y - 5, EggPoints[i - 1][j].z);
                        glNormal3f(EggPoints[i - 1][j].NVectorX, EggPoints[i - 1][j].NVectorY, EggPoints[i - 1][j].NVectorZ);
                        glVertex3f(EggPoints[i - 1][j + 1].x, EggPoints[i - 1][j + 1].y - 5, EggPoints[i - 1][j + 1].z);
                        glNormal3f(EggPoints[i - 1][j + 1].NVectorX, EggPoints[i - 1][j + 1].NVectorY, EggPoints[i - 1][j + 1].NVectorZ);





                        glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                        glNormal3f(EggPoints[i][j].NVectorX, EggPoints[i][j].NVectorY, EggPoints[i][j].NVectorZ);
                        glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                        glNormal3f(EggPoints[i][j + 1].NVectorX, EggPoints[i][j + 1].NVectorY, EggPoints[i][j + 1].NVectorZ);
                        glVertex3f(EggPoints[i - 1][j + 1].x, EggPoints[i - 1][j + 1].y - 5, EggPoints[i - 1][j + 1].z);
                        glNormal3f(EggPoints[i - 1][j + 1].NVectorX, EggPoints[i - 1][j + 1].NVectorY, EggPoints[i - 1][j + 1].NVectorZ);
                    }
                }
                if (pom != N) {
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glNormal3f(EggPoints[i][0].NVectorX, EggPoints[i][0].NVectorY, EggPoints[i][0].NVectorZ);
                    glVertex3f(EggPoints[pom][N - 1].x, EggPoints[pom][N - 1].y - 5, EggPoints[pom][N - 1].z);
                    glNormal3f(EggPoints[pom][N - 1].NVectorX, EggPoints[pom][N - 1].NVectorY, EggPoints[pom][N - 1].NVectorZ);
                    glNormal3f(EggPoints[pom - 1][N - 1].NVectorX, EggPoints[pom - 1][N - 1].NVectorY, EggPoints[pom - 1][N - 1].NVectorZ);
                    glVertex3f(EggPoints[pom - 1][N - 1].x, EggPoints[pom - 1][N - 1].y - 5, EggPoints[pom - 1][N - 1].z);




                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glNormal3f(EggPoints[i][0].NVectorX, EggPoints[i][0].NVectorY, EggPoints[i][0].NVectorZ);
                    glVertex3f(EggPoints[i + 1][0].x, EggPoints[i + 1][0].y - 5, EggPoints[i + 1][0].z);
                    glNormal3f(EggPoints[i + 1][0].NVectorX, EggPoints[i + 1][0].NVectorY, EggPoints[i + 1][0].NVectorZ);
                    glVertex3f(EggPoints[pom - 1][N - 1].x, EggPoints[pom - 1][N - 1].y - 5, EggPoints[pom - 1][N - 1].z);
                    glNormal3f(EggPoints[pom - 1][N - 1].NVectorX, EggPoints[pom - 1][N - 1].NVectorY, EggPoints[pom - 1][N - 1].NVectorZ);


                }
            }
        }

        glEnd();
        break;
    case 4:
        glutSolidTeapot(3.0);
        break;
    default:
        break;
    }

}



void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej

    if (status == 2) {
        R += delta_y;
        viewer[0] = cos(cameraTheta * M_PI / 180) * cos(cameraPhi * M_PI / 180) * R;
        viewer[1] = sin(cameraPhi * M_PI / 180) * R;
        viewer[2] = sin(cameraTheta * M_PI / 180) * cos(cameraPhi * M_PI / 180) * R;
    }

    if (status == 1 && rotationStatus == 1)                     // jeśli lewy klawisz myszy wcięnięty
    {
        calculateAngles();
        viewer[0] = cos(cameraTheta * M_PI / 180) * cos(cameraPhi * M_PI / 180) * R;
        viewer[1] = sin(cameraPhi * M_PI / 180) * R;
        viewer[2] = sin(cameraTheta * M_PI / 180) * cos(cameraPhi * M_PI / 180) * R;

        if (cameraPhi > 90 && cameraPhi < 270) {
            up = -1;
        }
        else {
            up = 1;
        }

    }


    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, up, 0.0);
    // Zdefiniowanie położenia obserwatora
    Axes();
    // Narysowanie osi przy pomocy funkcji zdefiniowanej powyżej 


    if (status == 1 && rotationStatus == 0)                     // jeśli lewy klawisz myszy wcięnięty
    {
        theta[0] += delta_y * pix2angleY;    // modyfikacja kąta obrotu o kat proporcjonalny

        theta[1] += delta_x * pix2angleX;    // modyfikacja kąta obrotu o kat proporcjonalny
    }

    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);




    Egg();

    glFlush();
    // Przekazanie poleceń rysujących do wykonania

    glutSwapBuffers();
    //
}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Kolor czyszcący (wypełnienia okna) ustawiono na czarny
    /*************************************************************************************/

//  Definicja materiału z jakiego zrobiony jest czajnik
//  i definicja źródła światła

/*************************************************************************************/


/*************************************************************************************/
// Definicja materiału z jakiego zrobiony jest czajnik

    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki ka =[kar,kag,kab] dla światła otoczenia

    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               

    GLfloat mat_shininess = { 20.0 };
    // współczynnik n opisujący połysk powierzchni

/*************************************************************************************/
// Definicja źródła światła

    GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
    // położenie źródła

    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    // składowe intensywności świecenia źródła światła otoczenia
    // Ia = [Iar,Iag,Iab]

    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie dyfuzyjne Id = [Idr,Idg,Idb]

    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // składowe intensywności świecenia źródła światła powodującego
    // odbicie kierunkowe Is = [Isr,Isg,Isb]

    GLfloat att_constant = { 1.0 };
    // składowa stała ds dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_linear = { 0.05 };
    // składowa liniowa dl dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

    GLfloat att_quadratic = { 0.001 };
    // składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
    // odległości od źródła

/*************************************************************************************/
// Ustawienie parametrów materiału i źródła światła

/*************************************************************************************/
// Ustawienie patrametrów materiału


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    /*************************************************************************************/
    // Ustawienie parametrów źródła

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


    /*************************************************************************************/
    // Ustawienie opcji systemu oświetlania sceny

    glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
    glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
    glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

/*************************************************************************************/




}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    pix2angleX = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
    pix2angleY = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji

    glLoadIdentity();
    // Czyszcznie macierzy bieżącej

    gluPerspective(70, 1.0, 1.0, 30.0);
    // Ustawienie parametrów dla rzutu perspektywicznego


    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
    // Ustawienie wielkości okna okna widoku (viewport) w zależności
    // relacji pomiędzy wysokością i szerokością okna

    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu  

    glLoadIdentity();
    // Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli


void clearEggPoints() {
    for (int i = 0; i < N; i++)
        delete EggPoints[i];
    delete[] EggPoints;
}

void keys(unsigned char key, int x, int y)
{
    if (key == 'q') model = 1;
    if (key == 'w') model = 2;
    if (key == 'e') model = 3;
    if (key == 'r') model = 4;
    if (key == 'u') {
        clearEggPoints();
        layers++;
        N = layers * 2 - 2;
        step = 1.0 / N;
        GenerateEggPoints();
    }
    if (key == 'd') {
        clearEggPoints();
        layers--;
        N = layers * 2 - 2;
        step = 1.0 / N;
        GenerateEggPoints();
    }
    if (key == 'c') {
        rotationStatus = rotationStatus == 1 ? 0 : 1;
    }


    RenderScene();
}




void main(void)
{

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Układ współrzędnych 3-D");

    GenerateEggPoints();



    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);

    std::cout << "Sterowanie programem:\n q-punkty \n w-siatka\n e-trojkaty\n r-dzbanek\c-zmiana trybu obrotu kamera/objekt \n u-Zwieksz N\n d-Zmniejsz N";
    glutKeyboardFunc(keys);

    glutMouseFunc(Mouse);
    // Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

    glutMotionFunc(Motion);
    // Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy




    glutIdleFunc(spinEgg);


    MyInit();
    // Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przystąpieniem do renderowania 

    glEnable(GL_DEPTH_TEST);
    // Włączenie mechanizmu usuwania powierzchni niewidocznych

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/