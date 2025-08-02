#include "dlgprocess.h"
#include "ui_dlgprocess.h"

#include <QScrollBar>

DlgProcess::DlgProcess(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::DlgProcess)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &DlgProcess::processOutput);
    connect(m_process, &QProcess::readyRead, this, &DlgProcess::processOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &DlgProcess::processOutput);
    connect(m_process, &QProcess::errorOccurred, this, &DlgProcess::onError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DlgProcess::processFinished);


    m_process->setWorkingDirectory(QCoreApplication::applicationDirPath());
    m_process->start(".\\screenshot_tool.exe", {path});
}

DlgProcess::~DlgProcess()
{
    disconnect(m_process, &QProcess::readyReadStandardOutput, this, &DlgProcess::processOutput);
    disconnect(m_process, &QProcess::readyRead, this, &DlgProcess::processOutput);
    disconnect(m_process, &QProcess::readyReadStandardError, this, &DlgProcess::processOutput);
    disconnect(m_process, &QProcess::errorOccurred, this, &DlgProcess::onError);
    disconnect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DlgProcess::processFinished);

    m_process->kill();
    m_process->waitForFinished(-1);

    delete m_process;
    delete ui;
}

void DlgProcess::on_buttonBox_rejected()
{

}

QProcess *DlgProcess::process() const
{
    return m_process;
}

void DlgProcess::setProcess(QProcess *process)
{
    m_process = process;
}

void DlgProcess::clearOutput()
{
    ui->outputText->clear();
}

void DlgProcess::output(QString str)
{
    ui->outputText->append(str);

    // Автопрокрутка
    QScrollBar *sb = ui->outputText->verticalScrollBar();
    sb->setValue(sb->maximum());
}



void DlgProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    output("\nПроцесс завершен с кодом: " + QString::number(exitCode));
}

void DlgProcess::onError(QProcess::ProcessError error)
{
    output("\nОшибка. ProcessError: " + QString::number(error));
}

QList<QPair<QString, int> > DlgProcess::watches() const
{
    return m_watches;
}

QList<QString> DlgProcess::errorsUrl() const
{
    return m_errorsUrl;
}

QString DlgProcess::currentUrl() const
{
    return m_currentUrl;
}

void DlgProcess::processOutput()
{
    // Чтение вывода процесса
    QByteArray output = m_process->readAllStandardOutput();
    QByteArray error = m_process->readAllStandardError();
    if (!output.isEmpty()) {
        QString s_output = QString::fromLocal8Bit(output);
        this->output(s_output);

        // Проверка на сообщения об ошибках

        if (s_output.contains("Прогресс: ")) {
            int start = s_output.indexOf("Прогресс: ") + 10;
            int end = s_output.indexOf("%", start);
            QString s_progress = s_output.mid(start, end - start).trimmed();
            bool isOk = false;
            double progress = s_progress.toDouble(&isOk);
            if (isOk) {
                ui->progressBar->setValue(progress);
            }
        }
        if (s_output.contains("Обработка URL: < ")) {
            int start = s_output.indexOf("Обработка URL: < ") + 17;
            int end = s_output.indexOf(" >", start);
            QString s_url = s_output.mid(start, end - start).trimmed();
            m_currentUrl = s_url;
        }
        if (s_output.contains("Просмотры: ")) {
            int start = s_output.indexOf("Просмотры: ") + 11;
            int end = s_output.indexOf("\n", start);
            QString s_watches = s_output.mid(start, end - start).trimmed();
            bool isOk = false;
            double watches = s_watches.toDouble(&isOk);
            if (isOk) {
                m_watches.append({m_currentUrl, qRound(watches)});
            }
            else {
                m_watches.append({m_currentUrl, -1});
            }
        }
    }

    if (!error.isEmpty()) {
        QString s_error = QString::fromLocal8Bit(error);
        this->output("<font color='red'>" + s_error + "</font>");
        if (s_error.contains("Ошибка при обработке URL ")) {
            int start = s_error.indexOf("Ошибка при обработке URL ") + 25;
            int end = s_error.indexOf(" :", start);
            QString url = s_error.mid(start, end - start).trimmed();
            m_errorsUrl.append(url);
        }
    }
}
