#ifndef EMPLOYEEADDDIALOG_H
#define EMPLOYEEADDDIALOG_H

#include "qevent.h"
#include <QDialog>
#include "employee.h"

namespace Ui {
class employeeAddDialog;
}

class employeeAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit employeeAddDialog(QWidget *parent = nullptr, QList<Employee>* employeeList = nullptr);
    ~employeeAddDialog();

protected:

    void closeEvent(QCloseEvent *event) override {
        // Emit a custom signal before closing
        emit windowClosing();
        event->accept(); // or event->ignore(); if you want to prevent closing

    }


signals:
    void windowClosing();
    void dataEntered(const Employee &emp);

private slots:
    void verifyAndSendData();

    void on_sendDataButton_clicked();

private:
    Ui::employeeAddDialog *ui;
    QList<Employee> employees;
};

#endif // EMPLOYEEADDDIALOG_H
