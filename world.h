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
    enum class PotentialType {
        Gravity = 0,
        Coulomb = 1,
        GravityAndCoulomb = 2,
        Harmonic = 3,
        LennardJones = 4,
        Yukawa = 5
    };

    explicit World(QOpenGLContext* context);
    ~World();

    void add_body(Body* body);
    void draw(bool advance_simulation);
    void set_camera(const QMatrix4x4& value);

    float gravitational_constant = 1.0f;
    float coulomb_constant = 10.0f;
    float harmonic_constant = 0.2f;
    float lennard_jones_epsilon = 0.02f;
    float lennard_jones_sigma = 0.35f;
    float yukawa_range = 2.5f;
    float softening = 0.01f;
    float time_step = 0.001f;
    float time_scale = 1.0f;
    int substeps_per_frame = 8;
    PotentialType potential_type = PotentialType::Gravity;
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
    unsigned int property_buffer = 0;
    int current_buffer = 0;
    bool gpu_initialized = false;
    float substep_accumulator = 0.0f;
};

#endif // WORLD_H
