#ifndef TICKETADDDIALOG_H
#define TICKETADDDIALOG_H

#include "qevent.h"
#include <QDialog>
#include "qlistwidget.h"
#include "ticket.h"
#include "timeslot.h"
namespace Ui {
class TicketAddDialog;
}

class TicketAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TicketAddDialog(QWidget *parent = nullptr, QList<Employee>* employeeList = nullptr, QList<TimeSlot>* timeSlotList = nullptr, QList<Ticket>* ticketList = nullptr);
    ~TicketAddDialog();
    void SetupEmployeeListWidget();

protected:

    void closeEvent(QCloseEvent *event) override {

        emit windowClosing();
        event->accept();

    }


signals:
    void windowClosing();
    void dataEntered(const Ticket &t);

private slots:


    void on_employees_itemClicked(QListWidgetItem *item);

     void on_calendar_cellClicked(int row, int column);

    void on_createTicketButton_clicked();

private:
    bool verifyData();
    Ui::TicketAddDialog *ui;
    Employee* chosenEmployee = nullptr;
    int ticketID;
    QList<Employee>* employees;
    QList<Ticket>* tickets;
    QList<TimeSlot>* timeSlots;
    QList<TimeSlot> chosenSlots;
    QList<TimeSlot> disabledSlots;

};

#endif // TICKETADDDIALOG_H
