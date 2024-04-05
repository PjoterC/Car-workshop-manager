#include "ticketadddialog.h"
#include "ui_ticketadddialog.h"

TicketAddDialog::TicketAddDialog(QWidget *parent, QList<Employee>* employeeList, QList<TimeSlot>* timeSlotList, QList<Ticket>* ticketList)
    : QDialog(parent)
    , ui(new Ui::TicketAddDialog)
{
    employees = employeeList;
    timeSlots = timeSlotList;
    tickets = ticketList;

    //generate id for new ticket
    int currHighestID = 0;
    for(const Ticket& i : *tickets){
        if(i.GetTicketID() >= currHighestID){
            currHighestID = i.GetTicketID();
        }
    }
    ticketID = currHighestID + 1;



    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Ticket creation form");


    //turn off labels
    ui->bEmptyLabel->setVisible(false);
    ui->rEmptyLabel->setVisible(false);
    ui->mEmptyLabel->setVisible(false);


    //set calendar flags
    for (int row = 0; row < ui->calendar->rowCount(); ++row) {
        for (int column = 0; column < ui->calendar->columnCount(); ++column) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); \
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            ui->calendar->setItem(row, column, item);
        }
    }



    SetupEmployeeListWidget();
}

TicketAddDialog::~TicketAddDialog()
{
    delete ui;
}


bool TicketAddDialog::verifyData(){
    bool valid = true;
    if(ui->brandEdit->text() == ""){
        ui->bEmptyLabel->setVisible(true);
        valid = false;
    }
    if(ui->modelEdit->text() == ""){
        ui->mEmptyLabel->setVisible(true);
        valid = false;
    }
    if(ui->regIDEdit->text() == ""){
        ui->rEmptyLabel->setVisible(true);
        valid = false;
    }
    if(chosenSlots.empty()){
        ui->timeSelectLabel->setText("You have to select time of repair!");
        valid = false;
    }
    return valid;
}


//fills the list widget with employees
void TicketAddDialog::SetupEmployeeListWidget(){
    QListWidget* listWidget = ui->employees;
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->clear();
    for(const Employee &entry : *employees){
        QString label("Name: "+entry.name+" Salary: "+QString::number(entry.salary)+" ID: "+QString::number(entry.id));
        listWidget->addItem(label);
    }
}



void TicketAddDialog::on_employees_itemClicked(QListWidgetItem *item)
{
    //clear data from previous chosen employee
    for(TimeSlot i : disabledSlots){
        QTableWidgetItem *item = ui->calendar->item(i.hour, i.day);
        item->setBackground(Qt::white);
    }
    for(TimeSlot j : chosenSlots){
        QTableWidgetItem *item = ui->calendar->item(j.hour, j.day);
        item->setBackground(Qt::white);
    }
    disabledSlots.clear();
    chosenSlots.clear();

    int chosenEmployeeIndex = ui->employees->row(item);
    chosenEmployee = (Employee*)&employees->at(chosenEmployeeIndex);




    for(TimeSlot t : *timeSlots){
        if(t.employeeID == chosenEmployee->id){
            disabledSlots.append(t);
            QTableWidgetItem *item = ui->calendar->item(t.hour, t.day);

            item->setBackground(Qt::gray);
        }
    }
    ui->timeSelectLabel->setText("Choose when the repair will take place:");

}



void TicketAddDialog::on_calendar_cellClicked(int row, int column)
{

    if(chosenEmployee == nullptr){
        ui->timeSelectLabel->setText("Select employee first!");
        return;
    }
    QTableWidgetItem *item = ui->calendar->item(row, column);
    TimeSlot slot(ticketID, chosenEmployee->id, column, row);
    for(TimeSlot t : disabledSlots){
        if(t.hour == row && t.day == column){
            return;
        }
    }
    if(chosenSlots.contains(slot)){
        chosenSlots.removeOne(slot);
        item->setBackground(Qt::white);

        return;
    }

    item->setBackground(Qt::yellow);
    chosenSlots.append(slot);




}


void TicketAddDialog::on_createTicketButton_clicked()
{
    if(!verifyData()){
        return;
    }
    else{

        Ticket t(ticketID, ui->brandEdit->text(), ui->modelEdit->text(), ui->regIDEdit->text(), ui->description->toPlainText(), *chosenEmployee);
        t.timeSlots = chosenSlots;
        emit dataEntered(t);
        close();

    }
}

