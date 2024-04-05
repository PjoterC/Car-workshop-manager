#ifndef PARTADDDIALOG_H
#define PARTADDDIALOG_H

#include <QDialog>
#include "boughtpart.h"
namespace Ui {
class PartAddDialog;
}

class PartAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PartAddDialog(QWidget *parent = nullptr, int ticketid = 0);
    ~PartAddDialog();


signals:
    void dataEntered(const BoughtPart &bp);

private slots:
    bool verifyAndSendData();

    void on_addButton_clicked();

private:
    Ui::PartAddDialog *ui;
    int ticketid;
};

#endif // PARTADDDIALOG_H
