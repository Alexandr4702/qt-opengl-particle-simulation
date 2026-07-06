#ifndef GL_GAME_H
#define GL_GAME_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QPoint>
#include "camera.h"
#include "world.h"

class GL_GAME: public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    GL_GAME(QWidget *parent);
    ~GL_GAME();
    bool toggle_simulation();
    void reset_simulation();
    void spawn_particles(int count);
    void set_time_scale(double scale);
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void initShader();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    void rebuild_scene();

    Body* Cube;
    World* world = nullptr;
    QPoint last_mouse_position;
    Camera camera;
    QElapsedTimer frame_timer;
    int particle_count = 512;
    float simulation_time_scale = 1.0f;
    bool simulation_running = false;
    bool rebuild_requested = false;

};

#endif // GL_GAME_H
