#include "boughtpart.h"

BoughtPart::BoughtPart() {}

BoughtPart::BoughtPart(int tID, QString n, float a, float p) : ticketID(tID), name(n), amount(a), price(p) {
    totalPrice = calculateTotalPrice();
}


float BoughtPart::calculateTotalPrice(){
    return amount*price;
}
