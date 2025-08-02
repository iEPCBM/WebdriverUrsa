#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include "dlgviews.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Загрузка ссылок
    loadLinks();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_saveButton_clicked()
{
    saveLinks();
    QMessageBox::information(this, "Сохранено", "Ссылки успешно сохранены!");
}

void MainWindow::on_addButton_clicked()
{
    bool ok;
    QString url = QInputDialog::getText(this, "Добавить ссылку",
                                       "Введите URL:", QLineEdit::Normal, "", &ok);
    if (ok && !url.isEmpty()) {
        ui->listWidget->addItem(url);
    }
}

void MainWindow::on_editButton_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) return;

    bool ok;
    QString newUrl = QInputDialog::getText(this, "Редактировать ссылку",
                                         "Введите новый URL:", QLineEdit::Normal,
                                         item->text(), &ok);
    if (ok && !newUrl.isEmpty()) {
        item->setText(newUrl);
    }
}

void MainWindow::on_removeButton_clicked()
{
    delete ui->listWidget->currentItem();
}

void MainWindow::on_runButton_clicked()
{
    // Сохраняем текущий список
    saveLinks();

    // Очищаем предыдущие пометки об ошибках
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        ui->listWidget->item(i)->setBackground(Qt::white);
    }

    // Очищаем вывод
    QString dir = QFileDialog::getExistingDirectory(this, "Выбор папки сохранения скриншотов...", QApplication::applicationDirPath());
    if (dir.isNull())
        return;
    DlgProcess dlgProc(this, dir);
    dlgProc.exec();

    auto l_errors = dlgProc.errorsUrl();

    for (auto errorUrl : l_errors) {
        markErrorUrl(errorUrl);
    }

    if (l_errors.size()>0) {
        QMessageBox::warning(this, "Имеются ошибки при обработке ссылок", "Имеются ошибки при обработке ссылок. Такие ссылки окрасятся крысным цветом");
    }

    DlgViews dlgViews(dlgProc.watches(), this);
    dlgViews.exec();
    //dlgProc.clearOutput();

    // Запускаем Python-утилиту
}

void MainWindow::markErrorUrl(const QString &url)
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        if (item->text() == url) {
            item->setBackground(Qt::red);
            break;
        }
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    bool ok;
    QString newUrl = QInputDialog::getText(this, "Редактировать ссылку",
                                         "Введите новый URL:", QLineEdit::Normal,
                                         item->text(), &ok);
    if (ok && !newUrl.isEmpty()) {
        item->setText(newUrl);
    }
}

void MainWindow::saveLinks()
{
    QFile file("links.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл!");
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        out << ui->listWidget->item(i)->text() << "\n";
    }
    file.close();
}

void MainWindow::loadLinks()
{
    QFile file("links.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            ui->listWidget->addItem(line);
        }
    }
    file.close();
}

