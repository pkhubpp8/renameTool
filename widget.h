#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    QString getNewName(QString source);
    ~Widget();

private slots:
    void on_addFileButton_clicked();

    void on_renameButton_clicked();

    void showSuffixToolTip();

private:
    Ui::Widget *ui;
    bool isFirstTimeAdd = true;
};
#endif // WIDGET_H
