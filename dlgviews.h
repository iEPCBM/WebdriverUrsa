#ifndef DLGVIEWS_H
#define DLGVIEWS_H

#include <QDialog>

namespace Ui {
class DlgViews;
}

class DlgViews : public QDialog
{
    Q_OBJECT

public:
    explicit DlgViews(QList<QPair<QString, int>> data, QWidget *parent = nullptr);
    ~DlgViews();

    QList<QPair<QString, int> > data() const;
    void setData(const QList<QPair<QString, int> > &data);

    void updateTable();

private:
    Ui::DlgViews *ui;
    QList<QPair<QString, int>> m_data;
};

#endif // DLGVIEWS_H
