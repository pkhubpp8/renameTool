#include <QStandardPaths>

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    this->setWindowTitle("重命名工具");

    QStringList horizontalLabels;
    horizontalLabels << "原始文件名" << "目标文件名" << "结果";
    ui->fileTableWidget->setColumnCount(horizontalLabels.size());
    ui->fileTableWidget->setHorizontalHeaderLabels(horizontalLabels);   // 设置行标签

    // QStringList verticalLabels;
    // verticalLabels << "1" << "2" << "3";
    // ui->fileTableWidget->setRowCount(verticalLabels.size());
    // ui->fileTableWidget->setVerticalHeaderLabels(verticalLabels);
    ui->fileTableWidget->verticalHeader()->setVisible(false);           // 隐藏列标签

    ui->normalRadioButton->setChecked(true);
    isFirstTimeAdd = true;
}

QString Widget::getNewName(QString source)
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
    return dest;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addFileButton_clicked()
{

    QStringList files;
    if (isFirstTimeAdd)
    {
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件", desktopPath);
    }
    else
    {
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件");
    }
    isFirstTimeAdd = false;
    // for (auto& file: files)
    for (int i = 0; i < files.size(); i++)
    {
        if (!files[i].isEmpty()) {
            qDebug() << "Selected file:" << files[i];
            int row = ui->fileTableWidget->rowCount();
            ui->fileTableWidget->insertRow(row);

            ui->fileTableWidget->setItem(row, 0, new QTableWidgetItem(files[i]));
            QString newName = getNewName(files[i]);
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
        QString result = "未修改";
        QTableWidgetItem *item = ui->fileTableWidget->item(row, 2);
        if (item)
        {
            item->setText(result);
        }
    }
}

