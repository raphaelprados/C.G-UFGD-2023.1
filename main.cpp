
/*

    Universidade Federal da Grande Dourados
    Faculdade de Ciências Exatas e Tecnologia
    Disciplina: Introdução à Computação Gráfica / Laboratório de Computação Gráfica
    Discente: Raphael Alexsander Prado dos Santos
    Docente: Adailton José Alves da Cruz

*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Utils imports
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <cmath>
#include <map>

// Command prompt imports
#include <iostream>

#define POINTS 4

#define SCREEN_LENGTH 640
#define SCREEN_HEIGHT 480


struct Point {
    float x, y, z;
};

struct Camera {
    Point orgn,
          drct,
          vect;
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
    std::vector<transformation> rots_x;     // Vector that stores x axis rotations
    std::vector<transformation> rots_z;     // Vector that stores x axis rotations
    std::vector<transformation> trns;       // Vector that stores all translations
    std::vector<Cuboid*> joints;            // Reference to the other body members
    std::string type;                       // Shows the member's type
    Point color;                            // Has the color values for the cuboid
    Point cur_rotation;                     // Stores total current rotation values
    unsigned delay = 1000;                  // Animation delay
public:
    // I usually wouldn't use public variables, but time is short and using a getter would make the code harder to read/write
    bool selected = false;
    Point dims;             // Dimensions

    Cuboid(float height, float width, float length, std::string type) {
        selected = false;
        dims = {width, height, length};
        cur_rotation = {0.0f, 0.0f, 0.0f};
        this->type = type;
        std::cout << this->type << " initialized" << std::endl;
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
        // Setting body member color
        if(type == "head")                                  // head
            color = {1.0, 0.5, 0.0};
        else if(type.find("for") != std::string::npos)       // forearm
            color = {1.0, 0.5, 0.0};
        else if(type.find("arm") != std::string::npos)       // arm
            color = {1.0, 1.0, 0.0};
        else if(type.find("chest") != std::string::npos)       // chest
            color = {0.0, 0.5, 1.0};
        else if(type.find("up") != std::string::npos)       // upper_leg
            color = {1.0, 1.0, 0.5};
        else if(type.find("low") != std::string::npos)       // lower_leg
            color = {1.0, 0.5, 0.0};
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
        if(selected)
            glColor3f(0.0f, 1.0f, 1.0f);
        else
            glColor3f(color.x, color.y, color.z);
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

    // Pushes all the rotation and translations matrixes for this Cuboid
    void pushActMx() {
        glPushMatrix();
        if(type == "right_forearm" || type == "left_forearm" || type == "lower_right_leg" || type == "lower_left_leg") {
            glTranslatef(dims.x, dims.y + (selected ? 0.0f : joints[0]->dims.y), 0.0f);         // Returns to origin
            for(transformation tf : trns)                                   // Translations
                glTranslatef(tf.d.x, tf.d.y, tf.d.z);
            for(transformation tf : rots_x)                                 // X axis rotations
                glRotatef(tf.deg, tf.ax.x, tf.ax.y, tf.ax.z);
            for(transformation tf : rots_z)                                 // Z axis rotations
                glRotatef(tf.deg, tf.ax.x, tf.ax.y, tf.ax.z);
            // the unary operator controls if said member is being moved along with it's parent member
            glTranslatef(-dims.x, - dims.y - (selected ? 0.0f : joints[0]->dims.y), 0.0f);      // Sets position for rotation
        } else if(type == "right_arm" || type == "left_arm" || type == "upper_right_leg" || type == "upper_left_leg") {
            glTranslatef(dims.x, dims.y, 0.0f);                             // Returns to origin
            for(transformation tf : trns)                                   // Translations
                glTranslatef(tf.d.x, tf.d.y, tf.d.z);
            for(transformation tf : rots_x)                                 // X axis rotations
                glRotatef(tf.deg, tf.ax.x, tf.ax.y, tf.ax.z);
            for(transformation tf : rots_z)                                 // Z axis rotations
                glRotatef(tf.deg, tf.ax.x, tf.ax.y, tf.ax.z);
            glTranslatef(-dims.x, -dims.y, 0.0f);                           // Sets position for rotation
        }
    }

    // Pops all transformations for this Cuboid
    void popActMx() {
        usleep(delay);
        glPopMatrix();
    }

    bool addMovement(char key) {
        // Sets the rotation movement for the head
        const float mv = 1.0f;
        transformation tf;

        // head movements
        if(this->type == "head") {
            if(key == 'i' || key == 'k') {
                if(key == 'i') {                              // Moving up (eyes looking at the sky)
                    if(cur_rotation.x < 90.0f) {
                        tf = {'r', mv, 90.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.x += mv;
                    }
                } else {                                        // Moving down (eyes looking at the floor)
                    if(cur_rotation.x > -90.0f) {
                        tf = {'l', -mv, -90.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.x -= mv;
                    }
                }
                rots_x.push_back(tf);
                // Removes all unnecessary rotations
                if(rots_x.size() != 0 && (cur_rotation.x == 0.0f || cur_rotation.x == 360.0f))
                    rots_x.clear();
            }


        // Arms movements
        } else if(this->type.find("arm") != std::string::npos) {
            // Up leaning movement
            if(key == 'i') {
                if(cur_rotation.x < 360.0f) {
                    tf = {'r', mv,  360.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                    cur_rotation.x += mv;
                }
            // Left leaning movement
            } else if(key == 'j') {
                // For the left arm or forearm (increasing angle)
                if(this->type.find("lef") != std::string::npos) {
                    if(cur_rotation.z > -180.0f) {
                        tf = {'r', -mv, -180.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z -= mv;
                    }
                }
                // For the right arm or forearm (decreasing angle)
                else {
                    if(cur_rotation.z > 0.0f) {
                        tf = {'r', -mv, 180.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z -= mv;
                    }
                }
            // Down leaning movement
            } else if(key == 'k') {
                if(cur_rotation.x > -360.0f) {
                    tf = {'r', -mv, -360.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                    cur_rotation.x -= mv;
                }
            // Right leaning movement
            } else if(key == 'l') {
                // For the left arm or forearm (decreasing angle)
                if(this->type.find("lef") != std::string::npos) {
                    if(cur_rotation.z < 0.0f) {
                        tf = {'r', mv, -180.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z += mv;
                    }
                }
                // For the right arm or forearm (increasing angle)
                else {
                    if(cur_rotation.z < 180.0f) {
                        std::cout << "Wut" << std::endl;
                        tf = {'r', +mv, 180.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z += mv;
                    }
                }
            } else {
                return false; // Some function input error occurred
            }
            // Calls for movement on connected member
            if(this->type.find("for") == std::string::npos)   // is an arm, not a forearm
                joints[0]->addMovement(key);


        // Legs movements
        } else if(this->type.find("leg") != std::string::npos) {
            // Up leaning movement
            if(key == 'i') {
                if(cur_rotation.x < 45.0f) {
                    tf = {'r', mv, 45.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                    cur_rotation.x += mv;
                    std::cout << cur_rotation.x << std::endl;
                }
            // Left leaning movement
            } else if(key == 'j') {
                // For the left leg or lower leg (increasing angle)
                if(this->type.find("lef") != std::string::npos) {
                    std::cout << "Bazinga" << std::endl;
                    if(cur_rotation.z > -45.0f) {
                        tf = {'r', -mv, -45.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z -= mv;
                    }
                }
                // For the right leg or lower leg (decreasing angle)
                else
                    if(cur_rotation.z > 0.0f) {
                        std::cout << "Found" << std::endl;
                        tf = {'r', -mv, 90.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z -= mv;
                    }
            } else if(key == 'k') {
                if(cur_rotation.x > -45.0f) {
                    tf = {'r', -mv, -45.0f, {1.0, 0.0, 0.0}, {0.0f, 0.0f, 0.0f}};
                    cur_rotation.x -= mv;
                    std::cout << cur_rotation.x << std::endl;
                }
            // Right leaning movement
            } else if(key == 'l') {
                // For the left arm or forearm (decreasing angle)
                if(this->type.find("lef") != std::string::npos) {
                    if(cur_rotation.z < 0.0) {
                        tf = {'r', mv, -90.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z += mv;
                    }
                }
                // For the right arm or forearm (increasing angle)
                else {
                    if(cur_rotation.z < 45.0f) {
                        tf = {'r', mv, 45.0f, {0.0, 0.0, 1.0}, {0.0f, 0.0f, 0.0f}};
                        cur_rotation.z += mv;
                    }
                }
            } else {
                return false; // Some function input error occurred
            }
            if(key == 'i' || key == 'k')
                rots_x.push_back(tf);
            else
                rots_z.push_back(tf);
        } else {
            return false;
        }
        // Pushes selected movement
        if(key == 'i' || key == 'k')
            rots_x.push_back(tf);
        else
            rots_z.push_back(tf);
        // Calls for movement on connected member
        if(this->type.find("up") != std::string::npos) { // upper leg calls movement for lower leg
            joints[0]->addMovement(key);
        }
        // Removes all unnecessary rotations
        if(rots_x.size() != 0 && (cur_rotation.x == 0.0f || cur_rotation.x == 360.0f))
            rots_x.clear();
        if(rots_z.size() != 0 && (cur_rotation.z == 0.0f || cur_rotation.z == 360.0f))
            rots_z.clear();
    }

    void setToDefault() {
        rots_x.clear();
        rots_z.clear();
        trns.clear();

    }
};

class Joints : public Cuboid {
private:
    double radius;
    int slices, stacks;
public:
    Joints() {

    }

    Joints(double r, int sl, int st) {
        radius = r;
        slices = sl;
        stacks = st;
    }

    void drawJoint() {
        glutSolidSphere(radius, slices, stacks);
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
    Joints left_knee,
           right_knee,
           left_femur,
           right_femur,
           left_elbow,
           right_elbow,
           left_shoulder,
           right_shoulder;
    Point origem;
    std::string member;
public:
    Dummy() {
        origem = {0.0f, 0.0f, 0.0f};
        // Builds a dummy with the following dimensions:
        // Cuboid(float height, float width, float length, std::string type)
        std::cout << "Initializing..." << std::endl;
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
        this->left_knee = Joints(2.5f, 50, 50);
        this->right_knee = Joints(2.5f, 50, 50);
        this->left_femur = Joints(2.5f, 50, 50);
        this->right_femur = Joints(2.5f, 50, 50);
        this->left_elbow = Joints(2.5f, 50, 50);
        this->right_elbow = Joints(2.5f, 50, 50);
        this->left_shoulder = Joints(2.5f, 50, 50);
        this->right_shoulder = Joints(2.5f, 50, 50);

        head.link(&chest);
        left_forearm.link(&left_arm);
        right_forearm.link(&right_arm);
        lower_left_leg.link(&upper_left_leg);
        lower_right_leg.link(&upper_right_leg);

        right_arm.link(&chest, &right_forearm);
        left_arm.link(&chest, &left_forearm);
        upper_left_leg.link(&chest, &lower_left_leg);
        upper_right_leg.link(&chest, &lower_right_leg);

        chest.link(&head, &left_arm, &right_arm, &upper_left_leg, &upper_right_leg);
    }

    void walk() {
        // First phase, 90 degrees front for right leg
        for(int i = 0; i < 90; i++) {
            upper_right_leg.addMovement('k');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }
        // Second phase, -45 degrees for both legs
        for(int i = 0; i < 22; i++) {
            upper_right_leg.addMovement('i');
            upper_left_leg.addMovement('i');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }


        // Third phase, -45 degrees for right leg and +135 for right
        for(int i = 0; i < 112; i++) {
            if(i < 23)
                upper_right_leg.addMovement('i');
            upper_left_leg.addMovement('k');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }

        // Fourth phase, -45 degrees for both legs
        for(int i = 0; i < 22; i++) {
            upper_right_leg.addMovement('i');
            upper_left_leg.addMovement('i');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }
        // Fifth phase, -45 degrees for right leg and 135 for left leg
        for(int i = 0; i < 112; i++) {
            if(i < 22)
                upper_left_leg.addMovement('i');
            upper_right_leg.addMovement('k');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }

        for(int i = 0; i < 45; i++) {
            upper_right_leg.addMovement('i');
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            glutPostRedisplay();
            //usleep(1000);
        }
    }

    void turn() {

    }

    void jump() {

    }

    void info() {

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

        float deg = -45.0f;

        // Desenho da cabeca
        glColor3f(0.0f, 1.0f, 0.5f);
        glPushMatrix();
            glTranslatef((chest.dims.x - head.dims.x) / 2, lower_left_leg.dims.y + upper_left_leg.dims.y + chest.dims.y, 0.0f); // Deslocamento para imprimir a partir do pescoco pelo lado esquerdo
            this->head.draw();
        glPopMatrix();

        // Desenho do torso
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.0f, upper_left_leg.dims.y + lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir o torso a partir do canto inferior esquerdo
            this->chest.draw();
        glPopMatrix();

        // Desenho das coxas
        glPushMatrix();
            glTranslatef(0.0f, lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir coxa esquerda
            this->upper_left_leg.draw();
            glTranslatef(upper_left_leg.dims.x + (chest.dims.x - 2*upper_left_leg.dims.x), 0.0f, 0.0f); // Deslocamento para imprimir coxa direita
            this->upper_right_leg .draw();
        glPopMatrix();

        // Desenho das pernas
        this->lower_left_leg.draw();
        glPushMatrix();
            glTranslatef(lower_left_leg.dims.x + (chest.dims.x - 2*lower_left_leg.dims.x),
                         0.0f, 0.0f); // Deslocamento para imprimir perna direita
            this->lower_right_leg.draw();
        glPopMatrix();

        // Desenho do braço
        glPushMatrix();
            glTranslatef(-left_forearm.dims.x, lower_left_leg.dims.y + upper_left_leg.dims.y + left_forearm.dims.y, 0.0f); // Deslocamento para imprimir a partir do cotovelo esquerdo
            this->left_arm.draw();
            glTranslatef(left_forearm.dims.x + chest.dims.x, 0.0f, 0.0f); // Deslocamento para imprimir a partir do cotovelo direito
            this->right_arm.draw();
        glPopMatrix();

        // Desenho do antebraco
        glPushMatrix();
            glTranslatef(-1*left_forearm.dims.x,
                         upper_left_leg.dims.y + lower_left_leg.dims.y, 0.0f); // Deslocamento para imprimir a partir da mao esquerda
            this->left_forearm.draw();
            glTranslatef(left_forearm.dims.x + chest.dims.x, 0.0f, 0.0f); // Deslocamento para imprimir a partir da mao direita
            this->right_forearm.draw();
        glPopMatrix();

        // Prints informations on the screen
        info();

        glutSwapBuffers();
    }

    void clearSelect() {
        head.selected = false;
        chest.selected = false;
        left_arm.selected = false;
        right_arm.selected = false;
        left_forearm.selected = false;
        right_forearm.selected = false;
        upper_left_leg.selected = false;
        lower_left_leg.selected = false;
        lower_right_leg.selected = false;
        upper_right_leg.selected = false;
    }

    void newSelect(char part) {
        std::cout << part << std::endl;
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
                left_arm.selected = true;
                member = "larm";
                break;
            case '3':
                std::cout << "Right arm selected" << std::endl;
                right_arm.selected = true;
                member = "rarm";
                // Select chest
                break;
            case '4':
                std::cout << "Left forearm selected" << std::endl;
                left_forearm.selected = true;
                member = "lfarm";
                break;
            case '5':
                std::cout << "Right forearm selected" << std::endl;
                right_forearm.selected = true;
                member = "rfarm";
                // Select chest
                break;
            case '6':
                std::cout << "Coxa esquerda selected" << std::endl;
                upper_left_leg.selected = true;
                member = "ulleg";
                // Select coxa
                break;
            case '7':
                std::cout << "Coxa direita selected" << std::endl;
                upper_right_leg.selected = true;
                member = "urleg";
                // Select coxa
                break;
            case '8':
                std::cout << "Panturrilha esquerda selected" << std::endl;
                lower_left_leg.selected = true;
                member = "llleg";
                // Select panturrilha
                break;
            case '9':
                std::cout << "Panturrilha direita selected" << std::endl;
                lower_right_leg.selected = true;
                member = "lrleg";
                // Select panturrilha
                break;
            case 'n':
                std::cout << "None selected" << std::endl;
                member = "none";
                break;
        }
    }

    char select(char part) {
        static std::string member = "none";

        clearSelect();
        member = "none";
        newSelect(part);
    }

    void setMovement(char key) {
        std::cout << member << std::endl;
        if (member == "chest")
            chest.addMovement(key);
        else if (member == "head")
            head.addMovement(key);
        else if (member == "larm")
            left_arm.addMovement(key);
        else if (member == "rarm")
            right_arm.addMovement(key);
        else if (member == "lfarm")
            left_forearm.addMovement(key);
        else if (member == "rfarm")
            right_forearm.addMovement(key);
        else if (member == "ulleg")
            upper_left_leg.addMovement(key);
        else if (member == "urleg")
            upper_right_leg.addMovement(key);
        else if (member == "llleg")
            lower_left_leg.addMovement(key);
        else if (member == "lrleg")
            lower_right_leg.addMovement(key);
        else
            member = "none";
    }

    void clearMoves() {
        this->head.setToDefault();
        this->chest.setToDefault();
        this->left_arm.setToDefault();
        this->left_forearm.setToDefault();
        this->right_arm.setToDefault();
        this->right_forearm.setToDefault();
        this->upper_left_leg.setToDefault();
        this->lower_left_leg.setToDefault();
        this->upper_right_leg.setToDefault();
        this->lower_right_leg.setToDefault();
        clearSelect();
    }
};


// Global variable declaration
Dummy dm = Dummy();
Camera cam[] = {   // Vector of camera positions
                    {{-100.0, 20.0,   0.0}, {0.0, 30.0, 0.0}, {0.0, 1.0, 0.0}},
                    {{- 25.0, 20.0, 100.0}, {0.0, 30.0, 0.0}, {0.0, 1.0, 0.0}},
                    {{   0.0, 20.0, 100.0}, {0.0, 30.0, 0.0}, {0.0, 1.0, 0.0}},
                    {{  25.0, 20.0, 100.0}, {0.0, 30.0, 0.0}, {0.0, 1.0, 0.0}},
                    {{ 100.0, 20.0,   0.0}, {0.0, 30.0, 0.0}, {0.0, 1.0, 0.0}}
                };
short unsigned ci = 2;  // Camera index


static void resize(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble) width/height, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cam[ci].orgn.x, cam[ci].orgn.y, cam[ci].orgn.z,
              cam[ci].drct.x, cam[ci].drct.y, cam[ci].drct.z,
              cam[ci].vect.x, cam[ci].vect.y, cam[ci].vect.z);
    dm.draw();
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
            dm.walk();
            // Set walk animation
            break;
        case '0':
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
        case 'n':
            dm.clearSelect();
            break;
        case 'c':
            ci = (ci + 1) % (sizeof(cam)/sizeof(Camera));
            break;
        case 'r':
            dm.clearMoves();
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

    glutCreateWindow("Trabalho Robô - Computação Gráfica");

    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);       // Allows for keeping a key pressed and receive multiple inputs
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glutMainLoop();

    return EXIT_SUCCESS;
}

/*

Notas e Matrizes

gluPerspective(45.0, (GLdouble) width/height, 1.0, 100.0);


*/
