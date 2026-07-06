#ifndef BODY_H
#define BODY_H
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix3x3>

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
    Body(const Body& other);
    Body(Body&& other)noexcept;
    Body& operator = (const Body & other);
    Body& operator = (Body&& other)noexcept;

    QVector3D position = QVector3D(0,0,0);
    QQuaternion orenation = QQuaternion(1,0,0,0);
    QVector3D scale = QVector3D(1,1,1);

    QVector3D linear_velocity = QVector3D(0,0,0);
    QVector3D angular_velocity = QVector3D(0,0,0);

    QVector3D angular_acceleration = QVector3D(0,0,0);
    QVector3D linear_acclereation = QVector3D(0,0,0);

    QVector <QVector3D> temporary_forces;

    QVector3D force_summ = QVector3D(0,0,0);
    QVector <QVector3D> forces;
    QVector3D torque_summ = QVector3D(0,0,0);
    QVector <QVector3D> torques;

    double density = 1;
    double Volume = 1;
    double mass = 1;
    QMatrix3x3 J;

    void set_location(const QVector3D& location);
    void set_orenatation(const QQuaternion& oreantation);
    double getDensity() const;
    void setDensity(double value);
    double getVolume() const;
    void setVolume(double value);
    double getMass() const;
    void setMass(double value);
    QVector3D getLinear_velocity() const;
    void setLinear_velocity(const QVector3D &value);
    QVector3D getAngular_velocity() const;
    void setAngular_velocity(const QVector3D &value);
    QVector3D getAngular_acceleration() const;
    void setAngular_acceleration(const QVector3D &value);
    QVector3D getLinear_acclereation() const;
    void setLinear_acclereation(const QVector3D &value);
    QVector3D getForce() const;
    void setForce(const QVector3D &value);
    QVector3D getTorque() const;
    void setTorque(const QVector3D &value);
    QVector3D getPosition() const;
    void setPosition(const QVector3D &value);
    QVector3D getScale() const;
    void setScale(const QVector3D &value);
public:
    virtual void init_geometry();
    virtual void draw();
    void draw_instances(int count, unsigned int position_buffer);
    virtual void set_projection(QMatrix4x4*);
    virtual void set_cam(QMatrix4x4*);
    void set_shader(QOpenGLShaderProgram* shader_);
    void set_context(QOpenGLContext* ctx_);
    QOpenGLContext* ctx = nullptr;
private:
    QOpenGLBuffer vertex_buffer;
    QOpenGLVertexArrayObject vertex_array;
    QOpenGLShaderProgram* shader = nullptr;
    QMatrix4x4* projection = nullptr;
    QMatrix4x4* camera = nullptr;
};

#endif // BODY_H
