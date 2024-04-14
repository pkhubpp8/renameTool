#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QToolTip>

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
    QString getNewName(QString source, const int row);
    ~Widget();

private slots:
    void on_addFileButton_clicked();

    void on_renameButton_clicked();

    void showSuffixToolTip();

    void on_clearFileButton_clicked();

    void on_normalRadioButton_clicked();

    void on_regExpRadioButton_clicked();

    void on_addSuffixCheckBox_clicked();

    void on_modifySuffixCheckBox_clicked();

private:
    void fileTableInit();
    void replacePartInit();
    void suffixPartInit();
    void addIndexPartInit();
    void refreshData();

    Ui::Widget *ui;
    bool isFirstRun = true;
};
#endif // WIDGET_H
