#include "camera.h"

#include <QtMath>

QMatrix4x4 Camera::view_matrix() const
{
    const float yaw_radians = qDegreesToRadians(yaw);
    const float pitch_radians = qDegreesToRadians(pitch);
    const float horizontal_distance = distance * qCos(pitch_radians);
    const QVector3D position = target + QVector3D(
        horizontal_distance * qSin(yaw_radians),
        distance * qSin(pitch_radians),
        horizontal_distance * qCos(yaw_radians));

    QMatrix4x4 view;
    view.lookAt(position, target, QVector3D(0.0f, 1.0f, 0.0f));
    return view;
}

void Camera::orbit(float horizontal_delta, float vertical_delta)
{
    desired_yaw -= horizontal_delta * 0.4f;
    desired_pitch = qBound(-85.0f,
                           desired_pitch + vertical_delta * 0.4f, 85.0f);
}

void Camera::zoom(float wheel_steps)
{
    desired_distance *= qPow(0.85f, wheel_steps);
    desired_distance = qBound(2.0f, desired_distance, 80.0f);
}

void Camera::move(float forward_delta, float right_delta, float vertical_delta)
{
    const float yaw_radians = qDegreesToRadians(desired_yaw);
    const float pitch_radians = qDegreesToRadians(desired_pitch);
    const QVector3D forward(qCos(pitch_radians) * -qSin(yaw_radians),
                            -qSin(pitch_radians),
                            qCos(pitch_radians) * -qCos(yaw_radians));
    const QVector3D right = QVector3D::crossProduct(
        forward, QVector3D(0.0f, 1.0f, 0.0f)).normalized();

    desired_target += forward * forward_delta
                    + right * right_delta
                    + QVector3D(0.0f, vertical_delta, 0.0f);
}

void Camera::update(float delta_seconds)
{
    const float blend = 1.0f - qExp(-12.0f * qMin(delta_seconds, 0.1f));
    target += (desired_target - target) * blend;
    yaw += (desired_yaw - yaw) * blend;
    pitch += (desired_pitch - pitch) * blend;
    distance += (desired_distance - distance) * blend;
}
