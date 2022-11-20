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

/*************************************************************************************/

// Funkcja rysująca osie układu współrzędnych

struct Point {
    float x;
    float y;
    float z;

    float R;
    float G;
    float B;
};

//dzielna podziałów boku kwadratu jednostkoweg
const int N = 50
;
//tablica przechowująca punkty jajka
struct Point EggPoints[N][N];
//zmienna odpowiedzialna za aktualny model
int model = 1;  // 1- punkty, 2- siatka, 3 - wypełnione trójkąty

static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu

float step = 1.0 / N;


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

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)




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


void GenerateEggPoints() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            EggPoints[i][j].x = generateX(i * step, j * step);
            EggPoints[i][j].y = generateY(i * step, j * step);
            EggPoints[i][j].z = generateZ(i * step, j * step);

            //Kolorki
            EggPoints[i][j].R = ((double)rand() / (RAND_MAX));
            EggPoints[i][j].G = ((double)rand() / (RAND_MAX));
            EggPoints[i][j].B = ((double)rand() / (RAND_MAX));
            
        }
    }
}


void spinEgg()
{

    theta[0] -= 0.05;
    if (theta[0] > 360.0) theta[0] -= 360.0;

    theta[1] -= 0.05;
    if (theta[1] > 360.0) theta[1] -= 360.0;

    theta[2] -= 0.05;
    if (theta[2] > 360.0) theta[2] -= 360.0;

    glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
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
            for (int j = 0; j < N ; j++) {
                //Linie Poziome
                
                if (j < N-1) {
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
                    glVertex3f(EggPoints[solver + 1][N-1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);
                }
            }
        }

        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glVertex3f(EggPoints[1][N -1].x, EggPoints[1][N - 1].y - 5, EggPoints[1][N - 1].z);

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
                    glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glColor3f(EggPoints[i][j + 1].R, EggPoints[i][j + 1].G, EggPoints[i][j + 1].B);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glColor3f(EggPoints[0][0].R, EggPoints[0][0].G, EggPoints[0][0].B);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                }
                else {
                    glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glColor3f(EggPoints[i + 1][j].R, EggPoints[i + 1][j].G, EggPoints[i + 1][j].B);
                    glVertex3f(EggPoints[i + 1][j].x, EggPoints[i + 1][j].y - 5, EggPoints[i + 1][j].z);
                    glColor3f(EggPoints[i + 1][j + 1].R, EggPoints[i + 1][j + 1].G, EggPoints[i + 1][j + 1].B);
                    glVertex3f(EggPoints[i + 1][j + 1].x, EggPoints[i + 1][j + 1].y - 5, EggPoints[i + 1][j + 1].z);

                    glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glColor3f(EggPoints[i][j + 1].R, EggPoints[i][j + 1].G, EggPoints[i][j + 1].B);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glColor3f(EggPoints[i + 1][j + 1].R, EggPoints[i + 1][j + 1].G, EggPoints[i + 1][j + 1].B);
                    glVertex3f(EggPoints[i + 1][j + 1].x, EggPoints[i + 1][j + 1].y - 5, EggPoints[i + 1][j + 1].z);
                }
                if (solver != N) {
                    glColor3f(EggPoints[i][0].R, EggPoints[i][0].G, EggPoints[i][0].B);
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glColor3f(EggPoints[solver][N - 1].R, EggPoints[solver][N - 1].G, EggPoints[solver][N - 1].B);
                    glVertex3f(EggPoints[solver][N - 1].x, EggPoints[solver][N - 1].y - 5, EggPoints[solver][N - 1].z);
                    glColor3f(EggPoints[solver + 1][N - 1].R, EggPoints[solver + 1][N - 1].G, EggPoints[solver + 1][N - 1].B);
                    glVertex3f(EggPoints[solver + 1][N - 1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);

                    if (i != N / 2) {
                        glColor3f(EggPoints[i][0].R, EggPoints[i][0].G, EggPoints[i][0].B);
                        glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                        glColor3f(EggPoints[i - 1][0].R, EggPoints[i - 1][0].G, EggPoints[i - 1][0].B);
                        glVertex3f(EggPoints[i - 1][0].x, EggPoints[i - 1][0].y - 5, EggPoints[i - 1][0].z);
                        glColor3f(EggPoints[solver + 1][N - 1].R, EggPoints[solver + 1][N - 1].G, EggPoints[solver + 1][N - 1].B);
                        glVertex3f(EggPoints[solver + 1][N - 1].x, EggPoints[solver + 1][N - 1].y - 5, EggPoints[solver + 1][N - 1].z);
                    }
                }
            }
        }
        glColor3f(EggPoints[0][0].R, EggPoints[0][0].G, EggPoints[0][0].B);
        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glColor3f(EggPoints[N - 1][0].R, EggPoints[N - 1][0].G, EggPoints[N - 1][0].B);
        glVertex3f(EggPoints[N - 1][0].x, EggPoints[N - 1][0].y - 5, EggPoints[N - 1][0].z);
        glColor3f(EggPoints[1][N - 1].R, EggPoints[1][N - 1].G, EggPoints[1][N - 1].B);
        glVertex3f(EggPoints[1][N - 1].x, EggPoints[1][N - 1].y - 5, EggPoints[1][N - 1].z);


        glColor3f(EggPoints[0][0].R, EggPoints[0][0].G, EggPoints[0][0].B);
        glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
        glColor3f(EggPoints[1][0].R, EggPoints[1][0].G, EggPoints[1][0].B);
        glVertex3f(EggPoints[1][0].x, EggPoints[1][0].y - 5, EggPoints[1][0].z);
        glColor3f(EggPoints[N - 1][N - 1].R, EggPoints[N - 1][N - 1].G, EggPoints[N - 1][N - 1].B);
        glVertex3f(EggPoints[N - 1][N - 1].x, EggPoints[N - 1][N - 1].y - 5, EggPoints[N - 1][N - 1].z);


        for (int i = 0; i < N / 2 + 1; i++) {
            int pom = N - i;
            for (int j = 0; j < N - 1; j++) {
                if (i == 1) {
                    glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glColor3f(EggPoints[i][j + 1].R, EggPoints[i][j + 1].G, EggPoints[i][j + 1].B);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glColor3f(EggPoints[0][0].R, EggPoints[0][0].G, EggPoints[0][0].B);
                    glVertex3f(EggPoints[0][0].x, EggPoints[0][0].y - 5, EggPoints[0][0].z);
                }
                else {
                    if (i != 0) {
                        glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                        glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                        glColor3f(EggPoints[i - 1][j].R, EggPoints[i - 1][j].G, EggPoints[i - 1][j].B);
                        glVertex3f(EggPoints[i - 1][j].x, EggPoints[i - 1][j].y - 5, EggPoints[i - 1][j].z);
                        glColor3f(EggPoints[i - 1][j + 1].R, EggPoints[i - 1][j + 1].G, EggPoints[i - 1][j + 1].B);
                        glVertex3f(EggPoints[i - 1][j + 1].x, EggPoints[i - 1][j + 1].y - 5, EggPoints[i - 1][j + 1].z);
                    }


                    glColor3f(EggPoints[i][j].R, EggPoints[i][j].G, EggPoints[i][j].B);
                    glVertex3f(EggPoints[i][j].x, EggPoints[i][j].y - 5, EggPoints[i][j].z);
                    glColor3f(EggPoints[i][j + 1].R, EggPoints[i][j + 1].G, EggPoints[i][j + 1].B);
                    glVertex3f(EggPoints[i][j + 1].x, EggPoints[i][j + 1].y - 5, EggPoints[i][j + 1].z);
                    glColor3f(EggPoints[i - 1][j + 1].R, EggPoints[i - 1][j + 1].G, EggPoints[i - 1][j + 1].B);
                    glVertex3f(EggPoints[i - 1][j + 1].x, EggPoints[i - 1][j + 1].y - 5, EggPoints[i - 1][j + 1].z);
                }
                if (pom != N) {
                    glColor3f(EggPoints[i][0].R, EggPoints[i][0].G, EggPoints[i][0].B);
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glColor3f(EggPoints[pom][N - 1].R, EggPoints[pom][N - 1].G, EggPoints[pom][N - 1].B);
                    glVertex3f(EggPoints[pom][N - 1].x, EggPoints[pom][N - 1].y - 5, EggPoints[pom][N - 1].z);
                    glColor3f(EggPoints[pom - 1][N - 1].R, EggPoints[pom - 1][N - 1].G, EggPoints[pom - 1][N - 1].B);
                    glVertex3f(EggPoints[pom - 1][N - 1].x, EggPoints[pom - 1][N - 1].y - 5, EggPoints[pom - 1][N - 1].z);


                    glColor3f(EggPoints[i][0].R, EggPoints[i][0].G, EggPoints[i][0].B);
                    glVertex3f(EggPoints[i][0].x, EggPoints[i][0].y - 5, EggPoints[i][0].z);
                    glColor3f(EggPoints[i + 1][0].R, EggPoints[i + 1][0].G, EggPoints[i + 1][0].B);
                    glVertex3f(EggPoints[i + 1][0].x, EggPoints[i + 1][0].y - 5, EggPoints[i + 1][0].z);
                    glColor3f(EggPoints[pom - 1][N - 1].R, EggPoints[pom - 1][N - 1].G, EggPoints[pom - 1][N - 1].B);
                    glVertex3f(EggPoints[pom - 1][N - 1].x, EggPoints[pom - 1][N - 1].y - 5, EggPoints[pom - 1][N - 1].z);
                }
            }
        }

        glEnd();
        break;
    case 4:
        glutWireTeapot(3.0);
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




    glColor3f(1.0f, 1.0f, 1.0f);


    glRotatef(theta[0], 1.0, 0.0, 0.0);

    glRotatef(theta[1], 0.0, 1.0, 0.0);

    glRotatef(theta[2], 0.0, 0.0, 1.0);

    Axes();
    // Narysowanie osi przy solverocy funkcji zdefiniowanej wyżej

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

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio  określającej proporcję
    // wymiarów okna 

    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0

        vertical = 1;

    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkościokna okna widoku (viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)  

    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji 

    glLoadIdentity();
    // Czyszcznie macierzy bieżącej            

    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie współczynnika  proporcji okna
    // Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
    // przestrzeni ograniczającej pozwalającej zachować właściwe
    // proporcje rysowanego obiektu.
    // Do okreslenia przestrzeni ograniczjącej służy funkcja
    // glOrtho(...)            

    if (horizontal <= vertical)

        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);

    else

        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);

    glMatrixMode(GL_MODELVIEW);
    // Przełączenie macierzy bieżącej na macierz widoku modelu                                    

    glLoadIdentity();
    // Czyszcenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

void keys(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'w') model = 2;
    if (key == 's') model = 3;

    RenderScene(); // przerysowanie obrazu sceny
}


void main(void)
{

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Układ współrzędnych 3-D");

    GenerateEggPoints();

    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną
    // (callback function).  Bedzie ona wywoływana za każdym razem
    // gdy zajdzie potrzba przeryswania okna 

    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
    // zazmiany rozmiaru okna      


    glutKeyboardFunc(keys);
    

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