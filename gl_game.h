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
    Body* Cube;
    World* world = nullptr;
    QPoint last_mouse_position;
    Camera camera;
    QElapsedTimer frame_timer;

};

#endif // GL_GAME_H
