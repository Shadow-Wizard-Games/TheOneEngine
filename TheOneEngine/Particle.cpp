#include "Particle.h"
#include <GL/glew.h>

void Particle::SetDuration(float duration)
{
    this->duration = duration;
    lifetime = 0;

    lifetimeOverOne = 0;
    dtOverOne = 1 / duration;
}

void Particle::ResetAttributes()
{
    initialColor = glm::vec4(1, 1, 1, 1);
    color = initialColor;

    position = vec3(0, 0, 0);

    direction = vec3(0, 0, 0);
    speed = 0;

    rotation = vec4(1, 0, 0, 0);

    initialScale = vec3(1, 1, 1);
    scale = initialScale;
}

void Particle::Update(double dt)
{
    lifetime += dt;

    lifetimeOverOne += dtOverOne * dt;

    position += direction * (speed * dt);
}