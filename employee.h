#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include <QString>
class Employee
{
public:
    Employee();
    Employee(QString name, int id, float salary);

    QString name;
    int id;
    float salary;

};

#endif // EMPLOYEE_H
