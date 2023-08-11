
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <cmath>

#include <iostream>

#define POINTS 4

#define SCREEN_LENGTH 640
#define SCREEN_HEIGHT 480

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
        glBegin(GL_QUADS);
            glVertex3f(points[0].x, points[0].y, points[0].z);
            glVertex3f(points[1].x, points[1].y, points[1].z);
            glVertex3f(points[3].x, points[3].y, points[3].z);
            glVertex3f(points[2].x, points[2].y, points[2].z);
            /*
            std::cout << points[0].x << " " << points[0].y << " " << points[0].z << std::endl;
            std::cout << points[1].x << " " << points[1].y << " " << points[1].z << std::endl;
            std::cout << points[3].x << " " << points[3].y << " " << points[2].z << std::endl;
            std::cout << points[2].x << " " << points[2].y << " " << points[3].z << std::endl;
        */
        glEnd();
    }
};

struct transformation {
    char tp;            // Type
    // For rotations
    float deg;          // Degree
    float total_deg;    // Total degree variation
    // For translations
    Point d;
};

class Cuboid {
private:
    Face f_front,
         f_back,
         f_bottom,
         f_top,
         f_left,
         f_right;
    std::vector<transformation> transfs;    // Vector that stores all transformations
    std::vector<Cuboid*> joints;
    std::string type;
public:
    // I usually wouldn't use public variables, but time is short and using a getter would make the code harder to read/write
    Point tf_ref_pt;        // Reference point for transformations
    bool selected = false;
    Point dims; // Dimensions

    Cuboid(float height, float width, float length, std::string type) {
        dims = {height, width, length};
        setReferencePoint(width, height, length);
        // Face(float bl, float br, float tl, float tr)
        std::vector<Point> pts{{0.0f, 0.0f, length}, {width, 0.0f, length}, {0.0f, height, length}, {width, height, length}};
        f_front = Face(pts[0], pts[1], pts[2], pts[3]);
        //pts = {{0.0f, 0.0f, 0.0f}, {width, 0.0f, 0.0f}, {0.0f, height, 0.0f}, {width, height, 0.0f}};
        pts = {{0.0f, 0.0f, 0.0f}, {width, 0.0f, 0.0f}, {0.0f, height, 0.0f}, {width, height, 0.0f}};
        f_back = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, length}, {width, 0.0f, length}, {0.0f, 0.0f, 0.0f}, {width, 0.0f, 0.0f}};
        f_bottom = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, height, length}, {width, height, length}, {0.0f, height, 0.0f}, {width, height, 0.0f}};
        f_top = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, length}, {0.0f, height, 0.0f}, {0.0f, height, length}};
        f_left = Face(pts[0], pts[1], pts[2], pts[3]);
        pts = {{width, 0.0f, 0.0f}, {width, 0.0f, length}, {width, height, 0.0f}, {width, height, length}};
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
        pushActMx();
            f_front.draw();
            f_back.draw();
            f_bottom.draw();
            f_top.draw();
            f_left.draw();
            f_right.draw();
        popActMx();

    }

    void setReferencePoint(float x, float y, float z) {
        if(type == "head" || type == "chest")
            tf_ref_pt = {dims.x/2, 0.0f, dims.z/2};
        else if(type == "arm" || type == "forearm")
            tf_ref_pt = {dims.x, dims.y, dims.z/2};
        else if(type == "upper_leg" || type == "lower_leg")
            tf_ref_pt = {dims.x/2, dims.y, dims.z/2};
    }

    void pushActMx() {
        glPushMatrix();
        for(transformation tf : transfs)
            if(tf.tp = 't')
                glTranslatef(tf_ref_pt.x, tf_ref_pt.y, tf_ref_pt.z);
        for(transformation tf : transfs)
            if(tf.tp = 'r')
                glRotatef(tf.deg, tf_ref_pt.x, tf_ref_pt.y, tf_ref_pt.z);
    }

    void popActMx() {
        usleep(10000);
        glPopMatrix();
    }

    bool checkMovement(char type, Point params) {

    }

    void pushAction() {

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
    Point origem;
public:
    Dummy() {
        origem = {0.0f, 0.0f, 0.0f};
        // Builds a dummy with the following dimensions:
        // Cuboid(float height, float width, float length, std::string type)
        this->head = Cuboid(10.0f, 10.0f, 5.0f, "head");
        this->chest = Cuboid(20.0f, 12.5f, 5.0f, "chest");
        this->left_arm = Cuboid(10.0f, 5.0f, 5.0f, "arm");
        this->left_forearm = Cuboid(10.0f, 5.0f, 5.0f, "arm");
        this->right_arm = Cuboid(10.0f, 5.0f, 5.0f, "arm");
        this->right_forearm = Cuboid(10.0f, 5.0f, 5.0f, "arm");
        this->upper_left_leg = Cuboid(12.5f, 5.0f, 5.0f, "upper_leg");
        this->upper_right_leg = Cuboid(12.5f, 5.0f, 5.0f, "upper_leg");
        this->lower_left_leg = Cuboid(12.5f, 5.0f, 5.0f, "lower_leg");
        this->lower_right_leg = Cuboid(12.5f, 5.0f, 5.0f, "lower_leg");
    }

    void walk_legup() {}
    void walk_leg90deg() {}
    void walk_legdown() {}

    void walk() {
        // legup perna direita
        // leg90deg ambas as pernas
        // legdown direita e legup esquerda
        // leg90ged ambas as pernas
        // legdown esquerda e legup direita
        // legdown direita
    }

    void turn() {

    }

    void jump() {

    }

    void look_up() {

    }

    void look_down() {

    }

    void animate() {

    }

    // Funções de Impressão

    void draw() {
        glViewport(0, 0, SCREEN_LENGTH, SCREEN_HEIGHT);
        /*
            Os parametros do translate assume valores diferentes se o boneco estiver se movimentando.
                Para este caso, se há uma animação, movimenta-se a origem e soma-se a origem aos
                parâmetros do translate.
            Para os movimentos dos membros, são chamados rotates com os ângulos de cada caso.
         */

        animate();

        // Desenho das pernas
        glColor3f(1.0f, 0.0f, 0.0f);
        this->lower_left_leg.draw();
        glPushMatrix();
            glRotatef(0.1f, lower_left_leg.tf_ref_pt.x, lower_left_leg.tf_ref_pt.y, lower_left_leg.tf_ref_pt.z);
            glTranslatef(lower_left_leg.dims.x + (chest.dims.x - 2*lower_left_leg.dims.x),
                         0.0f, 0.0f); // Deslocamento para imprimir perna direita
            this->lower_right_leg.draw();
        glPopMatrix();
        // Desenho das coxas
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, upper_left_leg.dims.y, 0.0f); // Deslocamento para imprimir coxa esquerda
            this->upper_left_leg.draw();
            glTranslatef(upper_left_leg.dims.x + (chest.dims.x - 2*upper_left_leg.dims.x), 0.0f, 0.0f); // Deslocamento para imprimir coxa direita
            this->upper_right_leg .draw();
        glPopMatrix();
        // Desenho do torso
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, upper_left_leg.dims.y + lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir o torso a partir do canto inferior esquerdo
            this->chest.draw();
        glPopMatrix();
        // Desenho do braco
        glColor3f(0.5f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(-1*left_forearm.dims.x/2,
                         upper_left_leg.dims.y + lower_left_leg.dims.y + left_forearm.dims.y, 0.0f); // Deslocamento para imprimir a partir da mao esquerda
            this->left_forearm.draw();
            glPushMatrix();
                glTranslatef(chest.dims.x - right_arm.dims.x/4, 0.0f, 0.0f); // Deslocamento para imprimir a partir da mao direita
                this->right_forearm.draw();
            glPopMatrix();
            glColor3f(0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, left_forearm.dims.y, 0.0f); // Deslocamento para imprimir a partir do cotovelo esquerdo
            this->left_arm.draw();
            glTranslatef(chest.dims.x - right_arm.dims.x/4, 0.0f, 0.0f); // Deslocamento para imprimir a partir do cotovelo direito
            this->right_arm.draw();
        glPopMatrix();
        // Desenho da cabeca
        glColor3f(0.0f, 1.0f, 0.5f);
        glPushMatrix();
            glTranslatef(head.dims.x / 5, lower_left_leg.dims.y + upper_left_leg.dims.y + chest.dims.y + 7.5f, 0.0f); // Deslocamento para imprimir a partir do pescoco pelo lado esquerdo
            this->head.draw();
        glPopMatrix();
    }
};

Dummy dm = Dummy();

static void resize(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble) width/height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(-10.0, 20.0, 100.0, 0.0, 30.0, 0.0, 0.0, 1.0, 0.0);
    dm.draw();

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y) {
    static char selected = 'n';

    switch (key) {
        case 27 :
        case 'q':
            exit(0);
            break;
        case 'w':
            std::cout << "Starting walk animation" << std::endl;
            // Set walk animation
            break;
        case 't':
            std::cout << "Torso selected" << std::endl;
            // Select chest
            break;
        case 'h':
            std::cout << "Head selected" << std::endl;
            // Select chest
            break;
        case 'a':
            std::cout << "Arm selected" << std::endl;
            // Select chest
            break;
        case 'f':
            std::cout << "Forearm selected" << std::endl;
            // Select chest
            break;
        case 'c':
            std::cout << "Coxa selected" << std::endl;
            // Select coxa
            break;
        case 'p':
            std::cout << "Panturrilha selected" << std::endl;
            // Select panturrilha
            break;
        case 'l':
            break;
        case 'r':
            break;
    }

    glutPostRedisplay();
}

static void idle(void) {
    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_LENGTH,SCREEN_HEIGHT);
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
