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
    isFirstRun_ = true;

    // this->setStyleSheet("QToolTip { color: #ffffff; background-color: #666666; border: 1px solid white; }");
}

void Widget::fileTableInit()
{
    QStringList horizontalLabels;
    horizontalLabels << "原始文件绝对路径" << "目标文件名" << "结果";
    ui->fileTableWidget->setColumnCount(horizontalLabels.size());
    ui->fileTableWidget->setHorizontalHeaderLabels(horizontalLabels);   // 设置行标签

    // QStringList verticalLabels;
    // verticalLabels << "1" << "2" << "3";
    // ui->fileTableWidget->setRowCount(verticalLabels.size());
    // ui->fileTableWidget->setVerticalHeaderLabels(verticalLabels);
    ui->fileTableWidget->verticalHeader()->setVisible(false);           // 隐藏列标签
    ui->fileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Widget::replacePartInit()
{
    ui->normalRadioButton->setChecked(true);

    QRegularExpression regExp("[^\\\\/:*?\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->outputLineEdit->setValidator(regExpVal);
    connect(ui->outputLineEdit, &QLineEdit::inputRejected, this, &Widget::showNormalReplaceTip);
}

void Widget::suffixPartInit()
{
    ui->suffixLineEdit->setPlaceholderText("添加优先于修改，无需包含\".\"");
    QRegularExpression regExp("[^\\\\/:*?\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->suffixLineEdit->setValidator(regExpVal);
    connect(ui->suffixLineEdit, &QLineEdit::inputRejected, this, &Widget::showSuffixLineEditToolTip);
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


    QRegularExpression regExp("[^\\\\/:\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->addIndexRuleLineEdit->setValidator(regExpVal);
    connect(ui->addIndexRuleLineEdit, &QLineEdit::inputRejected, this, &Widget::showAddIndexToolTip);
}

void Widget::showAddIndexToolTip()
{
    QToolTip::showText(ui->addIndexRuleLineEdit->mapToGlobal(QPoint()), R"(文件名不能包含非法字符 \ / : " < > |)");
}


void Widget::showSuffixLineEditToolTip()
{
    QToolTip::showText(ui->suffixLineEdit->mapToGlobal(QPoint()), R"(后缀名不能包含非法字符 \ / : * ? " < > |)");
}

QString Widget::getNewName(QString source, const int row)
{
    QFileInfo fileInfo(source);
    QString sourceFileName = fileInfo.fileName();
    QString dest = fileInfo.dir().filePath(sourceFileName);

    if (ui->normalRadioButton->isChecked())
    {
        QFileInfo tmpDstFileInfo(dest);
        QString input = ui->inputLineEdit->text();
        if (!input.isEmpty())
        {
            QString output = ui->outputLineEdit->text();
            dest = tmpDstFileInfo.dir().filePath(sourceFileName.replace(input, output));

            qDebug() << "normal replace, dest: " << dest;
        }
    }
    else if (ui->regExpRadioButton->isChecked())
    {
        QFileInfo tmpDstFileInfo(dest);
        QString input = ui->inputLineEdit->text();
        if (!input.isEmpty())
        {
            QRegularExpression regex(input);
            QString output = ui->outputLineEdit->text();
            dest = tmpDstFileInfo.dir().filePath(sourceFileName.replace(regex, output));

            // todo: 分组替换

            qDebug() << "regular replace, dest: " << dest;
        }
    }

    QString suffix = ui->suffixLineEdit->text();
    if (!suffix.isEmpty() and ui->addSuffixCheckBox->isChecked())
    {
        QFileInfo tmpDstFileInfo(dest);
        if (tmpDstFileInfo.suffix().isEmpty())
        {
            dest = tmpDstFileInfo.dir().filePath(tmpDstFileInfo.completeBaseName() + "." + ui->suffixLineEdit->text());
        }
        else
        {
            dest = tmpDstFileInfo.dir().filePath(tmpDstFileInfo.completeBaseName() + "." + tmpDstFileInfo.suffix() + "." + ui->suffixLineEdit->text());
        }

        qDebug() << "suffix add, dest: " << dest;
    }
    else if (!suffix.isEmpty() and ui->modifySuffixCheckBox->isChecked())
    {
        QFileInfo tmpDstFileInfo(dest);
        if (!tmpDstFileInfo.suffix().isEmpty())
        {
            dest = tmpDstFileInfo.dir().filePath(tmpDstFileInfo.completeBaseName() + "." + ui->suffixLineEdit->text());
        }
        qDebug() << "suffix modify, dest: " << dest;
    }

    QString rule = ui->addIndexRuleLineEdit->text();
    if (!rule.isEmpty())
    {
        int start = ui->startSpinBox->value();
        int step = ui->stepSpinBox->value();
        int width = ui->numOfDigitSpinBox->value();
        int currentIndex = start + row * step;
        QString indexStr = QString("%1").arg(currentIndex, width, 10, QChar('0'));
        qDebug() << "rule: " << rule;
        qDebug() << "当前文件是第" << row << "行，index为" << currentIndex << "编号为：" << indexStr;

        QFileInfo tmpDstFileInfo(dest);
        QString tmp = rule;
        while (tmp.contains(QString("?")))
        {
            tmp.replace(QString("?"), indexStr);
        }
        while (tmp.contains(QString("*")))
        {
            tmp.replace(QString("*"), tmpDstFileInfo.completeBaseName());
        }
        qDebug() << "tmp: " << tmp;
        dest = tmpDstFileInfo.dir().filePath(tmpDstFileInfo.suffix().isEmpty() ? tmp : (tmp + "." + tmpDstFileInfo.suffix()));
    }

    QFileInfo dstFile(dest);
    return dstFile.fileName();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_addFileButton_clicked()
{

    QStringList files;
    if (isFirstRun_)
    {
        isFirstRun_ = false;
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件", desktopPath);
    }
    else
    {
        files = QFileDialog::getOpenFileNames(nullptr, "选择文件");
    }

    for (int i = 0; i < files.size(); i++)
    {
        if (!files[i].isEmpty()) {
            auto result = observedFiles_.insert(files[i]);
            if (result.second)
            {
                int row = ui->fileTableWidget->rowCount();
                qDebug() << "Selected file:" << files[i] << ", insert to row " << row;
                ui->fileTableWidget->insertRow(row);

                ui->fileTableWidget->setItem(row, 0, new QTableWidgetItem(files[i]));
                QString newName = getNewName(files[i], row);
                ui->fileTableWidget->setItem(row, 1, new QTableWidgetItem(newName));
                ui->fileTableWidget->setItem(row, 2, new QTableWidgetItem("还未执行"));

                // ui->fileTableWidget->setItem(row, column, item);
            }
            else
            {
                qDebug() << "Selected file:" << files[i] << " already exists, do nothing";
            }
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
            QFileInfo srcFile(sourceItem->text());
            bool result = QFile::rename(sourceItem->text(), srcFile.dir().filePath(destItem->text()));
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

void Widget::on_clearFileButton_clicked()
{
    qDebug() << __func__;
    ui->fileTableWidget->clearContents();
    ui->fileTableWidget->setRowCount(0);
    observedFiles_.clear();
    refreshData();
}

void Widget::on_normalRadioButton_clicked()
{
    if (ui->outputLineEdit->validator())
    {
        delete ui->outputLineEdit->validator();
        ui->outputLineEdit->setValidator(nullptr);
    }
    QRegularExpression regExp("[^\\\\/:*?\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->outputLineEdit->setValidator(regExpVal);

    connect(ui->outputLineEdit, &QLineEdit::inputRejected, this, &Widget::showNormalReplaceTip);

    qDebug() << __func__;
    refreshData();
}

void Widget::showNormalReplaceTip()
{
    QToolTip::showText(ui->outputLineEdit->mapToGlobal(QPoint()), R"(文件名不能包含非法字符 \ / : * ? " < > |)");
}

void Widget::on_regExpRadioButton_clicked()
{
    if (ui->outputLineEdit->validator())
    {
        delete ui->outputLineEdit->validator();
        ui->outputLineEdit->setValidator(nullptr);
    }
    QRegularExpression regExp("[^\\/:*?\"<>|]+");
    QRegularExpressionValidator *regExpVal = new QRegularExpressionValidator();
    regExpVal->setRegularExpression(regExp);
    ui->outputLineEdit->setValidator(regExpVal);
    connect(ui->outputLineEdit, &QLineEdit::inputRejected, this, &Widget::showRegularReplaceTip);

    qDebug() << __func__;
    refreshData();
}

void Widget::showRegularReplaceTip()
{
    QToolTip::showText(ui->outputLineEdit->mapToGlobal(QPoint()), R"(文件名不能包含非法字符 / : * ? " < > |)");
}

void Widget::on_addSuffixCheckBox_clicked()
{
    qDebug() << __func__;
    refreshData();
}

void Widget::on_modifySuffixCheckBox_clicked()
{
    qDebug() << __func__;
    refreshData();
}

void Widget::on_suffixLineEdit_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_addIndexRuleLineEdit_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_inputLineEdit_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_outputLineEdit_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_startSpinBox_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_stepSpinBox_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_numOfDigitSpinBox_textChanged(const QString &arg1)
{
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_rmFileButton_clicked()
{
    QList<QTableWidgetItem *> listToRemove = ui->fileTableWidget->selectedItems();
    std::set<int> rowToRemove;
    for(int i = 0; i < listToRemove.count(); i++)
    {
        QTableWidgetItem *item = listToRemove.at(i);
        if (item)
        {
            rowToRemove.insert(item->row());
        }
        //qDebug() << "selecteditem " << item->text();
    }

    for (auto& it: rowToRemove)
    {
        qDebug() << "remove row " << it;
        QTableWidgetItem *sourceItem = ui->fileTableWidget->item(it, 0);
        if (sourceItem)
        {
            observedFiles_.erase(sourceItem->text());
        }
        else
        {
            qDebug() << "warning: source item is not exist for row " << it;
        }
        ui->fileTableWidget->removeRow(it);
    }
}

