﻿#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QToolTip>
#include <QComboBox>
#include <QDropEvent>

#include <set>
#include "regularhelper.h"

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

    void showNormalReplaceTip();

    void showRegularReplaceTip();

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

    void on_rmFileButton_clicked();

    void on_letterCaseComboBox_currentTextChanged(const QString &arg1);

    void on_caseTargetComboBox_currentTextChanged(const QString &arg1);

    void on_clearAllPlanButton_clicked();

    void on_regularHelperButton_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void comboBoxInit();
    void fileTableInit();
    void replacePartInit();
    void suffixPartInit();
    void addIndexPartInit();
    void refreshData();
    void insertToFileTable(QString& filePath);

    Ui::Widget *ui;
    regularHelper *rHelper;
    bool isFirstRun_ = true;
    std::set<QString> observedFiles_;
    bool isClearing = false;
};
#endif // WIDGET_H
