#include "body.h"
#include <QOpenGLFunctions>

Body::Body(QOpenGLContext* ctx_):ctx(ctx_)
{
}

Body::Body(const Body &other)
{
    position = other.position;
    orenation = other.orenation;
    scale = other.scale;

    linear_velocity = other.linear_velocity;
    angular_velocity = other.angular_velocity;

    angular_acceleration = other.angular_acceleration;
    linear_acclereation = other.linear_acclereation;
    density = other.density;
    Volume = other.Volume;
    mass = other.mass;
    ctx = other.ctx;
}

Body::Body(Body &&other) noexcept
{
    position = other.position;
    orenation = other.orenation;
    scale = other.scale;
    linear_velocity = other.linear_velocity;
    angular_velocity = other.angular_velocity;

    angular_acceleration=other.angular_acceleration;
    linear_acclereation=other.linear_acclereation;

    density=other.density;
    Volume=other.Volume;
    mass=other.mass;
    ctx=other.ctx;

}

Body &Body::operator=(const Body &other)
{
    if (this == &other)
        return *this;
    position=other.position;
    orenation=other.orenation;
    scale=other.scale;

    linear_velocity=other.linear_velocity;
    angular_velocity=other.angular_velocity;

    angular_acceleration=other.angular_acceleration;
    linear_acclereation=other.linear_acclereation;

    density=other.density;
    Volume=other.Volume;
    mass=other.mass;
    ctx=other.ctx;
    return *this;
}

Body &Body::operator=(Body &&other) noexcept
{
    if (this == &other)
        return *this;
    position=other.position;
    orenation=other.orenation;
    scale=other.scale;

    linear_velocity=other.linear_velocity;
    angular_velocity=other.angular_velocity;

    angular_acceleration=other.angular_acceleration;
    linear_acclereation=other.linear_acclereation;

    density=other.density;
    Volume=other.Volume;
    mass=other.mass;
    ctx=other.ctx;
    return *this;
}

QVector3D Body::getScale() const
{
    return scale;
}

void Body::setScale(const QVector3D &value)
{
    scale = value;
}


void Body::set_orenatation(const QQuaternion &oreantation)
{
    orenation=oreantation;
}

QVector3D Body::getPosition() const
{
    return position;
}

void Body::setPosition(const QVector3D &value)
{
    position = value;
}



void Body::init_geometry()
{
    static const QVector3D vertices[] = {
        {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1},
        {-1, -1,  1}, { 1,  1,  1}, {-1,  1,  1},
        { 1, -1, -1}, {-1, -1, -1}, {-1,  1, -1},
        { 1, -1, -1}, {-1,  1, -1}, { 1,  1, -1},
        {-1, -1, -1}, {-1, -1,  1}, {-1,  1,  1},
        {-1, -1, -1}, {-1,  1,  1}, {-1,  1, -1},
        { 1, -1,  1}, { 1, -1, -1}, { 1,  1, -1},
        { 1, -1,  1}, { 1,  1, -1}, { 1,  1,  1},
        {-1,  1,  1}, { 1,  1,  1}, { 1,  1, -1},
        {-1,  1,  1}, { 1,  1, -1}, {-1,  1, -1},
        {-1, -1, -1}, { 1, -1, -1}, { 1, -1,  1},
        {-1, -1, -1}, { 1, -1,  1}, {-1, -1,  1}
    };

    vertex_array.create();
    vertex_array.bind();

    vertex_buffer.create();
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices, sizeof(vertices));

    shader->enableAttributeArray("a_position");
    shader->setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(QVector3D));

    vertex_buffer.release();
    vertex_array.release();

}

void Body::draw()
{
    if (!shader || !projection || !camera || !vertex_array.isCreated())
        return;

    QMatrix4x4 model;
    model.translate(position);
    model.rotate(orenation);
    model.scale(scale);

    shader->bind();
    shader->setUniformValue("mvp_matrix", *projection * *camera * model);
    vertex_array.bind();
    ctx->functions()->glDrawArrays(GL_TRIANGLES, 0, 36);
    vertex_array.release();

}

void Body::draw_instances(int count, unsigned int position_buffer)
{
    if (!shader || !projection || !camera || !vertex_array.isCreated())
        return;

    QOpenGLFunctions_4_3_Core* functions =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(ctx);

    shader->bind();
    shader->setUniformValue("view_projection", *projection * *camera);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, position_buffer);
    vertex_array.bind();
    functions->glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
    vertex_array.release();
}

void Body::set_projection(QMatrix4x4 *projection_)
{
    projection = projection_;
}

void Body::set_cam(QMatrix4x4 *camera_)
{
    camera = camera_;
}

void Body::set_shader(QOpenGLShaderProgram *shader_)
{
    shader = shader_;
}

void Body::set_context(QOpenGLContext *ctx_)
{
    ctx=ctx_;
}


QVector3D Body::getLinear_acclereation() const
{
    return linear_acclereation;
}

void Body::setLinear_acclereation(const QVector3D &value)
{
    linear_acclereation = value;
}

QVector3D Body::getAngular_acceleration() const
{
    return angular_acceleration;
}

void Body::setAngular_acceleration(const QVector3D &value)
{
    angular_acceleration = value;
}

QVector3D Body::getAngular_velocity() const
{
    return angular_velocity;
}

void Body::setAngular_velocity(const QVector3D &value)
{
    angular_velocity = value;
}

QVector3D Body::getLinear_velocity() const
{
    return linear_velocity;
}

void Body::setLinear_velocity(const QVector3D &value)
{
    linear_velocity = value;
}

double Body::getMass() const
{
    return mass;
}

void Body::setMass(double value)
{
    mass = value;
}

double Body::getVolume() const
{
    return Volume;
}

void Body::setVolume(double value)
{
    Volume = value;
}

double Body::getDensity() const
{
    return density;
}

void Body::setDensity(double value)
{
    density = value;
}
