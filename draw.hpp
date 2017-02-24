#ifndef DRAW_HPP
#define DRAW_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glu.h>
#include <glm/vec3.hpp>
using glm::vec3;

namespace Draw {

    void axes();
    void unitCube();
    void unitSphere();

    // Definitions below

    inline void axes() {
        glBegin(GL_TRIANGLES);
        glColor3f(1,0,0);
        glVertex3f(0,-0.2,0);
        glVertex3f(1, 0.0,0);
        glVertex3f(0,+0.2,0);
        glColor3f(0,1,0);
        glVertex3f(0,0,-0.2);
        glVertex3f(0,1, 0.0);
        glVertex3f(0,0,+0.2);
        glColor3f(0,0,1);
        glVertex3f(-0.2,0,0);
        glVertex3f( 0.0,0,1);
        glVertex3f(+0.2,0,0);
        glEnd();
    }

    inline void unitCube() {
        vec3 verts[8] = {
            vec3(-0.5,-0.5,-0.5),
            vec3(0.5,-0.5,-0.5),
            vec3(-0.5,0.5,-0.5),
            vec3(0.5,0.5,-0.5),
            vec3(-0.5,-0.5,0.5),
            vec3(0.5,-0.5,0.5),
            vec3(-0.5,0.5,0.5),
            vec3(0.5,0.5,0.5)
        };
        int faces[6][4] = {
            {0,2,3,1},
            {0,4,6,2},
            {0,1,5,4},
            {4,5,7,6},
            {1,3,7,5},
            {2,6,7,3}
        };
        vec3 normals[6] = {
            vec3(0,0,-1),
            vec3(-1,0,0),
            vec3(0,-1,0),
            vec3(0,0,+1),
            vec3(+1,0,0),
            vec3(0,+1,0)
        };
        glBegin(GL_QUADS);
        for (int f = 0; f < 6; f++) {
            vec3 n = normals[f];
            glNormal3f(n.x,n.y,n.z);
            for (int i = 0; i < 4; i++) {
                vec3 v = verts[faces[f][i]];
                glVertex3f(v.x,v.y,v.z);
            }
        }
        glEnd();
    }

    inline void unitSphere() {
        static GLUquadric* quadric = gluNewQuadric();
        gluSphere(quadric, 1, 40, 40);
    }

}

#endif
