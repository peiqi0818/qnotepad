#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QFileDialog>
#include <QSettings>
#include <QTextBlock>
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QLabel>
#include <QFontDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QDesktopServices>

#include "findAndReplaceDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool isSaved();//是否保存
    bool isUpdate();//是否修改
    void resetWindowTitle();//重置窗口标题
    void openFile(QString filePath);//打开文件

    void newFindDialog();//创建查找对话框

    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    QString filePath_;//文件路径
    QString fileName_;//文件名
    QString savedFileData;//已经保存的文件内容
    QSettings settings;//会话设置
    QFont font;//字体
    QLabel* posLabel;//位置标签

private:
    Ui::MainWindow *ui;

    FindAndReplaceDialog* findAndReplaceDialog = nullptr;

private slots:
    /**文件*/
    void openFileAction();
    bool saveFileAction();
    void exitAction();
    void newFileAction();
    void saveAsAction();
    void printAction();

    /**编辑*/
    void findAction();//查找
    void findNextAction();
    void findPrevAction();
    void replaceAction();
    void textEditUpdate();//文本更新
    void cursorPosUpdate();//指针位置更新
    void undoAble(bool flag);//撤销
    void selectedUpdate();//选择的内容更新

    void undoAction();//撤销
    void cutAction();//剪切
    void copyAction();//复制
    void pasteAction();//粘贴
    void deleteAction();//删除

    /**格式字体*/
    void fontAction();//字体格式

    /**关于*/
    void aboutAction();

    /**帮助文档*/
    void helpAction();

};
#endif // MAINWINDOW_H




