
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

struct RotationInfo {
    float deg, x, y, z;
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

// Could be better structured, but this implementation allows for better readability
struct transformation {
    char tp;            // Type
    // For rotations
    float deg;          // Degree
    float total_deg;    // Total degree variation
    Point ax;          // Rotation axis
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
    std::vector<Cuboid*> joints;            // Reference to the other body members
    std::string type;                       // Shows the member's type
public:
    // I usually wouldn't use public variables, but time is short and using a getter would make the code harder to read/write
    bool selected = false;
    Point tf_ref_pt;        // Reference point for transformations
    Point dims;             // Dimensions

    Cuboid(float height, float width, float length, std::string type) {
        dims = {width, height, length};
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

    // For lower legs, forearms and head
    void link(Cuboid *only_member) {
        joints.push_back(only_member);
    }

    // For upper legs and arms
    void link(Cuboid *lower_member, Cuboid *chest) {
        joints.push_back(chest);
        joints.push_back(lower_member);
    }

    // For chest only
    void link(Cuboid *head, Cuboid *left_arm, Cuboid *right_arm, Cuboid *left_leg, Cuboid *right_leg) {
        joints.push_back(head);
        joints.push_back(left_arm);
        joints.push_back(right_arm);
        joints.push_back(left_leg);
        joints.push_back(right_leg);
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
        if(type == "right_forearm" || type == "left_forearm" || type == "lower_right_leg" || type == "lower_left_leg") {
            glTranslatef(dims.x, dims.y + joints[1].dims.y, 0.0f);          // Returns to origin
            for(transformation tf : transfs)                                // Translation loop
                if(tf.tp = 't')
                    glTranslatef(tf_ref_pt.x, tf_ref_pt.y, tf_ref_pt.z);
            for(transformation tf : transfs)                                // Rotation loop
                if(tf.tp = 'r')
                    glRotatef(deg, 0.0, 0.0, 1.0);
            glTranslatef(-dims.x, - dims.y - joints[1].dims.y, 0.0f);       // Sets position for rotation
        } else if(type == "right_arm" || type == "left_arm" || type == "upper_right_leg" || type == "upper_left_leg") {
            glTranslatef(dims.x, dims.y, 0.0f);
            for(transformation tf : transfs)
                glRotatef(deg, 0.0, 0.0, 1.0);
            glTranslatef(-dims.x, -dims.y, 0.0f);
        }
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
        this->left_arm = Cuboid(10.0f, 5.0f, 5.0f, "left_arm");
        this->left_forearm = Cuboid(10.0f, 5.0f, 5.0f, "left_forearm");
        this->right_arm = Cuboid(10.0f, 5.0f, 5.0f, "right_arm");
        this->right_forearm = Cuboid(10.0f, 5.0f, 5.0f, "right_forearm");
        this->upper_left_leg = Cuboid(12.5f, 5.0f, 5.0f, "upper_left_leg");
        this->upper_right_leg = Cuboid(12.5f, 5.0f, 5.0f, "upper_right_leg");
        this->lower_left_leg = Cuboid(12.5f, 5.0f, 5.0f, "lower_left_leg");
        this->lower_right_leg = Cuboid(12.5f, 5.0f, 5.0f, "lower_right_leg");
    }

    void setRotationDirection(char key, char part) {
        transformation tf;

        if(key == 'j') {

        } else if(key == 'l') {

        } else if(key == 'k') {

        } else if(key == 'i') {

        }
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

        float deg = -45.0f;

        // Desenho das pernas
        this->lower_left_leg.draw();
        glPushMatrix();
            glTranslatef(lower_left_leg.dims.x + (chest.dims.x - 2*lower_left_leg.dims.x),
                         0.0f, 0.0f); // Deslocamento para imprimir perna direita
            this->lower_right_leg.draw();
        glPopMatrix();

        // Desenho das coxas
        glPushMatrix();
            glTranslatef(0.0f, lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir coxa esquerda
            this->upper_left_leg.draw();
            glTranslatef(upper_left_leg.dims.x + (chest.dims.x - 2*upper_left_leg.dims.x), 0.0f, 0.0f); // Deslocamento para imprimir coxa direita
            this->upper_right_leg .draw();
        glPopMatrix();

        // Desenho do braço
        glPushMatrix();
            glTranslatef(-left_forearm.dims.x, lower_left_leg.dims.y + upper_left_leg.dims.y + left_forearm.dims.y, 0.0f); // Deslocamento para imprimir a partir do cotovelo esquerdo
            glPushMatrix();
                /*
                glTranslatef(left_forearm.dims.x, left_forearm.dims.y, 0.0f);
                glRotatef(deg, 0.0, 0.0, 1.0);
                glTranslatef(-left_forearm.dims.x, -left_forearm.dims.y, 0.0f);
                */
                this->left_arm.draw();
            glPopMatrix();
            glTranslatef(left_forearm.dims.x + chest.dims.x, 0.0f, 0.0f); // Deslocamento para imprimir a partir do cotovelo direito
            this->right_arm.draw();
        glPopMatrix();

        // Desenho do antebraco
        glPushMatrix();
            glTranslatef(-1*left_forearm.dims.x,
                         upper_left_leg.dims.y + lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir a partir da mao esquerda
            glPushMatrix();
                /*
                glTranslatef(left_forearm.dims.x, left_forearm.dims.y + left_arm.dims.y, 0.0f);
                glRotatef(deg, 0.0, 0.0, 1.0);
                glTranslatef(-left_forearm.dims.x, -left_forearm.dims.y -left_arm.dims.y, 0.0f);
                */
                this->left_forearm.draw();
            glPopMatrix();
            glTranslatef(left_forearm.dims.x + chest.dims.x, 0.0f, 0.0f); // Deslocamento para imprimir a partir da mao direita
            this->right_forearm.draw();
        glPopMatrix();

        // Desenho do torso
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, upper_left_leg.dims.y + lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir o torso a partir do canto inferior esquerdo
            this->chest.draw();
        glPopMatrix();

        // Desenho da cabeca
        glColor3f(0.0f, 1.0f, 0.5f);
        glPushMatrix();
            glTranslatef((chest.dims.x - head.dims.x) / 2, lower_left_leg.dims.y + upper_left_leg.dims.y + chest.dims.y, 0.0f); // Deslocamento para imprimir a partir do pescoco pelo lado esquerdo
            this->head.draw();
        glPopMatrix();

    }

    char select(char part) {
        static std::string member = "none";
        switch(part) {
            case '2':
                std::cout << "Torso selected" << std::endl;
                chest.selected = true;
                member = "chest";
                break;
            case '0':
                std::cout << "Head selected" << std::endl;
                head.selected = true;
                member = "head";
                break;
            case '1':
                std::cout << "Left arm selected" << std::endl;
                member = "larm";
                break;
            case '3':
                std::cout << "Right arm selected" << std::endl;
                member = "rarm";
                // Select chest
                break;
            case '4':
                std::cout << "Left forearm selected" << std::endl;
                member = "lfarm";
                break;
            case '5':
                std::cout << "Right forearm selected" << std::endl;
                member = "rfarm";
                // Select chest
                break;
            case '6':
                std::cout << "Coxa esquerda selected" << std::endl;
                member = "ulleg";
                // Select coxa
                break;
            case '7':
                std::cout << "Coxa direita selected" << std::endl;
                member = "urleg";
                // Select coxa
                break;
            case '8':
                std::cout << "Panturrilha esquerda selected" << std::endl;
                member = "llleg";
                // Select panturrilha
                break;
            case '9':
                std::cout << "Panturrilha direita selected" << std::endl;
                member = "lrleg";
                // Select panturrilha
                break;
            case 'n':
                std::cout << "None selected" << std::endl;
                member = "none";
                break;
        }
    }

    void setMovement(char key) {

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
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            dm.select(key);
            break;
        case 'i':
        case 'j':
        case 'k':
        case 'l':
            dm.setMovement(key);
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
