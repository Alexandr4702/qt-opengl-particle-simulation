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
    for (int index = 0; index < body_count; ++index) {
        const float angle = float(index) * 0.37f;
        const float radius = 1.0f + 6.0f * float(index) / float(body_count);
        Body* body = new Body(context());
        body->setPosition(QVector3D(radius * qCos(angle),
                                    radius * qSin(angle), -20.0f));
        body->setScale(QVector3D(0.08f, 0.08f, 0.08f));
        body->setMass(0.2);
        world->add_body(body);
    }
}

void GL_GAME::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static int cnt=0;
//    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    world->draw();
    update();
    cnt++;
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
