#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include "employee.h"
#include "ticket.h"
#include "ticketinfodialog.h"
#include "timeslot.h"
#include "employeeadddialog.h"
#include "ticketadddialog.h"
#include "ticketeditdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addEmployeeButton_clicked();

    void on_createTicketButton_clicked();

    void on_editTicketButton_clicked();



    void on_employeeList_itemClicked(QListWidgetItem *item);

    void on_removeEmployeeButton_clicked();


    void on_ticketList_itemClicked(QListWidgetItem *item);

    void on_infoTicketButton_clicked();

    void on_saveScheduleButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    QList<Employee> employeeList;
    Employee* chosenEmployee = nullptr;
    int chosenEmployeeIndex;

    QList<Ticket> ticketList;
    Ticket* chosenTicket = nullptr;
    int chosenTicketIndex;
    QList<TimeSlot> allOccupiedSlots;

    //input forms
    employeeAddDialog* newEmpForm = nullptr;
    TicketAddDialog* newTicketForm = nullptr;
    TicketEditDialog* editTicketForm = nullptr;
    TicketInfoDialog* infoTicketForm = nullptr;

    //new employee form
    void onNewEmpFormDestroyed();
    void handleNewEmpData(const Employee &employee);
    void setupEmployeeListWidget();
    bool removeEmployee(int id);

    //new ticket form
    void onNewTicketFormDestroyed();
    void handleNewTicketData(const Ticket& ticket);
    void loadPartsToTickets();


    //edit ticket form
    void onEditTicketFormDestroyed();
    void handleEditTicketData(const Ticket& ticket);


    //info ticket form
    void onInfoTicketFormDestroyed();


    void setupTicketListWidget();


    void closeEvent(QCloseEvent *event) override {
        if(newEmpForm) newEmpForm->close();
        if(newTicketForm) newTicketForm->close();
        if(editTicketForm) editTicketForm->close();
        if(infoTicketForm) infoTicketForm->close();
        event->accept();
    }

    //loading from database
    void loadEmployees();
    void loadTimeSlots();
    void loadTickets();

};
#endif // MAINWINDOW_H
