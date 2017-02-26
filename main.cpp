#include "engine.hpp"
#include "draw.hpp"
#include <glm/glm.hpp>
#include <ctime>

using namespace std;
using glm::vec2;
using glm::vec3;
using glm::vec4;

class Car {
public:
    vec3 position;
    vec3 velocity;
    float rotateRate;
    float collisionRadius;
    float thrustFactor, dragFactor, rotateFactor;
    void draw() {
        glColor3f(0.8,0.2,0.2);
        // Replace the following with a more car-like geometry
        glPushMatrix();
        glTranslatef(position.x,position.y,position.z);
        glRotatef(rotateRate, 0, 1, 0);
        glScalef(3, 2, 4);
        Draw::unitCube();
        // glVertex3f(position.x,position.y,position.z);
        glPopMatrix();
    }
};

class Ball {
 public:
   vec3 position;
   vec3 velocity;
   vec3 gravity;
   float collisionRadius;
   float dragFactor;
   void draw() {
     glColor3f(1,1,1);
     glPushMatrix();
     glTranslatef(position.x, position.y,position.z);
     Draw::unitSphere();
     glPopMatrix();

     Draw::circleXZShadow(2, position);

   }
};

class CarSoccer: public Engine {
public:
    SDL_Window *window;
    Car car;
    Ball ball;
    CarSoccer() {
        srand((unsigned)time(0));
        double sd = (rand() % 101) / 100.0 * M_PI;
        window = createWindow("Car Soccer", 1280, 720);
        car.collisionRadius = 2.5;
        car.position = vec3(0, 1, 45); // center of car is 1 m above ground
        car.velocity = vec3(0, 0, 0);
        car.rotateRate = 0.0;
        car.thrustFactor = 250;
        car.rotateFactor = 210;
        car.dragFactor = 5;

        ball.position = vec3(0, 2, 0);
        ball.velocity = vec3(25 * cos(sd), 50, 25 * sin(sd));
        ball.gravity = vec3(0, -200, 0);
        ball.collisionRadius = 2;
        ball.dragFactor = 0.2;
    }

    ~CarSoccer() {
        SDL_DestroyWindow(window);
    }

    void run() {
        float fps = 60, timeStep = 1/fps;
        while (!shouldQuit()) {
            Engine::handleInput();
            simulate(timeStep);
            drawGraphics();
            Engine::waitForNextFrame(timeStep);
        }
    }

    void simulate(float timeStep) {
        // consider whether the ball reaches the goal
        if ((ball.position.z <= -50 + ball.collisionRadius ||
            ball.position.z >= 50 - ball.collisionRadius) && (
            ball.position.x >= -10 + ball.collisionRadius &&
            ball.position.x <= 10 - ball.collisionRadius &&
            ball.position.y <= 10 - ball.collisionRadius) ) {
          double sd = (rand() % 101) / 100.0 * M_PI;
          car.position = vec3(0, 1, 45); // center of car is 1 m above ground
          car.velocity = vec3(0, 0, 0);
          car.rotateRate = 0.0;

          ball.position = vec3(0, 2, 0);
          ball.velocity = vec3(25 * cos(sd), 50, 25 * sin(sd));
        }
        // An oversimplified dynamics model for the car
        vec2 dir = getControlDirection();
        if (dir.y == 0) {
          vec3 drag = car.dragFactor*car.velocity;
          car.velocity -= drag * timeStep;
          car.position += car.velocity*timeStep;
          if (car.position.x > 40 - car.collisionRadius ||
            car.position.x < -40 + car.collisionRadius ||
            car.position.z > 50 - car.collisionRadius ||
            car.position.z < -50 + car.collisionRadius ) {
              // hit the wall. stop and recover the position
              car.position -= car.velocity*timeStep;
              car.velocity = vec3(0,0,0);
          }
        } else {
          if (glm::length(dir) > 0)
          dir = glm::normalize(dir);
          car.rotateRate += -dir.x * car.rotateFactor * timeStep;
          vec3 thrust = car.thrustFactor *
          vec3(-dir.y * sin(car.rotateRate * M_PI / 180),
          0,
          -dir.y * cos(car.rotateRate * M_PI / 180));
          vec3 drag = car.dragFactor*car.velocity;
          car.velocity += (thrust - drag)*timeStep;
          car.position += car.velocity*timeStep;
          if (car.position.x > 40 - car.collisionRadius ||
            car.position.x < -40 + car.collisionRadius ||
            car.position.z > 50 - car.collisionRadius ||
            car.position.z < -50 + car.collisionRadius ) {
              // hit the wall, stop and recover the position and rotate angle
              car.position -= car.velocity*timeStep;
              car.velocity = vec3(0,0,0);
              car.rotateRate -= -dir.x * car.rotateFactor * timeStep;
          }
        }
        // Handle ball/wall, car/wall, and ball/car collisions here

        // ground
        if (ball.position.y < 0 + ball.collisionRadius) {
          // hit ground
          ball.velocity.y = fabs(ball.velocity.y);
          ball.position.y = 0 + ball.collisionRadius;
        }
        if (ball.position.y > 35 - ball.collisionRadius) {
          // hit the upper wall
          ball.velocity.y = -fabs(ball.velocity.y);
          ball.position.y = 35 - ball.collisionRadius;
        }
        if (ball.position.x < -40 + ball.collisionRadius) {
          // hit the right wall
          ball.velocity.x = fabs(ball.velocity.x);
          ball.position.x = -40 + ball.collisionRadius;
        }
        if (ball.position.x > 40 - ball.collisionRadius) {
          // hit the left wall
          ball.velocity.x = -fabs(ball.velocity.x);
          ball.position.x = 40 - ball.collisionRadius;
        }
        if (ball.position.z < -50 + ball.collisionRadius) {
          ball.velocity.z = fabs(ball.velocity.z);
          ball.position.z = -50 + ball.collisionRadius;
        }
        if (ball.position.z > 50 - ball.collisionRadius) {
          ball.velocity.z = -fabs(ball.velocity.z);
          ball.position.z = 50 - ball.collisionRadius;
        }

        // calculate out the position after this dt
        vec3 drag = ball.dragFactor * ball.velocity;
        ball.velocity += (ball.gravity - drag) * timeStep;

        // car and ball
        vec3 dis_ball_car = ball.position - car.position;
        if (glm::length(dis_ball_car) <=
            ball.collisionRadius + car.collisionRadius) {
          ball.position = car.position +
                          (ball.collisionRadius + car.collisionRadius) *
                          glm::normalize(dis_ball_car);
          vec3 rel_v = ball.velocity - car.velocity;
          ball.velocity += glm::length(rel_v) * glm::normalize(dis_ball_car);
        }

        ball.position += ball.velocity * timeStep;
    }

    vec2 getControlDirection() {
        vec2 dir(0,0);
        if (Engine::isKeyDown(SDL_SCANCODE_LEFT))
            dir.x--;
        if (Engine::isKeyDown(SDL_SCANCODE_RIGHT))
            dir.x++;
        if (Engine::isKeyDown(SDL_SCANCODE_UP))
            dir.y++;
        if (Engine::isKeyDown(SDL_SCANCODE_DOWN))
            dir.y--;
        return dir;
    }

    void addLight(GLenum light, vec4 position, vec3 color) {
        glEnable(light);
        glLightfv(light, GL_POSITION, &position[0]);
        glLightfv(light, GL_DIFFUSE, &color[0]);
    }

    void drawGraphics() {
        // Allow lines to show up on top of filled polygons
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);
        // Set up the camera in a good position to see the entire field
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 16/9., 1,1000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,60,70, 0,0,10, 0,1,0);
        // Clear window
        glClearColor(0.2,0.2,0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Add some lights
        addLight(GL_LIGHT0, vec4(+1,0.5,+1,0), vec3(0.5,0.5,0.5));
        addLight(GL_LIGHT1, vec4(+1,0.5,-1,0), vec3(0.5,0.5,0.5));
        addLight(GL_LIGHT2, vec4(-1,0.5,-1,0), vec3(0.5,0.5,0.5));
        addLight(GL_LIGHT3, vec4(-1,0.5,+1,0), vec3(0.5,0.5,0.5));
        glEnable(GL_LIGHTING);
        // Draw the ground as a quadrilateral
        glColor3f(0.3,0.6,0.2);
        glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-50,0,-60);
        glVertex3f(+50,0,-60);
        glVertex3f(+50,0,+60);
        glVertex3f(-50,0,+60);
        glEnd();
        // Draw the car
        car.draw();

        glDisable(GL_LIGHTING);
        // Draw the field borders, the pitch markings, and the goals here
        glColor3f(1,1,1);
        glBegin(GL_LINE_STRIP);
        glVertex3f(+40,0,+50);
        glVertex3f(-40,0,+50);
        glVertex3f(-40,0,-50);
        glVertex3f(+40,0,-50);
        glVertex3f(+40,0,+50);

        glVertex3f(+40,+35,+50);
        glVertex3f(-40,+35,+50);
        glVertex3f(-40,+35,-50);
        glVertex3f(+40,+35,-50);
        glVertex3f(+40,+35,+50);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(-40,0,+50);
        glVertex3f(-40,+35,+50);

        glVertex3f(-40,0,-50);
        glVertex3f(-40,+35,-50);

        glVertex3f(+40,0,-50);
        glVertex3f(+40,+35,-50);

        glVertex3f(+40,0,0);
        glVertex3f(-40,0,0);

        glVertex3f(+20,0,+50);
        glVertex3f(+20,0,+35);

        glVertex3f(+20,0,+35);
        glVertex3f(-20,0,+35);

        glVertex3f(-20,0,+35);
        glVertex3f(-20,0,+50);

        glVertex3f(+20,0,-50);
        glVertex3f(+20,0,-35);

        glVertex3f(+20,0,-35);
        glVertex3f(-20,0,-35);

        glVertex3f(-20,0,-35);
        glVertex3f(-20,0,-50);
        glEnd();

        Draw::circleXZ(10);

        glColor3f(0,0.5,1);
        for (int x = -10; x < 10; x++) {
          for (int y = 0; y < 10; y++) {
            glBegin(GL_LINE_LOOP);
            glVertex3f(x, y, -50);
            glVertex3f(x+1, y, -50);
            glVertex3f(x+1, y+1, -50);
            glVertex3f(x, y+1, -50);
            glEnd();
          }
        }

        glColor3f(1,0.5,0);
        for (int x = -10; x < 10; x++) {
          for (int y = 0; y < 10; y++) {
            glBegin(GL_LINE_LOOP);
            glVertex3f(x, y, 50);
            glVertex3f(x+1, y, 50);
            glVertex3f(x+1, y+1, 50);
            glVertex3f(x, y+1, 50);
            glEnd();
          }
        }

        glEnable(GL_LIGHTING);

        // Draw the ball
        ball.draw();
        SDL_GL_SwapWindow(window);
    }

    void onKeyDown(SDL_KeyboardEvent &e) {

        // Check whether the space bar was pressed, and if so, reset the ball
        // to the center of the pitch and give it a "kick-off" velocity.
        // We found that a nice initial velocity is (25 cos(a), 10, 25 sin(a))
        // where a is a random number between 0 and pi.
        if (e.keysym.scancode == SDL_SCANCODE_SPACE) {
          double sd = (rand() % 101) / 100.0 * M_PI;
          car.position = vec3(0, 1, 45); // center of car is 1 m above ground
          car.velocity = vec3(0, 0, 0);
          car.rotateRate = 0.0;

          ball.position = vec3(0, 2, 0);
          ball.velocity = vec3(25 * cos(sd), 50, 25 * sin(sd));
        }

    }
};

int main(int argc, char *argv[]) {
    CarSoccer game;
    game.run();
    return 0;
}
