
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <vector>
#include <string>
#include <cmath>

#define POINTS 4

struct Point {
    float x, y, z;
};

class Face {
private:
    Point points[POINTS];   // Top right
public:
    Face(Point bl, Point br, Point tl, Point tr) {
        this->points[0] = bl;
        this->points[1] = br;
        this->points[2] = tl;
        this->points[3] = tr;
    }

    Face() {
        this->points[0] = {0.0f, 0.0f, 0.0f};
        this->points[1] = {0.0f, 0.0f, 0.0f};
        this->points[2] = {0.0f, 0.0f, 0.0f};
        this->points[3] = {0.0f, 0.0f, 0.0f};
    }

    void draw() {
        glBegin(GL_POLYGON);
            glVertex3f(points[0].x, points[0].y, points[0].z);
            glVertex3f(points[1].x, points[1].y, points[1].z);
            glVertex3f(points[2].x, points[2].y, points[2].z);
            glVertex3f(points[3].x, points[3].y, points[3].z);
        glEnd();
    }
};

class Cuboid {
private:
    Face f_front,
         f_back,
         f_bottom,
         f_top,
         f_left,
         f_right;
    std::string type;
    std::vector<Cuboid*> joints;
public:
    Cuboid(float height, float width, float length, std::string type) {
        // Face(float bl, float br, float tl, float tr)
        std::vector<Point> pts{{0.0f, 0.0f, length}, {width, 0.0f, length}, {0.0f, height, length}, {height, width, length}};
        f_front = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, 0}, {width, 0.0f, 0}, {0.0f, height, 0}, {height, width, 0}};
        f_back = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, 0}, {width, 0.0f, 0}, {0.0f, height, 0}, {height, width, 0}};
        f_bottom = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, length}, {width, 0.0f, 0.0f}, {0.0f, width, length}};
        f_top = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, height, 0.0f}, {0.0f, height, length}, {width, height, 0.0f}, {width, height, length}};
        f_left = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, length},{0.0f, height, 0.0f}, {0.0f, height, length}};
        f_right = Face(pts[0], pts[1], pts[2], pts[3]);
    }

    Cuboid() {
        f_front = Face({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        f_back = Face({1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        f_bottom = Face({1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        f_top = Face({1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        f_left = Face({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        f_right = Face({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
    }

    void draw() {
        float colors[3];
        colors[0] = static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
        colors[1] = static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
        colors[2] = static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
        f_front.draw();
        f_back.draw();
        f_bottom.draw();
        f_top.draw();
        f_left.draw();
        f_right.draw();
    }

    void link(Cuboid &cbd) {

    }

    void f_move(std::string face, Point d_bl, Point d_br, Point d_tl, Point d_tr) {

    }
};

class Dummy{
private:
    Cuboid chest,
           head,
           left_arm,
           left_forearm,
           right_arm,
           right_forearm,
           upper_right_leg,
           lower_right_leg,
           upper_left_leg,
           lower_left_leg;
public:
    Dummy() {
        // Builds a dummy with the following dimensions:
        // Cuboid(float height, float width, float length, std::string type)
        this->head = Cuboid(20.0f, 20.0f, 10.0f, "head");
        this->chest = Cuboid(30.0f, 10.0f, 10.0f, "chest");
        this->left_arm = Cuboid(10.0f, 10.0f, 20.0f, "arm");
        this->left_forearm = Cuboid(10.0f, 10.0f, 20.0f, "arm");
        this->right_arm = Cuboid(10.0f, 10.0f, 20.0f, "arm");
        this->right_forearm = Cuboid(10.0f, 10.0f, 20.0f, "arm");
        this->upper_left_leg = Cuboid(30.0f, 10.0f, 10.0f, "upper_leg");
        this->upper_right_leg = Cuboid(30.0f, 10.0f, 10.0f, "upper_leg");
        this->lower_left_leg = Cuboid(30.0f, 10.0f, 10.0f, "lower_leg");
        this->lower_right_leg = Cuboid(30.0f, 10.0f, 10.0f, "lower_leg");
    }

    // Funções de Impressão

    void draw() {
        this->head.draw();
        this->chest.draw();
        this->left_arm.draw();
        this->left_forearm.draw();
        this->right_arm.draw();
        this->right_forearm.draw();
        this->upper_left_leg.draw();
        this->upper_right_leg .draw();
        this->lower_left_leg.draw();
        this->lower_right_leg.draw();
    }

    // Funções de movimento

    void walk() {

    }

    void turn() {

    }

    void jump() {

    }

    void look_up() {

    }

    void look_down() {

    }
};

Dummy dm = Dummy();

static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f,0.0f,0.0f);
    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y) {
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(50,50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Trabalho Robô Lab. Computação Gráfica");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(1.0f,1.0f,1.0f,1.0f);

    glutMainLoop();

    return EXIT_SUCCESS;
}
