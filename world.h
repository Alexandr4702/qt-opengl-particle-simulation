#ifndef WORLD_H
#define WORLD_H

#include "body.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QVector>

class World
{
public:
    explicit World(QOpenGLContext* context);
    ~World();

    void add_body(Body* body);
    void draw();

    float gravitational_constant = 1.0f;
    float softening = 0.01f;
    float time_step = 0.001f;
    int substeps_per_frame = 8;
    QMatrix4x4 Projection;

private:
    void init_render_shader();
    void init_gpu_physics();
    void update_gpu();

    QMatrix4x4 camera;
    QVector<Body*> bodies;
    QOpenGLContext* context = nullptr;
    QOpenGLFunctions_4_3_Core* functions = nullptr;
    QOpenGLShaderProgram render_program;
    QOpenGLShaderProgram physics_program;
    unsigned int position_buffers[2] = {0, 0};
    unsigned int velocity_buffers[2] = {0, 0};
    int current_buffer = 0;
    bool gpu_initialized = false;
};

#endif // WORLD_H
