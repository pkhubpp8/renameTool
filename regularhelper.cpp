#include "regularhelper.h"
#include "ui_regularhelper.h"

regularHelper::regularHelper(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::regularHelper)
{
    qDebug() << "new helper";
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    this->setWindowTitle("正则表达式语法");
}

regularHelper::~regularHelper()
{
    qDebug() << "release helper";
    delete ui;
}
