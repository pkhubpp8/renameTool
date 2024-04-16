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

    void showSuffixLineEditToolTip();

    void showAddIndexToolTip();

    void on_clearFileButton_clicked();

    void on_normalRadioButton_clicked();

    void on_regExpRadioButton_clicked();

    void on_addSuffixCheckBox_clicked();

    void on_modifySuffixCheckBox_clicked();

    void on_suffixLineEdit_textChanged(const QString &arg1);

    void on_addIndexRuleLineEdit_textChanged(const QString &arg1);

    void on_inputLineEdit_textChanged(const QString &arg1);

    void on_outputLineEdit_textChanged(const QString &arg1);

    void on_startSpinBox_textChanged(const QString &arg1);

    void on_stepSpinBox_textChanged(const QString &arg1);

    void on_numOfDigitSpinBox_textChanged(const QString &arg1);

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
