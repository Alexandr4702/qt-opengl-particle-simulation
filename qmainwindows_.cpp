#include "qmainwindows_.h"
#include <QResizeEvent>

Qmainwindows_::Qmainwindows_()
{
    resize(800, 600);
    game = new GL_GAME(this);
    game->setGeometry(QRect(0, 0, 800 * ratio, 600));
}
void Qmainwindows_::resizeEvent(QResizeEvent *event)
{
    game->setGeometry(QRect(0, 0, event->size().width()*ratio, event->size().height()));
}
