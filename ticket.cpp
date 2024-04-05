#include "ticket.h"

Ticket::Ticket() {}


Ticket::Ticket(int ticketID, QString brand, QString model, QString regID, QString description, Employee employee) : brand(brand), model(model), regID(regID), description(description), employeeInCharge(employee), ticketID(ticketID){
    state = "created";
    expectedCost = 0;
    estimateAccepted = false;
    amountPaidByClient = 0;
}

Ticket::Ticket(int ticketID, QString brand, QString model, QString regID, QString description, Employee employee, QString state, float expectedCost, bool estimateAccepted, float amountPaidByClient) :
    brand(brand), model(model), regID(regID), description(description), employeeInCharge(employee), state(state), expectedCost(expectedCost), estimateAccepted(estimateAccepted), amountPaidByClient(amountPaidByClient), ticketID(ticketID){

}


int Ticket::GetTicketID() const{
    return ticketID;
}
