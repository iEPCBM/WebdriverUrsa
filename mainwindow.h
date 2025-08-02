#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProcess>

#include "dlgprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_saveButton_clicked();

    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_removeButton_clicked();

    void on_runButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    void saveLinks();
    void loadLinks();

    void markErrorUrl(const QString &url);
};
#endif // MAINWINDOW_H
