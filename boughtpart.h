#ifndef BOUGHTPART_H
#define BOUGHTPART_H

#include <QString>
class BoughtPart
{
public:
    BoughtPart();
    BoughtPart(int tID, QString n, float a, float p);

    int ticketID;
    QString name;
    float amount;
    float price;
    float totalPrice;


    float calculateTotalPrice();


};

#endif // BOUGHTPART_H
