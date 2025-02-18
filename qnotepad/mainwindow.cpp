#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowTitle("qnotepad");

    //字体保存
    QString font_setting;
        if (!(font_setting = settings.value("font").toString()).isEmpty())
        {
            font.fromString(font_setting);
            ui->plainTextEdit->setFont(font);
        }
    posLabel = new QLabel(QString::fromLocal8Bit("第 1 行，第 1 列"),this);
    ui->statusBar->addPermanentWidget(new QLabel(this), 1);
    ui->statusBar->addPermanentWidget(posLabel,1);

    /**连接信号与槽*/

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFileAction()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(saveFileAction()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exitAction()));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newFileAction()));
    connect(ui->actionSaveAs,SIGNAL(triggered()),this,SLOT(saveAsAction()));

    //编辑
    connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(undoAction()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(cutAction()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(copyAction()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(pasteAction()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(deleteAction()));
    connect(ui->actionFind,SIGNAL(triggered()),this,SLOT(findAction()));
    connect(ui->actionReplace,SIGNAL(triggered()),this,SLOT(replaceAction()));
    connect(ui->actionFindNext,SIGNAL(triggered()),this,SLOT(findNextAction()));
    connect(ui->actionFindPrev,SIGNAL(triggered()),this,SLOT(findPrevAction()));

    //关于
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutAction()));

    //编辑器
    connect(ui->plainTextEdit,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPosUpdate()));
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(textEditUpdate()));
    connect(ui->plainTextEdit,SIGNAL(undoAvailable()),this,SLOT(undoAble()));
    connect(ui->plainTextEdit,SIGNAL(selectionChanged()),this,SLOT(selectedUpdate()));

    //字体
   connect(ui->actionFont,SIGNAL(triggered()),this,SLOT(fontAction()));

   //打印
   connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(printAction()));

   //帮助
   connect(ui->actionHelp,SIGNAL(triggered()),this,SLOT(helpAction()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
/**槽*/
/**
 * @brief MainWindow::openFileAction 打开文件槽
 */
void MainWindow::openFileAction(){

    if(!isSaved()) return;
    QString recentPath = settings.value("recent/filePath").toString();
   QString filePath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),recentPath,QString::fromLocal8Bit("文本文档(*.txt);;所有文件(*.*)"));
   openFile(filePath);
}
/**
 * @brief MainWindow::saveFileAction 保存文件槽
 */
bool MainWindow::saveFileAction(){

    if(ui->plainTextEdit->toPlainText().isEmpty())
        return false;

    //如果路径名为空，就另存为
    if(filePath_.isEmpty()){
        QString recentPath = settings.value("recent/filePath").toString();//返回最近打开的路径
        QString filePath = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("另存为"),recentPath,QString::fromLocal8Bit("文本文档(*.txt);;所有文件(*.*)"));
        settings.setValue("recent/filePath", filePath);
        filePath_ = filePath;
        fileName_ = QFileInfo(filePath).baseName();
    }

    QFile file(filePath_);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setCodec("GBK");
    savedFileData = ui->plainTextEdit->toPlainText();//保存的文件数据
    stream << savedFileData;
    stream.flush();
    file.close();

    resetWindowTitle();
    return true;
}
/**
 * @brief MainWindow::exitAction 退出
 */
void MainWindow::exitAction(){

    if(!isSaved()) return;
    this->close();
}
/**
 * @brief MainWindow::newFileAction 新建
 */
void MainWindow::newFileAction(){

    if(!isSaved()) return;
    openFile("");
}
/**
 * @brief MainWindow::saveAsAction 另存为
 */
void MainWindow::saveAsAction(){

    QString temp = filePath_;
    filePath_ = "";
    if (!saveFileAction()) // 直接调用保存的
         filePath_ = temp;
}

/**打开文件API*/
void MainWindow::openFile(QString filePath){

    filePath_ = filePath;

    if(filePath.isEmpty()){
        savedFileData = "";//新建文件数据为空
        fileName_ = QString::fromLocal8Bit("无标题");
    }else{
        QFile file(filePath);
        if(!file.exists()) return;

        fileName_ = QFileInfo(filePath).baseName();

        if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QMessageBox::critical(this, "错误", "打开文件失败");
            return;
        }
        savedFileData = QString::fromLocal8Bit(file.readAll());//获取打开的文件数据
        file.close();
    }
    ui->plainTextEdit->setPlainText(savedFileData);
    resetWindowTitle();//重置窗口标题

}
/**
 * @brief MainWindow::resetWindowTitle 重置窗口标题
 */
void MainWindow::resetWindowTitle()
{
    this->setWindowTitle((isUpdate() ? "*" : "") + fileName_ + " - qnotepad");
}
/**
 * @brief MainWindow::isUpdate 是否更新
 * @return
 */
bool MainWindow::isUpdate()
{
    return ui->plainTextEdit->toPlainText() != savedFileData;
}
/**
 * @brief MainWindow::isSaved 是否保存
 * @return
 */
bool MainWindow::isSaved(){
    if(!isUpdate()) return true;//已经保存
    //未保存的修改
    int saveBtn = QMessageBox::question(this,"qnotepad",QString::fromLocal8Bit("你想要保存为") + (fileName_.isEmpty() ? QString::fromLocal8Bit("无标题")  : fileName_) + "?",QString::fromLocal8Bit("保存"),QString::fromLocal8Bit("不保存"),QString::fromLocal8Bit("取消"));
    if(saveBtn == 2)  return false;//取消
    else if(saveBtn == 0) return saveFileAction();
    return true;
}

/**窗口打开与关闭 */
void MainWindow::showEvent(QShowEvent *event){

    this->restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    this->restoreState(settings.value("mainwindow/state").toByteArray());
    QMainWindow::showEvent(event);
}
void MainWindow::closeEvent(QCloseEvent *event){

    if(!isSaved()) {
        event->ignore();
        return;
     }
    settings.setValue("mainwindow/geometry", this->saveGeometry());
    settings.setValue("mainwindow/state", this->saveState());
    QMainWindow::closeEvent(event);
}
/**
 * @brief MainWindow::newFindDialog 新建查找对话框
 */
void MainWindow::newFindDialog(){

    findAndReplaceDialog  =  new FindAndReplaceDialog(settings,this);

    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalShow, this, [=]{
            ui->actionFindNext->setEnabled(true);
            ui->actionFindPrev->setEnabled(true);
        });
    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalClose, this, [=]{
            ui->actionFindNext->setEnabled(false);
            ui->actionFindPrev->setEnabled(false);
        });
    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalFindNext, this, &MainWindow::findNextAction);
    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalFindPrev, this, &MainWindow::findPrevAction);
    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalReplceNext, this, [=]{
            const QString& findText = findAndReplaceDialog->getFindTextData();
            const QString& replaceText = findAndReplaceDialog->getReplaceTextData();
            if (findText.isEmpty()) return ;

    //设置选中的文本
    const QString& selectedText = ui->plainTextEdit->textCursor().selectedText();

    if((findAndReplaceDialog->isCaseSensitive() && selectedText != findText)  || selectedText.toLower() != findText.toLower()){
        //如果选中的词不是findText，查找下一个
        findNextAction();
    }else {
        //已选中则替换选中的
        QTextCursor tc = ui->plainTextEdit->textCursor();
        tc.insertText(replaceText);
        ui->plainTextEdit->setTextCursor(tc);

        findNextAction();
    }
});
    connect(findAndReplaceDialog, &FindAndReplaceDialog::signalReplaceAll, this, [=]{
            const QString& findText = findAndReplaceDialog->getFindTextData();
            const QString& replaceText = findAndReplaceDialog->getReplaceTextData();
            if (findText.isEmpty()) return;

            QString content = ui->plainTextEdit->toPlainText();
            QTextCursor textCursor = ui->plainTextEdit->textCursor();
            textCursor.setPosition(0);
            textCursor.setPosition(content.length(), QTextCursor::KeepAnchor);
            content.replace(findText, replaceText);
            textCursor.insertText(content);
        });
}
/**
 * @brief MainWindow::findNextAction 查找下一个槽
 */
void MainWindow::findNextAction(){

    const QString& text = findAndReplaceDialog->getFindTextData();
    if(text.isEmpty()) return;

    QTextDocument::FindFlags findFlags;

    if(findAndReplaceDialog->isCaseSensitive()) findFlags |= QTextDocument::FindCaseSensitively;

    bool result = ui->plainTextEdit->find(text,findFlags);


    if(!result && findAndReplaceDialog->isLoop() && ui->plainTextEdit->toPlainText().contains(text)){
        QTextCursor tc = ui->plainTextEdit->textCursor();//从头开始查找
        tc.setPosition(0);
        ui->plainTextEdit->setTextCursor(tc);
        findNextAction();
    }
}
/**
 * @brief MainWindow::findPrevAction 查找上一个
 */
void MainWindow::findPrevAction(){

    const QString& text = findAndReplaceDialog->getFindTextData();
    if(text.isEmpty()) return;

    QTextDocument::FindFlags findFlags = QTextDocument::FindBackward;

    if(findAndReplaceDialog->isCaseSensitive()) findFlags |= QTextDocument::FindCaseSensitively;

    bool result = ui->plainTextEdit->find(text,findFlags);

    if(!result && findAndReplaceDialog->isLoop() && ui->plainTextEdit->toPlainText().contains(text)){
        QTextCursor tc = ui->plainTextEdit->textCursor();//从末尾开始查找
        tc.setPosition(ui->plainTextEdit->toPlainText().length());//设置成末尾位置
        ui->plainTextEdit->setTextCursor(tc);
        findPrevAction();
    }
}
/**
 * @brief MainWindow::findAction 查找
 */
void MainWindow::findAction(){

    if(!findAndReplaceDialog) newFindDialog();

    findAndReplaceDialog->openFindOrReplaceDialog(false);
}

/**
 * @brief MainWindow::findAction 替换
 */
void MainWindow::replaceAction(){

    if(!findAndReplaceDialog) newFindDialog();

    findAndReplaceDialog->openFindOrReplaceDialog(true);
}

/**
 * @brief MainWindow::textEditUpdate 文本更新
 */
void MainWindow::textEditUpdate(){

    if(fileName_.isEmpty()) fileName_ = QString::fromLocal8Bit("无标题");
    resetWindowTitle();

    bool textEmpty = ui->plainTextEdit->toPlainText().isEmpty();
    ui->actionFind->setEnabled(!textEmpty);
    ui->actionReplace->setEnabled(!textEmpty);
    ui->actionFindNext->setEnabled(!textEmpty && findAndReplaceDialog && findAndReplaceDialog->isVisible());
    ui->actionFindPrev->setEnabled(!textEmpty && findAndReplaceDialog && findAndReplaceDialog->isVisible());
}
/**
 * @brief MainWindow::cursorPosUpdate 鼠标位置更新
 */
void MainWindow::cursorPosUpdate(){

    QTextCursor textCursor = ui->plainTextEdit->textCursor();

    QTextLayout* textLayout = textCursor.block().layout();

    int posInBlock = textCursor.position() - textCursor.block().position();
    int row = textLayout->lineForTextPosition(posInBlock).lineNumber() + textCursor.block().firstLineNumber();

    int col = textCursor.columnNumber();//列索引

    posLabel->setText(QString::fromLocal8Bit("第 ") + QString::number(row + 1) + QString::fromLocal8Bit(" 行，第 ") + QString::number(col + 1) + QString::fromLocal8Bit(" 列"));
}
/**
 * @brief MainWindow::undoAction 撤销
 */
void MainWindow::undoAble(bool flag){

    ui->actionUndo->setEnabled(flag);
}
/**
 * @brief MainWindow::selectedUpdate 选择的内容更新
 */
void MainWindow::selectedUpdate(){

    bool selected = ui->plainTextEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(selected);
    ui->actionCopy->setEnabled(selected);
    ui->actionDelete->setEnabled(selected);
}
/**
 * @brief MainWindow::undoAction 撤销
 */
void MainWindow::undoAction(){
    ui->plainTextEdit->undo();
}
/**
 * @brief MainWindow::cutAction 剪切
 */
void MainWindow::cutAction(){
    ui->plainTextEdit->cut();
}
/**
 * @brief MainWindow::copyAction 复制
 */
void MainWindow::copyAction(){
    ui->plainTextEdit->copy();
}
/**
 * @brief MainWindow::pasteAction 粘贴
 */
void MainWindow::pasteAction(){
    ui->plainTextEdit->paste();
}
/**
 * @brief MainWindow::deleteAction 删除
 */
void MainWindow::deleteAction(){
    QTextCursor textCursor = ui->plainTextEdit->textCursor();
        int pos = textCursor.position();
        if (pos >= ui->plainTextEdit->toPlainText().length())
            return ;
        textCursor.setPosition(pos + 1, QTextCursor::MoveMode::KeepAnchor);
        textCursor.removeSelectedText();
}
/**
 * @brief MainWindow::aboutAction 关于
 */

void MainWindow::aboutAction(){
    QMessageBox::about(this, QString::fromLocal8Bit("关于"), tr("qnotepad\n" "version1.0.0\n""simple notepad"));
}
/**
 * @brief MainWindow::fontAction 字体格式
 */
void MainWindow::fontAction(){
    bool flag;
    QFont font = QFontDialog::getFont(&flag,ui->plainTextEdit->font(),this,QString::fromLocal8Bit("字体"));

    if(!flag) return;
    ui->plainTextEdit->setFont(font);
    settings.setValue("font",font.toString());
}
/**
 * @brief MainWindow::printAction 打印，编译报错VS中缺少相应的库文件，在pro中添加QT+= printsupport选项即可
 */
void MainWindow::printAction(){

    QPrinter printer;
    QString printer_name = printer.printerName();

    if(printer_name.size() == 0) return;

    QPrintDialog printDialog(&printer,this);

    if(ui->plainTextEdit->textCursor().hasSelection()){
        printDialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    }
    if(printDialog.exec() == QDialog::Accepted){
        ui->plainTextEdit->print(&printer);
    }
}
/**
 * @brief MainWindow::helpAction 帮助
 */
void MainWindow::helpAction(){
    QDesktopServices::openUrl(QUrl("https://www.peiqiblog.com/article/4800/"));
}




