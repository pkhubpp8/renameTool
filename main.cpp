#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyleSheet("QToolTip { color: #ffffff; background-color: #333333; border: 1px solid white; }");

    Widget w;
    w.show();
    return a.exec();
}
