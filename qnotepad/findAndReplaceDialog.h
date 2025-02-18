#ifndef FINDANDREPLACEDIALOG_H
#define FINDANDREPLACEDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class FindAndReplaceDialog;
}

class FindAndReplaceDialog : public QDialog{

    Q_OBJECT

public:
    FindAndReplaceDialog(QSettings& settings,QWidget *parent = nullptr);
    ~FindAndReplaceDialog();

    void openFindOrReplaceDialog(bool isReplace);

    QString getFindTextData();
    QString getReplaceTextData();
    bool isCaseSensitive();
    bool isLoop();

signals:
    void signalShow();
    void signalClose();
    void signalFindPrev();
    void signalFindNext();
    void signalReplceNext();
    void signalReplaceAll();


private slots:
   void findNextSlots();
   void replaceSlots();
   void replaceAllSlots();
   void cancelSlots();
   void caseSensitiveSlots();
   void loopSlots();
   void upSlots();
   void downSlots();
   void findEditSlots();
   void replaceEditSlots();

private:
    Ui::FindAndReplaceDialog *ui;
    QSettings& settings;
};

#endif // FINDANDREPLACEDIALOG_H






