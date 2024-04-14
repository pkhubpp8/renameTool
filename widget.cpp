#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("重命名工具");

    fileTableInit();
    replacePartInit();
    suffixPartInit();
    addIndexPartInit();
    isFirstRun = true;

    // this->setStyleSheet("QToolTip { color: #ffffff; background-color: #666666; border: 1px solid white; }");
}

void Widget::fileTableInit()
{
    QStringList horizontalLabels;
    horizontalLabels << "原始文件名" << "目标文件名" << "结果";
    ui->fileTableWidget->setColumnCount(horizontalLabels.size());
    ui->fileTableWidget->setHorizontalHeaderLabels(horizontalLabels);   // 设置行标签

    // QStringList verticalLabels;
    // verticalLabels << "1" << "2" << "3";
    // ui->fileTableWidget->setRowCount(verticalLabels.size());
    // ui->fileTableWidget->setVerticalHeaderLabels(verticalLabels);
    ui->fileTableWidget->verticalHeader()->setVisible(false);           // 隐藏列标签
}

void Widget::replacePartInit()
{
    ui->normalRadioButton->setChecked(true);
}

void Widget::suffixPartInit()
{
    ui->suffixLineEdit->setPlaceholderText("添加优先于修改，无需包含\".\"");
    QRegularExpression regExp("[^\\\\/:*?\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->suffixLineEdit->setValidator(regExpVal);
    connect(ui->suffixLineEdit, &QLineEdit::inputRejected, this, &Widget::showSuffixToolTip);
}

void Widget::addIndexPartInit()
{
    ui->addIndexRuleLineEdit->setPlaceholderText("查看帮助");
    ui->addIndexRuleLineEdit->setToolTip("*表示基础文件名\n?代表编号\n\n\
例如: image.jpg\n\
*_?=image_<编号>.jpg");
    ui->startSpinBox->setMinimum(1);
    ui->startSpinBox->setMaximum(65535);

    ui->stepSpinBox->setMinimum(1);
    ui->stepSpinBox->setMaximum(65535);

    ui->numOfDigitSpinBox->setMinimum(1);
    ui->numOfDigitSpinBox->setMaximum(1024);
}

void Widget::showSuffixToolTip()
{
    QToolTip::showText(ui->suffixLineEdit->mapToGlobal(QPoint()), R"(后缀名包含非法字符 \ / : * ? " < > |)");
}

QString Widget::getNewName(QString source, const int row)
{
    QFileInfo fileInfo(source);
    QString sourceFileName = fileInfo.fileName();
    QString dest = fileInfo.dir().filePath(sourceFileName);

    if (ui->normalRadioButton->isChecked())
    {
        QString input = ui->inputLineEdit->text();
        if (!input.isEmpty())
        {
            QString output = ui->outputLineEdit->text();
            dest = fileInfo.dir().filePath(sourceFileName.replace(input, output));
        }
    }
    else if (ui->regExpRadioButton->isChecked())
    {
        QString input = ui->inputLineEdit->text();
        if (!input.isEmpty())
        {
            QRegularExpression regex(input);
            QString output = ui->outputLineEdit->text();
            dest = fileInfo.dir().filePath(sourceFileName.replace(regex, output));
        }
    }

    QString suffix = ui->suffixLineEdit->text();
    if (!suffix.isEmpty() and ui->addSuffixCheckBox->isChecked())
    {
        QFileInfo fileInfo(dest);
        dest = fileInfo.dir().filePath(fileInfo.completeBaseName() + "." + fileInfo.suffix() + "." + ui->suffixLineEdit->text());
    }
    else if (!suffix.isEmpty() and ui->modifySuffixCheckBox->isChecked())
    {
        QFileInfo fileInfo(dest);
        dest = fileInfo.dir().filePath(fileInfo.completeBaseName() + "." + ui->suffixLineEdit->text());
    }

    QString rule = ui->addIndexRuleLineEdit->text();
    if (!rule.isEmpty())
    {
        int start = ui->startSpinBox->value();
        int step = ui->stepSpinBox->value();
        int width = ui->numOfDigitSpinBox->value();
        int currentIndex = start + row * step;
        QString indexStr = QString("%1").arg(currentIndex, width);
        qDebug() << "rule: " << rule;
        qDebug() << "当前文件是第" << row << "行，index为" << currentIndex << "编号为：" << indexStr;

        QFileInfo fileInfo(dest);
        QString tmp = rule;
        while (tmp.contains(QString("?")))
        {
            tmp.replace(QString("?"), indexStr);
        }
        while (tmp.contains(QString("*")))
        {
            tmp.replace(QString("*"), fileInfo.completeBaseName());
        }
        qDebug() << "tmp: " << tmp;
        dest = fileInfo.dir().filePath(fileInfo.suffix().isEmpty() ? tmp : (tmp + "." + fileInfo.suffix()));
    }

    return dest;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addFileButton_clicked()
{

    QStringList files;
    if (isFirstRun)
    {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件", desktopPath);
    }
    else
    {
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件");
    }
    isFirstRun = false;
    // for (auto& file: files)
    for (int i = 0; i < files.size(); i++)
    {
        if (!files[i].isEmpty()) {
            int row = ui->fileTableWidget->rowCount();
            qDebug() << "Selected file:" << files[i] << ", insert to row " << row;
            ui->fileTableWidget->insertRow(row);

            ui->fileTableWidget->setItem(row, 0, new QTableWidgetItem(files[i]));
            QString newName = getNewName(files[i], row);
            ui->fileTableWidget->setItem(row, 1, new QTableWidgetItem(newName));
            ui->fileTableWidget->setItem(row, 2, new QTableWidgetItem("还未执行"));

            // ui->fileTableWidget->setItem(row, column, item);
        }
    }
}


void Widget::on_renameButton_clicked()
{
    for (int row = 0; row < ui->fileTableWidget->rowCount(); ++row) {
        // 重命名
        QTableWidgetItem *sourceItem = ui->fileTableWidget->item(row, 0);
        QTableWidgetItem *destItem = ui->fileTableWidget->item(row, 1);
        QTableWidgetItem *resultItem = ui->fileTableWidget->item(row, 2);
        if (sourceItem and destItem and resultItem)
        {
            bool result = QFile::rename(sourceItem->text(), destItem->text());
            // qDebug() << "src: " << sourceItem->text() << ", dst " << destItem->text();
            if (result)
            {
                resultItem->setText("修改成功");
            }
            else
            {
                resultItem->setText("修改失败");
            }
        }
    }
}


void Widget::on_clearFileButton_clicked()
{
    refreshData();
}

void Widget::on_normalRadioButton_clicked()
{
    refreshData();
}

void Widget::on_regExpRadioButton_clicked()
{
    refreshData();
}

void Widget::on_addSuffixCheckBox_clicked()
{
    refreshData();
}

void Widget::on_modifySuffixCheckBox_clicked()
{
    refreshData();
}

void Widget::refreshData()
{
    for (int row = 0; row < ui->fileTableWidget->rowCount(); ++row) {
        // 重命名
        QTableWidgetItem *sourceItem = ui->fileTableWidget->item(row, 0);
        QTableWidgetItem *destItem = ui->fileTableWidget->item(row, 1);
        QTableWidgetItem *resultItem = ui->fileTableWidget->item(row, 2);
        if (sourceItem and destItem and resultItem)
        {
            QString newName = getNewName(sourceItem->text(), row);
            destItem->setText(newName);
        }
    }
}
