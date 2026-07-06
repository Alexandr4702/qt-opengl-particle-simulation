#include "world.h"

#include <QOpenGLVersionFunctionsFactory>

World::World(QOpenGLContext* context_) : context(context_)
{
    functions = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(context);
    if (!functions)
        qFatal("OpenGL 4.3 is required for compute shader physics");

    functions->initializeOpenGLFunctions();
    init_render_shader();
    Projection.setToIdentity();
    Projection.perspective(90.0f, 1.0f, 1.0f, 90.0f);
}

World::~World()
{
    if (position_buffers[0] != 0) {
        functions->glDeleteBuffers(2, position_buffers);
        functions->glDeleteBuffers(2, velocity_buffers);
    }
    qDeleteAll(bodies);
}

void World::draw()
{
    camera.setToIdentity();
    if (bodies.isEmpty())
        return;

    if (!gpu_initialized)
        init_gpu_physics();

    for (int step = 0; step < substeps_per_frame; ++step)
        update_gpu();
    bodies.first()->draw_instances(bodies.size(), position_buffers[current_buffer]);
}

void World::init_render_shader()
{
    if (!render_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/VS.vsh") ||
        !render_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/FS.fsh") ||
        !render_program.link() || !render_program.bind()) {
        qFatal("Failed to initialize render shaders: %s",
               qPrintable(render_program.log()));
    }
}

void World::init_gpu_physics()
{
    if (!physics_program.addShaderFromSourceFile(QOpenGLShader::Compute, ":/Physics.comp") ||
        !physics_program.link()) {
        qFatal("Failed to initialize physics compute shader: %s",
               qPrintable(physics_program.log()));
    }

    QVector<QVector4D> positions;
    QVector<QVector4D> velocities;
    positions.reserve(bodies.size());
    velocities.reserve(bodies.size());

    for (Body* body : bodies) {
        positions.append(QVector4D(body->position, body->scale.x()));
        velocities.append(QVector4D(body->linear_velocity, float(body->mass)));
    }

    functions->glGenBuffers(2, position_buffers);
    functions->glGenBuffers(2, velocity_buffers);

    for (int index = 0; index < 2; ++index) {
        functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, position_buffers[index]);
        functions->glBufferData(GL_SHADER_STORAGE_BUFFER,
                                positions.size() * sizeof(QVector4D),
                                positions.constData(), GL_DYNAMIC_COPY);
        functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocity_buffers[index]);
        functions->glBufferData(GL_SHADER_STORAGE_BUFFER,
                                velocities.size() * sizeof(QVector4D),
                                velocities.constData(), GL_DYNAMIC_COPY);
    }

    bodies.first()->init_geometry();
    gpu_initialized = true;
}

void World::update_gpu()
{
    const int next_buffer = 1 - current_buffer;

    physics_program.bind();
    functions->glUniform1ui(physics_program.uniformLocation("body_count"),
                            unsigned(bodies.size()));
    functions->glUniform1f(physics_program.uniformLocation("delta_time"), time_step);
    functions->glUniform1f(physics_program.uniformLocation("gravitational_constant"),
                           gravitational_constant);
    functions->glUniform1f(physics_program.uniformLocation("softening"), softening);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
                                position_buffers[current_buffer]);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                                velocity_buffers[current_buffer]);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
                                position_buffers[next_buffer]);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
                                velocity_buffers[next_buffer]);
    functions->glDispatchCompute((unsigned(bodies.size()) + 127u) / 128u, 1, 1);
    functions->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    current_buffer = next_buffer;
}

void World::add_body(Body* body)
{
    body->set_projection(&Projection);
    body->set_cam(&camera);
    body->set_shader(&render_program);
    bodies.push_back(body);
}
