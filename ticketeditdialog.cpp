#include "ticketeditdialog.h"
#include "ui_ticketeditdialog.h"

TicketEditDialog::TicketEditDialog(QWidget *parent, const Ticket* ticket, const QList<Employee>* employeeList, const QList<TimeSlot>* timeSlotList)
    : QDialog(parent), ui(new Ui::TicketEditDialog), ticketToEdit(ticket), employees(employeeList)
    , timeSlots(timeSlotList)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Ticket editing form");

    //set calendar flags
    for (int row = 0; row < ui->calendar->rowCount(); ++row) {
        for (int column = 0; column < ui->calendar->columnCount(); ++column) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); \
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            ui->calendar->setItem(row, column, item);
        }
    }


    ui->brandLabel->setText("Brand: "+ticket->brand);
    ui->modelLabel->setText("Model: "+ticket->model);
    ui->regIDLabel->setText("RegID: "+ticket->regID);
    ui->descriptionEdit->setText(ticket->description);
    int statusindex = ui->statusBox->findText(ticket->state);
    ui->statusBox->setCurrentIndex(statusindex);
    ui->costEdit->setText(QString::number(ticket->expectedCost));
    ui->clientPaidEdit->setText(QString::number(ticket->amountPaidByClient));
    if(ticket->estimateAccepted){
        ui->acceptedBox->setCurrentIndex(1);
    }
    else{
        ui->acceptedBox->setCurrentIndex(0);
    }



    for(TimeSlot t : *timeSlots){
        if(t.ticketID == ticketToEdit->GetTicketID()){
            allEmployeesChosenSlots.append(t);

        }
    }
    for(const BoughtPart& bp : ticketToEdit->boughtParts){
        parts.append(bp);
    }




    SetupEmployeeListWidget();
    SetupPartsListWidget();
}

TicketEditDialog::~TicketEditDialog()
{
    delete ui;
}



void TicketEditDialog::SetupEmployeeListWidget(){
    QListWidget* listWidget = ui->employees;
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->clear();
    for(const Employee &entry : *employees){
        QString label("Name: "+entry.name+" Salary: "+QString::number(entry.salary)+" ID: "+QString::number(entry.id));
        listWidget->addItem(label);
    }
}

void TicketEditDialog::SetupPartsListWidget(){
    QListWidget* listWidget = ui->partsList;
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->clear();
    for(const BoughtPart &entry : ticketToEdit->boughtParts){
        QString label("Item: "+entry.name+" Amount: "+QString::number(entry.amount)+" Price each: $"+QString::number(entry.price)+" Total: $"+QString::number(entry.totalPrice));
        listWidget->addItem(label);
    }
}






void TicketEditDialog::on_employees_itemClicked(QListWidgetItem *item)
{
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



    for(TimeSlot s : *timeSlots){
        if(s.employeeID == chosenEmployee->id && s.ticketID != ticketToEdit->GetTicketID()){
            disabledSlots.append(s);

            QTableWidgetItem *item = ui->calendar->item(s.hour, s.day);
            item->setBackground(Qt::gray);
        }
    }

    for(TimeSlot t : allEmployeesChosenSlots){
        if(t.employeeID == chosenEmployee->id && t.ticketID == ticketToEdit->GetTicketID()){
            chosenSlots.append(t);

            QTableWidgetItem *item = ui->calendar->item(t.hour, t.day);
            item->setBackground(Qt::yellow);
        }
    }

}


void TicketEditDialog::on_addPartsButton_clicked()
{

        partAddForm = new PartAddDialog(nullptr, ticketToEdit->GetTicketID());
        connect(partAddForm, &PartAddDialog::dataEntered, this, &TicketEditDialog::addPart);
        partAddForm->exec();

}



void TicketEditDialog::addPart(const BoughtPart& bp){
    parts.append(bp);
    QString label("Item: "+bp.name+" Amount: "+QString::number(bp.amount)+" Price each: $"+QString::number(bp.price)+" Total: $"+QString::number(bp.totalPrice));
    ui->partsList->addItem(label);
}






void TicketEditDialog::on_calendar_cellClicked(int row, int column)
{
    if(chosenEmployee == nullptr){
        return;
    }
    QTableWidgetItem *item = ui->calendar->item(row, column);
    TimeSlot slot(ticketToEdit->GetTicketID(), chosenEmployee->id, column, row);

    for(TimeSlot t : disabledSlots){
        if(t.hour == row && t.day == column){
            return;
        }
    }
    if(allEmployeesChosenSlots.contains(slot)){

        allEmployeesChosenSlots.removeOne(slot);
        item->setBackground(Qt::white);

        return;
    }

    item->setBackground(Qt::yellow);
    chosenSlots.append(slot);
    allEmployeesChosenSlots.append(slot);

}

bool TicketEditDialog::verifyData(){
    bool valid = true;
    if(ui->costEdit->text().toFloat() <= 0 && ui->costEdit->text() != "0"){
        valid = false;
        ui->costEdit->setText("0");
    }
    if(ui->clientPaidEdit->text().toFloat() <= 0 && ui->clientPaidEdit->text() != "0"){
        valid = false;
        ui->clientPaidEdit->setText("0");
    }
    return valid;
}






void TicketEditDialog::on_saveButton_clicked()
{
    if(!verifyData()){
        return;
    }
    bool eaccepted = false;
    if(ui->acceptedBox->currentIndex() == 1){
        eaccepted = true;
    }

    Ticket t(ticketToEdit->GetTicketID(), ticketToEdit->brand, ticketToEdit->model, ticketToEdit->regID, ui->descriptionEdit->toPlainText(), ticketToEdit->employeeInCharge,
             ui->statusBox->currentText(), ui->costEdit->text().toFloat(), eaccepted, ui->clientPaidEdit->text().toFloat());

    t.timeSlots = allEmployeesChosenSlots;
    t.boughtParts = parts;

    emit dataEntered(t);
    close();

}


void TicketEditDialog::on_removePartsButton_clicked()
{
    if(chosenPart){
        parts.removeAt(chosenPartIndex);
        QListWidgetItem *item = ui->partsList->takeItem(chosenPartIndex);
        delete item;
    }
}


void TicketEditDialog::on_partsList_itemClicked(QListWidgetItem *item)
{

    chosenPartIndex = ui->partsList->row(item);
    chosenPart = &parts[chosenPartIndex];
}

