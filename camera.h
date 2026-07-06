#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
    QMatrix4x4 view_matrix() const;
    void orbit(float horizontal_delta, float vertical_delta);
    void zoom(float wheel_steps);
    void move(float forward_delta, float right_delta, float vertical_delta);
    void update(float delta_seconds);

private:
    QVector3D target = QVector3D(0.0f, 0.0f, -20.0f);
    QVector3D desired_target = target;
    float yaw = 0.0f;
    float desired_yaw = yaw;
    float pitch = 20.0f;
    float desired_pitch = pitch;
    float distance = 12.0f;
    float desired_distance = distance;
};

#endif // CAMERA_H
