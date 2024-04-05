#ifndef TICKETINFODIALOG_H
#define TICKETINFODIALOG_H

#include "qevent.h"
#include "qtablewidget.h"
#include "ticket.h"
#include <QDialog>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>


namespace Ui {
class TicketInfoDialog;
}

class TicketInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TicketInfoDialog(QWidget *parent = nullptr, const Ticket* ticket = nullptr, const QList<TimeSlot>* timeslots = nullptr, const QList<Employee>* employees = nullptr,
                              QTableWidget* table = nullptr);
    ~TicketInfoDialog();


protected:

    void closeEvent(QCloseEvent *event) override {
        emit windowClosing();
        event->accept();

    }


signals:
    void windowClosing();


private slots:
    void on_estimateButton_clicked();

    void on_costButton_clicked();

private:
    Ui::TicketInfoDialog *ui;
    const Ticket* t;
    const QList<TimeSlot>* ts;
    const QList<Employee>* emps;
    QTableWidget* calendar;
};

#endif // TICKETINFODIALOG_H
