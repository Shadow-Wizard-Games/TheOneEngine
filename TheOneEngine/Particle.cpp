#include "Particle.h"
#include <GL/glew.h>

void Particle::Update(double dt)
{
    lifetime += dt;

    color += deltaColor * dt;

    position += speed * dt;
}

void Particle::Render()
{
    glBegin(GL_TRIANGLES);
    glColor3ub(color.r, color.g, color.b);

    glVertex3d(position.x - 1, position.y + 1, position.z);
    glVertex3d(position.x - 1, position.y - 1, position.z);
    glVertex3d(position.x + 1, position.y + 1, position.z);

    glVertex3d(position.x - 1, position.y - 1, position.z);
    glVertex3d(position.x + 1, position.y - 1, position.z);
    glVertex3d(position.x + 1, position.y + 1, position.z);

    glEnd();
}
