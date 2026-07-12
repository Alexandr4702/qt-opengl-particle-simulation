#include "gl_game.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QWheelEvent>
#include <QtMath>

namespace {
constexpr float scene_depth = -20.0f;
constexpr float golden_angle = 2.399963f;

QVector3D spherical_offset(int index, int count, float min_radius, float max_radius)
{
    const float t = (float(index) + 0.5f) / float(qMax(1, count));
    const float z = 1.0f - 2.0f * t;
    const float xy = qSqrt(qMax(0.0f, 1.0f - z * z));
    const float angle = float(index) * golden_angle;
    const float radius = min_radius + (max_radius - min_radius) * qPow(t, 1.0f / 3.0f);

    return QVector3D(radius * xy * qCos(angle),
                     radius * xy * qSin(angle),
                     radius * z);
}

QVector3D tangential_velocity(const QVector3D& offset, float speed)
{
    QVector3D tangent = QVector3D::crossProduct(QVector3D(0.0f, 0.0f, 1.0f), offset);
    if (tangent.lengthSquared() < 0.0001f)
        tangent = QVector3D::crossProduct(QVector3D(0.0f, 1.0f, 0.0f), offset);

    return tangent.normalized() * speed;
}
}

GL_GAME::GL_GAME(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
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
    frame_timer.start();
    rebuild_scene();
}

void GL_GAME::rebuild_scene()
{
    delete world;
    world = new World(context());
    world->time_scale = simulation_time_scale;
    world->potential_type = potential_type;

    if (potential_type == World::PotentialType::Gravity) {
        constexpr float central_mass = 100.0f;
        Body* center = new Body(context());
        center->setPosition(QVector3D(0.0f, 0.0f, scene_depth));
        center->setScale(QVector3D(0.35f, 0.35f, 0.35f));
        center->setMass(central_mass);
        world->add_body(center);

        for (int index = 1; index < particle_count; ++index) {
            const QVector3D offset =
                spherical_offset(index - 1, particle_count - 1, 1.0f, 7.0f);
            const float radius = offset.length();
            const float orbital_speed =
                qSqrt(world->gravitational_constant * central_mass / radius);
            Body* body = new Body(context());
            body->setPosition(QVector3D(0.0f, 0.0f, scene_depth) + offset);
            body->setLinear_velocity(tangential_velocity(offset, orbital_speed));
            body->setScale(QVector3D(0.1f, 0.1f, 0.1f));
            body->setMass(0.02);
            world->add_body(body);
        }
    } else {
        const bool combined_potential =
            potential_type == World::PotentialType::GravityAndCoulomb;
        const bool charged_potential =
            potential_type == World::PotentialType::Coulomb || combined_potential;
        const bool orbiting_potential =
            combined_potential || potential_type == World::PotentialType::Harmonic
            || potential_type == World::PotentialType::Yukawa;
        for (int index = 0; index < particle_count; ++index) {
            const QVector3D offset =
                potential_type == World::PotentialType::LennardJones
                ? spherical_offset(index, particle_count, 0.7f, 4.7f)
                : spherical_offset(index, particle_count, 1.0f, 7.0f);
            Body* body = new Body(context());
            body->setPosition(QVector3D(0.0f, 0.0f, scene_depth) + offset);
            if (orbiting_potential)
                body->setLinear_velocity(tangential_velocity(offset, 0.35f));
            body->setScale(QVector3D(0.12f, 0.12f, 0.12f));
            body->setMass(1.0);
            if (charged_potential)
                body->setCharge(index % 2 == 0 ? 1.0 : -1.0);
            world->add_body(body);
        }
    }
    rebuild_requested = false;
}

void GL_GAME::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (rebuild_requested)
        rebuild_scene();

    camera.update(frame_timer.restart() / 1000.0f);
    world->set_camera(camera.view_matrix());
    world->draw(simulation_running);
    update();
}

bool GL_GAME::toggle_simulation()
{
    simulation_running = !simulation_running;
    return simulation_running;
}

void GL_GAME::reset_simulation()
{
    simulation_running = false;
    rebuild_requested = true;
    update();
}

void GL_GAME::set_time_scale(double scale)
{
    simulation_time_scale = float(qBound(0.1, scale, 10.0));
    if (world)
        world->time_scale = simulation_time_scale;
}

void GL_GAME::spawn_particles(int count)
{
    particle_count = qBound(2, count, 4096);
    if (particle_count % 2 != 0)
        ++particle_count;
    simulation_running = false;
    rebuild_requested = true;
    update();
}

void GL_GAME::set_potential(int potential_index)
{
    switch (potential_index) {
    case 1:
        potential_type = World::PotentialType::Coulomb;
        break;
    case 2:
        potential_type = World::PotentialType::GravityAndCoulomb;
        break;
    case 3:
        potential_type = World::PotentialType::Harmonic;
        break;
    case 4:
        potential_type = World::PotentialType::LennardJones;
        break;
    case 5:
        potential_type = World::PotentialType::Yukawa;
        break;
    default:
        potential_type = World::PotentialType::Gravity;
        break;
    }
    simulation_running = false;
    rebuild_requested = true;
    update();
}

void GL_GAME::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        last_mouse_position = event->position().toPoint();
}

void GL_GAME::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    const QPoint current_position = event->position().toPoint();
    const QPoint delta = current_position - last_mouse_position;
    last_mouse_position = current_position;
    camera.orbit(delta.x(), delta.y());
    update();
}

void GL_GAME::wheelEvent(QWheelEvent* event)
{
    camera.zoom(event->angleDelta().y() / 120.0f);
    update();
}

void GL_GAME::keyPressEvent(QKeyEvent* event)
{
    constexpr float movement = 0.5f;
    switch (event->key()) {
    case Qt::Key_W:
        camera.move(movement, 0.0f, 0.0f);
        break;
    case Qt::Key_S:
        camera.move(-movement, 0.0f, 0.0f);
        break;
    case Qt::Key_A:
        camera.move(0.0f, -movement, 0.0f);
        break;
    case Qt::Key_D:
        camera.move(0.0f, movement, 0.0f);
        break;
    case Qt::Key_Space:
        camera.move(0.0f, 0.0f, movement);
        break;
    case Qt::Key_Shift:
        camera.move(0.0f, 0.0f, -movement);
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
        return;
    }
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
