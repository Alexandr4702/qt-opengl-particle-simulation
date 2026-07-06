#include "gl_game.h"
#include <QOpenGLShaderProgram>
#include <QtMath>

GL_GAME::GL_GAME(QWidget *parent) : QOpenGLWidget(parent)
{

}

GL_GAME::~GL_GAME()
{
    if (!world)
        return;

    makeCurrent();
    delete world;
    doneCurrent();
}

void GL_GAME::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    world = new World(context());

    constexpr int body_count = 512;
    constexpr float central_mass = 100.0f;

    Body* center = new Body(context());
    center->setPosition(QVector3D(0.0f, 0.0f, -20.0f));
    center->setScale(QVector3D(0.35f, 0.35f, 0.35f));
    center->setMass(central_mass);
    world->add_body(center);

    for (int index = 1; index < body_count; ++index) {
        const float angle = float(index) * 0.37f;
        const float radius = 1.0f + 6.0f * float(index) / float(body_count);
        const float orbital_speed =
            qSqrt(world->gravitational_constant * central_mass / radius);
        Body* body = new Body(context());
        body->setPosition(QVector3D(radius * qCos(angle),
                                    radius * qSin(angle), -20.0f));
        body->setLinear_velocity(QVector3D(-orbital_speed * qSin(angle),
                                            orbital_speed * qCos(angle), 0.0f));
        body->setScale(QVector3D(0.1f, 0.1f, 0.1f));
        body->setMass(0.02);
        world->add_body(body);
    }
}

void GL_GAME::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    world->draw();
    update();
}

void GL_GAME::resizeGL(int width, int height)
{
    glViewport(0, 0, GLint(width), GLint(height));
    world->Projection.setToIdentity();

    double aspect = double(width) / double(height ? height : 1);
    const double zNear = 1.0, zFar = 100000.0, fov = 90.0;
    world->Projection.setToIdentity();
    world->Projection.perspective(fov, aspect, zNear, zFar);
}
