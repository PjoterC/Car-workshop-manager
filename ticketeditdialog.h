#ifndef TICKETEDITDIALOG_H
#define TICKETEDITDIALOG_H

#include "qevent.h"
#include <QDialog>
#include "qlistwidget.h"
#include "ticket.h"
#include "partadddialog.h"
namespace Ui {
class TicketEditDialog;
}

class TicketEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TicketEditDialog(QWidget *parent = nullptr, const Ticket* ticket = nullptr, const QList<Employee>* employeeList = nullptr, const QList<TimeSlot>* timeSlotList = nullptr);
    ~TicketEditDialog();

    void SetupEmployeeListWidget();
    void SetupPartsListWidget();

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

    void on_addPartsButton_clicked();

    void addPart(const BoughtPart& bp);

    void on_calendar_cellClicked(int row, int column);

    void on_saveButton_clicked();

    void on_removePartsButton_clicked();

    void on_partsList_itemClicked(QListWidgetItem *item);

private:
    Ui::TicketEditDialog *ui;
    const Ticket* ticketToEdit;
    Employee* chosenEmployee = nullptr;
    const QList<Employee>* employees;
    QList<BoughtPart> parts;
    BoughtPart* chosenPart = nullptr;
    int chosenPartIndex;
    const QList<TimeSlot>* timeSlots;
    QList<TimeSlot> chosenSlots;
    QList<TimeSlot> allEmployeesChosenSlots;
    QList<TimeSlot> disabledSlots;

    PartAddDialog* partAddForm = nullptr;


    bool verifyData();


};

#endif // TICKETEDITDIALOG_H
