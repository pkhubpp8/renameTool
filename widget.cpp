#include <QMimeData>
#include <QDebug>

#include "widget.h"
#include "qmimedata.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , rHelper(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("重命名工具");

    fileTableInit();
    replacePartInit();
    suffixPartInit();
    addIndexPartInit();
    comboBoxInit();
    isFirstRun_ = true;

    // this->setStyleSheet("QToolTip { color: #ffffff; background-color: #666666; border: 1px solid white; }");
    setAcceptDrops(true);
}

void Widget::comboBoxInit()
{
    if (ui->conflictComboBox->count() == 0)
    {
        ui->conflictComboBox->addItems({"不解决"});
    }
    if (ui->letterCaseComboBox->count() == 0)
    {
        ui->letterCaseComboBox->addItems({"默认不修改", "小写", "大写", "首字母大写", "首字母小写", "大小写反转", "简体转繁体", "繁体转简体"}); // OpenCC
    }
    if (ui->caseTargetComboBox->count() == 0)
    {
        ui->caseTargetComboBox->addItems({"不可用"});//{"全文件名", "仅基础文件名", "仅后缀名"});
    }
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
    ui->regularHelperButton->hide();

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
例如: 原始文件名image.jpg\n\
new_*_?=new_image_<编号>.jpg");
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
    if ((!suffix.isEmpty()) && ui->addSuffixCheckBox->isChecked())
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
    else if (!suffix.isEmpty() && ui->modifySuffixCheckBox->isChecked())
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

    // letterCaseComboBox->addItems({"默认不修改", "小写", "大写", "首字母大写", "首字母小写", "大小写反转"});
    // caseTargetComboBox->addItems({"全文件名", "仅基础文件名", "仅后缀名"});
    QString letterCase = ui->letterCaseComboBox->currentText();
    if (letterCase == "小写")
    {

    }
    else if (letterCase == "大写")
    {

    }
    else if (letterCase == "首字母大写")
    {

    }
    else if (letterCase == "首字母小写")
    {

    }
    else if (letterCase == "大小写反转")
    {

    }
    else
    {

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
        if (!files[i].isEmpty())
        {
            insertToFileTable(files[i]);
        }
    }
}


void Widget::on_renameButton_clicked()
{
    for (int row = 0; row < ui->fileTableWidget->rowCount(); ++row)
    {
        // 重命名
        QTableWidgetItem *sourceItem = ui->fileTableWidget->item(row, 0);
        QTableWidgetItem *destItem = ui->fileTableWidget->item(row, 1);
        QTableWidgetItem *resultItem = ui->fileTableWidget->item(row, 2);
        if (sourceItem && destItem && resultItem)
        {
            QFileInfo srcFile(sourceItem->text());
            if (sourceItem->text() == srcFile.dir().filePath(destItem->text()))
            {
                resultItem->setText("无需修改");
                continue;
            }
            if (QFile::exists(srcFile.dir().filePath(destItem->text())))
            {
                resultItem->setText("文件名冲突");
            }
            else
            {
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
}

void Widget::refreshData()
{
    for (int row = 0; row < ui->fileTableWidget->rowCount(); ++row)
    {
        // 重命名
        QTableWidgetItem *sourceItem = ui->fileTableWidget->item(row, 0);
        QTableWidgetItem *destItem = ui->fileTableWidget->item(row, 1);
        QTableWidgetItem *resultItem = ui->fileTableWidget->item(row, 2);
        if (sourceItem && destItem && resultItem)
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
    if (isClearing)
        return;
    ui->regularHelperButton->hide();
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
    if (isClearing)
        return;
    ui->regularHelperButton->show();
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
    if (isClearing)
        return;
    qDebug() << __func__;
    refreshData();
}

void Widget::on_modifySuffixCheckBox_clicked()
{
    if (isClearing)
        return;
    qDebug() << __func__;
    refreshData();
}

void Widget::on_suffixLineEdit_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_addIndexRuleLineEdit_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_inputLineEdit_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}

void Widget::on_outputLineEdit_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_startSpinBox_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_stepSpinBox_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_numOfDigitSpinBox_textChanged(const QString &arg1)
{
    if (isClearing)
        return;
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

void Widget::dragEnterEvent(QDragEnterEvent *event){
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction(); // 接受拖放事件
    }
}

void Widget::dropEvent(QDropEvent *event){
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl &url : urlList)
        {
            QString filePath = url.toLocalFile(); // 获取文件路径
            insertToFileTable(filePath);
        }
        event->acceptProposedAction();
    }
}

void Widget::insertToFileTable(QString& filePath)
{
    auto result = observedFiles_.insert(filePath);
    if (result.second)
    {
        int row = ui->fileTableWidget->rowCount();
        qDebug() << "insert file:" << filePath << ", insert to row " << row;
        ui->fileTableWidget->insertRow(row);

        ui->fileTableWidget->setItem(row, 0, new QTableWidgetItem(filePath));
        QString newName = getNewName(filePath, row);
        ui->fileTableWidget->setItem(row, 1, new QTableWidgetItem(newName));
        ui->fileTableWidget->setItem(row, 2, new QTableWidgetItem("还未执行"));
    }
    else
    {
        qDebug() << "insert file:" << filePath << " already exists, do nothing";
    }
}

void Widget::on_letterCaseComboBox_currentTextChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_caseTargetComboBox_currentTextChanged(const QString &arg1)
{
    if (isClearing)
        return;
    qDebug() << __func__ << ": " << arg1;
    refreshData();
}


void Widget::on_clearAllPlanButton_clicked()
{
    qDebug() << __func__;

    isClearing = true;
    ui->addIndexRuleLineEdit->clear();
    ui->startSpinBox->setValue(1);
    ui->stepSpinBox->setValue(1);
    ui->numOfDigitSpinBox->setValue(1);

    ui->addSuffixCheckBox->setCheckState(Qt::Unchecked);
    ui->modifySuffixCheckBox->setCheckState(Qt::Unchecked);
    ui->suffixLineEdit->clear();

    replacePartInit();
    ui->inputLineEdit->clear();
    ui->outputLineEdit->clear();

    if (ui->conflictComboBox->count() != 0)
    {
        ui->conflictComboBox->setCurrentIndex(0);
    }
    if (ui->letterCaseComboBox->count() != 0)
    {
        ui->letterCaseComboBox->setCurrentIndex(0);
    }
    if (ui->caseTargetComboBox->count() != 0)
    {
        ui->caseTargetComboBox->setCurrentIndex(0);
    }
    isClearing = false;
    refreshData();
}


void Widget::on_regularHelperButton_clicked()
{
    qDebug() << __func__;
    if (rHelper == nullptr)
    {
        rHelper = new regularHelper();
        rHelper->show();
    }
    else
    {
        rHelper->show();
    }
}

