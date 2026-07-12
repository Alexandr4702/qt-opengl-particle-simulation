#ifndef BODY_H
#define BODY_H
#include <QVector3D>

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLVertexArrayObject>

class Body
{
public:
    Body(QOpenGLContext* ctx_);
    virtual ~Body() = default;
    Body(const Body&) = delete;
    Body& operator=(const Body&) = delete;

    QVector3D position = QVector3D(0,0,0);
    QVector3D scale = QVector3D(1,1,1);
    QVector3D linear_velocity = QVector3D(0,0,0);
    double mass = 1;
    double charge = 0;

    void setMass(double value);
    void setCharge(double value);
    void setLinear_velocity(const QVector3D &value);
    void setPosition(const QVector3D &value);
    void setScale(const QVector3D &value);
    void init_geometry();
    void draw_instances(int count, unsigned int position_buffer);
    void set_projection(QMatrix4x4*);
    void set_cam(QMatrix4x4*);
    void set_shader(QOpenGLShaderProgram* shader_);
private:
    QOpenGLContext* ctx = nullptr;
    QOpenGLBuffer vertex_buffer;
    QOpenGLVertexArrayObject vertex_array;
    QOpenGLShaderProgram* shader = nullptr;
    QMatrix4x4* projection = nullptr;
    QMatrix4x4* camera = nullptr;
    int vertex_count = 0;
};

#endif // BODY_H
