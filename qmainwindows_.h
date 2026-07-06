#ifndef QMAINWINDOWS__H
#define QMAINWINDOWS__H

#include <QWidget>
#include "gl_game.h"

class Qmainwindows_ :public QWidget
{
//    Q_OBJECT
public:
     Qmainwindows_();
private:
     GL_GAME* game;
};


#endif // QMAINWINDOWS__H
