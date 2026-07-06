#include "body.h"

Body::Body(QOpenGLContext* context) : ctx(context)
{
}

void Body::setScale(const QVector3D &value)
{
    scale = value;
}

void Body::setPosition(const QVector3D &value)
{
    position = value;
}

void Body::setLinear_velocity(const QVector3D &value)
{
    linear_velocity = value;
}

void Body::setMass(double value)
{
    mass = value;
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

void Body::set_projection(QMatrix4x4 *value)
{
    projection = value;
}

void Body::set_cam(QMatrix4x4 *value)
{
    camera = value;
}

void Body::set_shader(QOpenGLShaderProgram *value)
{
    shader = value;
}
