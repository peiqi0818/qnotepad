#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowTitle("qnotepad");

    //���屣��
    QString font_setting;
        if (!(font_setting = settings.value("font").toString()).isEmpty())
        {
            font.fromString(font_setting);
            ui->plainTextEdit->setFont(font);
        }
    posLabel = new QLabel(QString::fromLocal8Bit("�� 1 �У��� 1 ��"),this);
    ui->statusBar->addPermanentWidget(new QLabel(this), 1);
    ui->statusBar->addPermanentWidget(posLabel,1);

    /**�����ź����*/

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFileAction()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(saveFileAction()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exitAction()));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newFileAction()));
    connect(ui->actionSaveAs,SIGNAL(triggered()),this,SLOT(saveAsAction()));

    //�༭
    connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(undoAction()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(cutAction()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(copyAction()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(pasteAction()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(deleteAction()));
    connect(ui->actionFind,SIGNAL(triggered()),this,SLOT(findAction()));
    connect(ui->actionReplace,SIGNAL(triggered()),this,SLOT(replaceAction()));
    connect(ui->actionFindNext,SIGNAL(triggered()),this,SLOT(findNextAction()));
    connect(ui->actionFindPrev,SIGNAL(triggered()),this,SLOT(findPrevAction()));

    //����
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutAction()));

    //�༭��
    connect(ui->plainTextEdit,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPosUpdate()));
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(textEditUpdate()));
    connect(ui->plainTextEdit,SIGNAL(undoAvailable()),this,SLOT(undoAble()));
    connect(ui->plainTextEdit,SIGNAL(selectionChanged()),this,SLOT(selectedUpdate()));

    //����
   connect(ui->actionFont,SIGNAL(triggered()),this,SLOT(fontAction()));

   //��ӡ
   connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(printAction()));

   //����
   connect(ui->actionHelp,SIGNAL(triggered()),this,SLOT(helpAction()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
/**��*/
/**
 * @brief MainWindow::openFileAction ���ļ���
 */
void MainWindow::openFileAction(){

    if(!isSaved()) return;
    QString recentPath = settings.value("recent/filePath").toString();
   QString filePath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("��"),recentPath,QString::fromLocal8Bit("�ı��ĵ�(*.txt);;�����ļ�(*.*)"));
   openFile(filePath);
}
/**
 * @brief MainWindow::saveFileAction �����ļ���
 */
bool MainWindow::saveFileAction(){

    if(ui->plainTextEdit->toPlainText().isEmpty())
        return false;

    //���·����Ϊ�գ������Ϊ
    if(filePath_.isEmpty()){
        QString recentPath = settings.value("recent/filePath").toString();//��������򿪵�·��
        QString filePath = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("���Ϊ"),recentPath,QString::fromLocal8Bit("�ı��ĵ�(*.txt);;�����ļ�(*.*)"));
        settings.setValue("recent/filePath", filePath);
        filePath_ = filePath;
        fileName_ = QFileInfo(filePath).baseName();
    }

    QFile file(filePath_);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setCodec("GBK");
    savedFileData = ui->plainTextEdit->toPlainText();//������ļ�����
    stream << savedFileData;
    stream.flush();
    file.close();

    resetWindowTitle();
    return true;
}
/**
 * @brief MainWindow::exitAction �˳�
 */
void MainWindow::exitAction(){

    if(!isSaved()) return;
    this->close();
}
/**
 * @brief MainWindow::newFileAction �½�
 */
void MainWindow::newFileAction(){

    if(!isSaved()) return;
    openFile("");
}
/**
 * @brief MainWindow::saveAsAction ���Ϊ
 */
void MainWindow::saveAsAction(){

    QString temp = filePath_;
    filePath_ = "";
    if (!saveFileAction()) // ֱ�ӵ��ñ����
         filePath_ = temp;
}

/**���ļ�API*/
void MainWindow::openFile(QString filePath){

    filePath_ = filePath;

    if(filePath.isEmpty()){
        savedFileData = "";//�½��ļ�����Ϊ��
        fileName_ = QString::fromLocal8Bit("�ޱ���");
    }else{
        QFile file(filePath);
        if(!file.exists()) return;

        fileName_ = QFileInfo(filePath).baseName();

        if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QMessageBox::critical(this, "����", "���ļ�ʧ��");
            return;
        }
        savedFileData = QString::fromLocal8Bit(file.readAll());//��ȡ�򿪵��ļ�����
        file.close();
    }
    ui->plainTextEdit->setPlainText(savedFileData);
    resetWindowTitle();//���ô��ڱ���

}
/**
 * @brief MainWindow::resetWindowTitle ���ô��ڱ���
 */
void MainWindow::resetWindowTitle()
{
    this->setWindowTitle((isUpdate() ? "*" : "") + fileName_ + " - qnotepad");
}
/**
 * @brief MainWindow::isUpdate �Ƿ����
 * @return
 */
bool MainWindow::isUpdate()
{
    return ui->plainTextEdit->toPlainText() != savedFileData;
}
/**
 * @brief MainWindow::isSaved �Ƿ񱣴�
 * @return
 */
bool MainWindow::isSaved(){
    if(!isUpdate()) return true;//�Ѿ�����
    //δ������޸�
    int saveBtn = QMessageBox::question(this,"qnotepad",QString::fromLocal8Bit("����Ҫ����Ϊ") + (fileName_.isEmpty() ? QString::fromLocal8Bit("�ޱ���")  : fileName_) + "?",QString::fromLocal8Bit("����"),QString::fromLocal8Bit("������"),QString::fromLocal8Bit("ȡ��"));
    if(saveBtn == 2)  return false;//ȡ��
    else if(saveBtn == 0) return saveFileAction();
    return true;
}

/**���ڴ���ر� */
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
 * @brief MainWindow::newFindDialog �½����ҶԻ���
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

    //����ѡ�е��ı�
    const QString& selectedText = ui->plainTextEdit->textCursor().selectedText();

    if((findAndReplaceDialog->isCaseSensitive() && selectedText != findText)  || selectedText.toLower() != findText.toLower()){
        //���ѡ�еĴʲ���findText��������һ��
        findNextAction();
    }else {
        //��ѡ�����滻ѡ�е�
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
 * @brief MainWindow::findNextAction ������һ����
 */
void MainWindow::findNextAction(){

    const QString& text = findAndReplaceDialog->getFindTextData();
    if(text.isEmpty()) return;

    QTextDocument::FindFlags findFlags;

    if(findAndReplaceDialog->isCaseSensitive()) findFlags |= QTextDocument::FindCaseSensitively;

    bool result = ui->plainTextEdit->find(text,findFlags);


    if(!result && findAndReplaceDialog->isLoop() && ui->plainTextEdit->toPlainText().contains(text)){
        QTextCursor tc = ui->plainTextEdit->textCursor();//��ͷ��ʼ����
        tc.setPosition(0);
        ui->plainTextEdit->setTextCursor(tc);
        findNextAction();
    }
}
/**
 * @brief MainWindow::findPrevAction ������һ��
 */
void MainWindow::findPrevAction(){

    const QString& text = findAndReplaceDialog->getFindTextData();
    if(text.isEmpty()) return;

    QTextDocument::FindFlags findFlags = QTextDocument::FindBackward;

    if(findAndReplaceDialog->isCaseSensitive()) findFlags |= QTextDocument::FindCaseSensitively;

    bool result = ui->plainTextEdit->find(text,findFlags);

    if(!result && findAndReplaceDialog->isLoop() && ui->plainTextEdit->toPlainText().contains(text)){
        QTextCursor tc = ui->plainTextEdit->textCursor();//��ĩβ��ʼ����
        tc.setPosition(ui->plainTextEdit->toPlainText().length());//���ó�ĩβλ��
        ui->plainTextEdit->setTextCursor(tc);
        findPrevAction();
    }
}
/**
 * @brief MainWindow::findAction ����
 */
void MainWindow::findAction(){

    if(!findAndReplaceDialog) newFindDialog();

    findAndReplaceDialog->openFindOrReplaceDialog(false);
}

/**
 * @brief MainWindow::findAction �滻
 */
void MainWindow::replaceAction(){

    if(!findAndReplaceDialog) newFindDialog();

    findAndReplaceDialog->openFindOrReplaceDialog(true);
}

/**
 * @brief MainWindow::textEditUpdate �ı�����
 */
void MainWindow::textEditUpdate(){

    if(fileName_.isEmpty()) fileName_ = QString::fromLocal8Bit("�ޱ���");
    resetWindowTitle();

    bool textEmpty = ui->plainTextEdit->toPlainText().isEmpty();
    ui->actionFind->setEnabled(!textEmpty);
    ui->actionReplace->setEnabled(!textEmpty);
    ui->actionFindNext->setEnabled(!textEmpty && findAndReplaceDialog && findAndReplaceDialog->isVisible());
    ui->actionFindPrev->setEnabled(!textEmpty && findAndReplaceDialog && findAndReplaceDialog->isVisible());
}
/**
 * @brief MainWindow::cursorPosUpdate ���λ�ø���
 */
void MainWindow::cursorPosUpdate(){

    QTextCursor textCursor = ui->plainTextEdit->textCursor();

    QTextLayout* textLayout = textCursor.block().layout();

    int posInBlock = textCursor.position() - textCursor.block().position();
    int row = textLayout->lineForTextPosition(posInBlock).lineNumber() + textCursor.block().firstLineNumber();

    int col = textCursor.columnNumber();//������

    posLabel->setText(QString::fromLocal8Bit("�� ") + QString::number(row + 1) + QString::fromLocal8Bit(" �У��� ") + QString::number(col + 1) + QString::fromLocal8Bit(" ��"));
}
/**
 * @brief MainWindow::undoAction ����
 */
void MainWindow::undoAble(bool flag){

    ui->actionUndo->setEnabled(flag);
}
/**
 * @brief MainWindow::selectedUpdate ѡ������ݸ���
 */
void MainWindow::selectedUpdate(){

    bool selected = ui->plainTextEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(selected);
    ui->actionCopy->setEnabled(selected);
    ui->actionDelete->setEnabled(selected);
}
/**
 * @brief MainWindow::undoAction ����
 */
void MainWindow::undoAction(){
    ui->plainTextEdit->undo();
}
/**
 * @brief MainWindow::cutAction ����
 */
void MainWindow::cutAction(){
    ui->plainTextEdit->cut();
}
/**
 * @brief MainWindow::copyAction ����
 */
void MainWindow::copyAction(){
    ui->plainTextEdit->copy();
}
/**
 * @brief MainWindow::pasteAction ճ��
 */
void MainWindow::pasteAction(){
    ui->plainTextEdit->paste();
}
/**
 * @brief MainWindow::deleteAction ɾ��
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
 * @brief MainWindow::aboutAction ����
 */

void MainWindow::aboutAction(){
    QMessageBox::about(this, QString::fromLocal8Bit("����"), tr("qnotepad\n" "version1.0.0\n""simple notepad"));
}
/**
 * @brief MainWindow::fontAction �����ʽ
 */
void MainWindow::fontAction(){
    bool flag;
    QFont font = QFontDialog::getFont(&flag,ui->plainTextEdit->font(),this,QString::fromLocal8Bit("����"));

    if(!flag) return;
    ui->plainTextEdit->setFont(font);
    settings.setValue("font",font.toString());
}
/**
 * @brief MainWindow::printAction ��ӡ�����뱨��VS��ȱ����Ӧ�Ŀ��ļ�����pro�����QT+= printsupportѡ���
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
 * @brief MainWindow::helpAction ����
 */
void MainWindow::helpAction(){
    QDesktopServices::openUrl(QUrl("https://www.peiqiblog.com/article/4800/"));
}




