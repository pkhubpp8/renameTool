#ifndef REGULARHELPER_H
#define REGULARHELPER_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class regularHelper;
}

class regularHelper : public QWidget
{
    Q_OBJECT

public:
    explicit regularHelper(QWidget *parent = nullptr);
    ~regularHelper();

private:
    Ui::regularHelper *ui;
};

#endif // REGULARHELPER_H
