#ifndef DLGPROCESS_H
#define DLGPROCESS_H

#include <QDialog>
#include <QProcess>


namespace Ui {
class DlgProcess;
}

class DlgProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DlgProcess(QWidget *parent = nullptr, QString path = "");
    ~DlgProcess();

    QProcess *process() const;
    void setProcess(QProcess *process);

    void clearOutput();
    void output(QString str);

    QString currentUrl() const;

    QList<QString> errorsUrl() const;

    QList<QPair<QString, int> > watches() const;

private slots:
    void on_buttonBox_rejected();

    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);

private:
    Ui::DlgProcess *ui;
    QProcess* m_process;
    QList<QPair<QString, int>> m_watches;
    QList<QString> m_errorsUrl;
    QString m_currentUrl = "";
};

#endif // DLGPROCESS_H
