#ifndef TICKET_H
#define TICKET_H
#include <QString>
#include <QSet>
#include <QPair>
#include "employee.h"
#include "timeslot.h"
#include "boughtpart.h"
class Ticket
{
public:
    Ticket();
    Ticket(int ticketID, QString brand, QString model, QString regID, QString description, Employee employee);
    Ticket(int ticketID, QString brand, QString model, QString regID, QString description, Employee employee, QString state, float expectedCost, bool estimateAccepted, float amountPaidByClient);
    int GetTicketID() const;

    QString brand;
    QString model;
    QString regID;
    QString description;
    Employee employeeInCharge;
    QList<TimeSlot> timeSlots;
    QString state;
    float expectedCost;
    bool estimateAccepted;
    QList<BoughtPart> boughtParts;
    float amountPaidByClient;

private:
    int ticketID;



};

#endif // TICKET_H
