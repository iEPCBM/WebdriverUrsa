#include "dlgviews.h"
#include "ui_dlgviews.h"

DlgViews::DlgViews(QList<QPair<QString, int> > data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgViews),
    m_data(data)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    updateTable();
}

DlgViews::~DlgViews()
{
    delete ui;
}

QList<QPair<QString, int> > DlgViews::data() const
{
    return m_data;
}

void DlgViews::setData(const QList<QPair<QString, int> > &data)
{
    m_data = data;
    updateTable();
}

void DlgViews::updateTable()
{
    ui->tblViews->clearContents();
    ui->tblViews->setRowCount(m_data.size()+1);
    int sum = 0;
    for (int row = 0; row < ui->tblViews->rowCount()-1; ++row) {
        QString cellData = QString::number(row+1);
        QTableWidgetItem *newItem = new QTableWidgetItem(cellData);
        ui->tblViews->setItem(row, 0, newItem);

        cellData = QString(m_data[row].first);
        newItem = new QTableWidgetItem(cellData);
        ui->tblViews->setItem(row, 1, newItem);

        cellData = QString::number(m_data[row].second);
        newItem = new QTableWidgetItem(cellData);
        ui->tblViews->setItem(row, 2, newItem);
        sum += m_data[row].second;
    }

    int last_row = ui->tblViews->rowCount()-1;

    QTableWidgetItem* newItem = new QTableWidgetItem("Итог:");
    ui->tblViews->setItem(last_row, 1, newItem);

    newItem = new QTableWidgetItem(QString::number(sum));
    ui->tblViews->setItem(last_row, 2, newItem);

    ui->tblViews->resizeColumnsToContents();
}
