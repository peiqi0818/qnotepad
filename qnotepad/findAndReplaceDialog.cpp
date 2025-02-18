#include "findAndReplaceDialog.h"
#include "ui_findAndReplaceDialog.h"

FindAndReplaceDialog::FindAndReplaceDialog(QSettings &settings,QWidget *parent)
    :QDialog(parent),
    ui(new Ui::FindAndReplaceDialog),
    settings(settings)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint,false);

    //保存对话框中的数据
    ui->findEdit->setText(settings.value("find/findText").toString());
    ui->replaceEdit->setText(settings.value("find/replaceText").toString());
    ui->caseSensitiveCheck->setChecked(settings.value("find/caseSensitive").toBool());
    ui->loopCheck->setChecked(settings.value("find/loop").toBool());
    if (!settings.value("find/down", true).toBool())
          ui->upRadio->setChecked(true);

    //连接信号与槽
    connect(ui->findNextBtn,SIGNAL(clicked()),this,SLOT(findNextSlots()));
    connect(ui->replaceBtn,SIGNAL(clicked()),this,SLOT(replaceSlots()));
    connect(ui->replaceAllBtn,SIGNAL(clicked()),this,SLOT(replaceAllSlots()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(cancelSlots()));
    connect(ui->caseSensitiveCheck,SIGNAL(clicked()),this,SLOT(caseSensitiveSlots()));
    connect(ui->loopCheck,SIGNAL(clicked()),this,SLOT(loopSlots()));
    connect(ui->upRadio,SIGNAL(clicked()),this,SLOT(upSlots()));
    connect(ui->downRadio,SIGNAL(clicked()),this,SLOT(downSlots()));
}

FindAndReplaceDialog::~FindAndReplaceDialog(){
    delete ui;
}
/**
 * @brief FindAndReplaceDialog::openFind 打开查找对话框
 * @param isReplace
 */
void FindAndReplaceDialog::openFindOrReplaceDialog(bool isReplace){

    //控制选项的显示，如果是查找则不显示下列该项
    ui->label_2->setVisible(isReplace);
    ui->replaceEdit->setVisible(isReplace);
    ui->replaceBtn->setVisible(isReplace);
    ui->replaceAllBtn->setVisible(isReplace);
    ui->groupBox->setVisible(!isReplace);
    QDialog::show();
    ui->findEdit->setFocus();
    ui->findEdit->selectAll();
    this->adjustSize();

    if(!isReplace)
        setWindowTitle("查找");
    else
        setWindowTitle("替换");
}
/**
 * @brief FindAndReplaceDialog::isLoop 是否循环
 * @return
 */
bool FindAndReplaceDialog::isLoop(){

    return ui->loopCheck->isChecked();
}
/**
 * @brief FindAndReplaceDialog::isCaseSensitive 区分大小写
 * @return
 */
bool FindAndReplaceDialog::isCaseSensitive(){
    return ui->caseSensitiveCheck->isChecked();
}
/**
 * @brief FindAndReplaceDialog::getFindTextData 获取查找的文本
 * @return
 */
QString FindAndReplaceDialog::getFindTextData(){

    return ui->findEdit->text();
}
/**
 * @brief FindAndReplaceDialog::getReplaceTextData 获取替换的文本
 * @return
 */
QString FindAndReplaceDialog::getReplaceTextData(){
    return ui->replaceEdit->text();
}


/**
 * @brief FindAndReplaceDialog::findNextSlots 查找下一个，按钮
 */
void FindAndReplaceDialog::findNextSlots(){

    if(ui->upRadio->isChecked()){
        emit signalFindPrev();
    }else {
        emit signalFindNext();
    }
    settings.setValue("find/findText", ui->findEdit->text());
}
/**
 * @brief FindAndReplaceDialog::replaceSlots 替换文本
 */
void FindAndReplaceDialog::replaceSlots(){

    emit signalReplceNext();
    settings.setValue("find/replaceText", ui->replaceEdit->text());
}

/**
 * @brief FindAndReplaceDialog::replaceAllSlots 全部替换
 */
void FindAndReplaceDialog::replaceAllSlots(){

    emit signalReplaceAll();
    settings.setValue("find/replaceText", ui->replaceEdit->text());
}
/**
 * @brief FindAndReplaceDialog::cancelSlots 取消
 */
void FindAndReplaceDialog::cancelSlots(){

    this->close();
}
/**
 * @brief FindAndReplaceDialog::caseSensitiveSlots 区分大小写
 */
void FindAndReplaceDialog::caseSensitiveSlots(){
    settings.setValue("find/caseSensitive", ui->caseSensitiveCheck->isChecked());
}
/**
 * @brief FindAndReplaceDialog::loopSlots 是否循环
 */
void FindAndReplaceDialog::loopSlots(){
    settings.setValue("find/loop", ui->loopCheck->isChecked());
}
/**
 * @brief FindAndReplaceDialog::upSlots 向上
 */
void FindAndReplaceDialog::upSlots(){
    settings.setValue("find/down", false);
}
/**
 * @brief FindAndReplaceDialog::downSlots 向下
 */
void FindAndReplaceDialog::downSlots(){

    settings.setValue("find/down", true);
}
/**
 * @brief FindAndReplaceDialog::findEditSlots 查找编辑框
 */
void FindAndReplaceDialog::findEditSlots(){
    findNextSlots();
}
/**
 * @brief FindAndReplaceDialog::replaceEditSlots 替换编辑框
 */
void FindAndReplaceDialog::replaceEditSlots(){
    replaceSlots();
}



