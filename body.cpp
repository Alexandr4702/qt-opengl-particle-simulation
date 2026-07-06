#include "body.h"

#include <QtMath>

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
    constexpr int stack_count = 12;
    constexpr int sector_count = 18;
    QVector<QVector3D> vertices;
    vertices.reserve(stack_count * sector_count * 6);

    const auto point = [](float latitude, float longitude) {
        const float latitude_cosine = qCos(latitude);
        return QVector3D(latitude_cosine * qCos(longitude),
                         qSin(latitude),
                         latitude_cosine * qSin(longitude));
    };

    for (int stack = 0; stack < stack_count; ++stack) {
        const float latitude_0 = -float(M_PI_2) + float(M_PI) * stack / stack_count;
        const float latitude_1 = -float(M_PI_2) + float(M_PI) * (stack + 1) / stack_count;

        for (int sector = 0; sector < sector_count; ++sector) {
            const float longitude_0 = float(M_PI * 2.0) * sector / sector_count;
            const float longitude_1 = float(M_PI * 2.0) * (sector + 1) / sector_count;
            const QVector3D lower_left = point(latitude_0, longitude_0);
            const QVector3D lower_right = point(latitude_0, longitude_1);
            const QVector3D upper_left = point(latitude_1, longitude_0);
            const QVector3D upper_right = point(latitude_1, longitude_1);

            vertices << lower_left << lower_right << upper_right
                     << lower_left << upper_right << upper_left;
        }
    }

    vertex_count = vertices.size();

    vertex_array.create();
    vertex_array.bind();

    vertex_buffer.create();
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices.constData(),
                           vertices.size() * int(sizeof(QVector3D)));

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
    functions->glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_count, count);
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
