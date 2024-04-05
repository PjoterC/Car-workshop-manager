#include "timeslot.h"

TimeSlot::TimeSlot() {}

TimeSlot::TimeSlot(int tID, int eID, int d, int h) : ticketID(tID), employeeID(eID), day(d), hour(h) {}


bool operator==(const TimeSlot& lhs, const TimeSlot& rhs)
{

    return lhs.ticketID == rhs.ticketID &&
           lhs.employeeID == rhs.employeeID &&
           lhs.day == rhs.day &&
           lhs.hour == rhs.hour;
}
