#include "world.h"

World::World(QOpenGLContext* context) : ctx(context)
{
    functions = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(context);
    if (!functions)
        qFatal("OpenGL 4.3 is required for compute shader physics");
    functions->initializeOpenGLFunctions();
    init_pos_oren_shader();
    Projection.setToIdentity();
    Projection.perspective(90, 1.0, 1, 90);
}

World::~World()
{
    if (position_buffers[0] != 0) {
        functions->glDeleteBuffers(2, position_buffers);
        functions->glDeleteBuffers(2, velocity_buffers);
    }
    qDeleteAll(bodies);
}

void World::update(float dt_)
{
    K=0;
//    fprintf(stderr,"hello update \r\n");
    if(bodies.size()==0)return;

    int size=bodies.size();

    for(int i=0;i<size;i++)
    {
        Body* body=bodies[i];

        body->temporary_forces.clear();
        K+=body->mass*(body->linear_velocity*body->linear_velocity).length()/2.0;

        for (Body* body_test : bodies )
        {
            if(body==body_test)continue;

                QVector3D r=(body->position-body_test->position);
                if(r.length()==0)
                {continue;}
                QVector3D F=-body->mass*body_test->mass*r;
//                if((r.length()*r.length()*100-r.length()*r.length()*r.length())==0)continue;
                body->temporary_forces.push_back(F/r.length()/(r.length()*r.length()));
//                qDebug()<<F/r.length()<<r.length()<<r;
//                U+=
        }

        body->force_summ=QVector3D(0,0,0);

        for(QVector3D force :body->temporary_forces)
        {
            body->force_summ+=force;
        }

        for(QVector3D force : body->forces)
        {
            body->force_summ += force;
        }

        body->torque_summ = QVector3D(0,0,0);
        for(QVector3D torque : body->torques)
        {
            body->torque_summ += torque;
        }

         body->linear_acclereation=body->force_summ/body->mass;
         body->linear_velocity+=body->linear_acclereation*dt_;

        body->angular_velocity += body->angular_acceleration*dt_;

         float omega=body->angular_velocity.length();
//         QQuaternion M(cosf(omega*dt_/2.0f),sinf(omega*dt_/2.0f)*body->angular_velocity);
//         M.normalize();
         QQuaternion M(1,body->angular_velocity*dt_/2);

         body->orenation=M*body->orenation;
         body->position+=body->linear_velocity*dt;
    }

//    qDebug()<<K;
    time+=dt_;
}

void World::update()
{

}

void World::draw()
{
    cam.setToIdentity();
    if (bodies.isEmpty())
        return;

    if (!gpu_initialized)
        init_gpu_physics();

    update_gpu(0.001f);
    bodies.first()->draw_instances(bodies.size(), position_buffers[current_buffer]);
}

void World::draw(QMatrix4x4 &projection_matrix)
{
    Q_UNUSED(projection_matrix);

    for(Body* body:bodies)
    {
        body->draw();
    }
}

void World::init_pos_oren_shader()
{

    if (!shader_position_orentation_programm.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/VS.vsh")) {
        qCritical() << shader_position_orentation_programm.log();
        exit(-1);
    }
    if (!shader_position_orentation_programm.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/FS.fsh")) {
        qCritical() << shader_position_orentation_programm.log();
        exit(-1);
    }

    if (!shader_position_orentation_programm.link()) {
        qCritical() << shader_position_orentation_programm.log();
        exit(-1);
    }
    if (!shader_position_orentation_programm.bind()) {
        qCritical() << shader_position_orentation_programm.log();
        exit(-1);
    }

}

QMatrix4x4* World::getProjection()
{
    return &Projection;
}

void World::setProjection(const QMatrix4x4 &value)
{
    Projection = value;
}

QOpenGLShaderProgram* World::getShader_position_orentation_programm()
{
    return &shader_position_orentation_programm;
}

void World::add_body(Body * body)
{
    body->set_projection(&Projection);
    body->set_cam(&cam);
    body->set_shader(&shader_position_orentation_programm);
    bodies.push_back(body);
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

void World::update_gpu(float delta_time)
{
    const int next_buffer = 1 - current_buffer;

    physics_program.bind();
    physics_program.setUniformValue("body_count", unsigned(bodies.size()));
    physics_program.setUniformValue("delta_time", delta_time);
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
