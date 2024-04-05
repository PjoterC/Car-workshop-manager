#ifndef TIMESLOT_H
#define TIMESLOT_H

class TimeSlot
{
public:
    TimeSlot();
    TimeSlot(int tID, int eID, int d, int h);
    friend bool operator==(const TimeSlot& lhs, const TimeSlot& rhs);
    int ticketID;
    int employeeID;
    int day;
    int hour;
};

#endif // TIMESLOT_H
