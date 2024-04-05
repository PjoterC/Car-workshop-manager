#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Car workshop manager");
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("carworkshop.db");

    if (!QFile::exists("carworkshop.db")){


        if (!db.open()) {
            qDebug() << "Error: Failed to open database";
            return;
        }

        QSqlQuery query;
        if (!query.exec("CREATE TABLE employees (id INTEGER PRIMARY KEY, name TEXT, salary REAL)")) {
            qDebug() << "Error executing query:" << query.lastError().text();
        }
        QSqlQuery query2;
        if (!query2.exec("CREATE TABLE tickets (id INTEGER PRIMARY KEY, brand TEXT, model TEXT, regid TEXT, description TEXT, empid INTEGER, state TEXT, cost REAL, estimateaccepted BIT, paidbyclient REAL)")) {
            qDebug() << "Error executing query:" << query2.lastError().text();
        }
        QSqlQuery query3;
        if (!query3.exec("CREATE TABLE timeslots (ticketid INTEGER, employeeid INTEGER, day INTEGER, hour INTEGER)")) {
            qDebug() << "Error executing query:" << query3.lastError().text();
        }
        QSqlQuery query4;
        if (!query4.exec("CREATE TABLE boughtparts (ticketid INTEGER, item TEXT, amount REAL, price REAL)")) {
            qDebug() << "Error executing query:" << query4.lastError().text();
        }

    }
    db.close();

    loadEmployees();

    setupEmployeeListWidget();

    loadTimeSlots();
    loadTickets();
    loadPartsToTickets();

    setupTicketListWidget();



    ui->selectEmpLabel->setVisible(false);
    ui->selectTicketLabel->setVisible(false);


    for (int row = 0; row < ui->calendar->rowCount(); ++row) {
        for (int column = 0; column < ui->calendar->columnCount(); ++column) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            ui->calendar->setItem(row, column, item);
        }
    }



}

MainWindow::~MainWindow()
{
    delete ui;
}



//EMPLOYEES


void MainWindow::on_addEmployeeButton_clicked()
{
    if(editTicketForm || newTicketForm || infoTicketForm){
        return;
    }
    ui->selectEmpLabel->setVisible(false);
    ui->selectTicketLabel->setVisible(false);
    if (newEmpForm == nullptr) {
        newEmpForm = new employeeAddDialog(nullptr, &employeeList);
        connect(newEmpForm, &employeeAddDialog::windowClosing, this, &MainWindow::onNewEmpFormDestroyed);
        connect(newEmpForm, &employeeAddDialog::dataEntered, this, &MainWindow::handleNewEmpData);

        newEmpForm->show();
    } else {
        newEmpForm->raise();

    }
}

void MainWindow::onNewEmpFormDestroyed(){
    newEmpForm = nullptr;

}

void MainWindow::handleNewEmpData(const Employee &data) {
    db.setDatabaseName("carworkshop.db");
    Employee emp(data.name, data.id, data.salary);



    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO employees (id, name, salary) VALUES (?, ?, ?)");
    query.addBindValue(data.id);
    query.addBindValue(data.name);
    query.addBindValue(data.salary);

    if (!query.exec()) {
        //qDebug() << "Error executing newemp query:" << query.lastError().text();
        return;
    }

    db.close();
    employeeList.append(emp);
    QString label("Name: "+emp.name+", Salary: $"+QString::number(emp.salary)+", ID: "+QString::number(emp.id));
    ui->employeeList->addItem(label);


}

void MainWindow::loadEmployees(){
    if (!db.open()) {
        qDebug() << "Error: Failed to open database";
        return;
    }

    QSqlQuery query;
    if(!query.exec("SELECT id, name, salary FROM employees")){
        qDebug() << "Error executing empload query" << query.lastError().text();
        return;
    }
    while (query.next()) {

        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        float salary = query.value(2).toFloat();
        Employee emp(name, id, salary);
        employeeList.append(emp);


    }
    db.close();
}



void MainWindow::setupEmployeeListWidget(){
    QListWidget* listWidget = ui->employeeList;
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->clear();
    for(const Employee &entry : employeeList){
        QString label("Name: "+entry.name+", Salary: $"+QString::number(entry.salary)+", ID: "+QString::number(entry.id));
        listWidget->addItem(label);
    }

}


void MainWindow::on_employeeList_itemClicked(QListWidgetItem *item)
{
    ui->selectEmpLabel->setText("Select employee first!");
    ui->selectTicketLabel->setVisible(false);
    ui->selectEmpLabel->setVisible(false);
    for (int row = 0; row < ui->calendar->rowCount(); ++row) {
        for (int column = 0; column < ui->calendar->columnCount(); ++column) {
            QTableWidgetItem *item = ui->calendar->item(row, column);
            item->setBackground(Qt::white);
        }
    }

    chosenEmployeeIndex = ui->employeeList->row(item);
    chosenEmployee = (Employee*)&employeeList.at(chosenEmployeeIndex);

    for(TimeSlot i : allOccupiedSlots){
        if(i.employeeID == chosenEmployee->id){
            QTableWidgetItem *item = ui->calendar->item(i.hour, i.day);
            item->setBackground(Qt::blue);
        }
    }

    ui->scheduleLabel->setText("Schedule: "+chosenEmployee->name+" (ID: "+QString::number(chosenEmployee->id)+")");
    chosenTicket = nullptr;

}


bool MainWindow::removeEmployee(int index){

    if(!chosenEmployee){
        return false;
    }
    for(const Ticket& t : ticketList){
        if(t.employeeInCharge.id == chosenEmployee->id && t.state != "closed"){
            ui->selectEmpLabel->setText("This employee is responsible for a ticket - can't be removed");
            return false;
        }
    }


    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return false;
    }
    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT * FROM employees WHERE id = ?");
    selectQuery.addBindValue(chosenEmployee->id);
    if (!selectQuery.exec()) {
        //qDebug() << "Error executing SELECT query:" << selectQuery.lastError().text();
        return false;
    }
    while (selectQuery.next()) {


        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM employees WHERE id = ?");
        deleteQuery.addBindValue(chosenEmployee->id);
        if (!deleteQuery.exec()) {
            //qDebug() << "Error executing DELETE query:" << deleteQuery.lastError().text();
            return false;
        }

    }






    QSqlQuery selectSlotQuery;
    selectSlotQuery.prepare("SELECT * FROM timeslots WHERE employeeid = ?");
    selectSlotQuery.addBindValue(chosenEmployee->id);
    if (!selectSlotQuery.exec()) {
        qDebug() << "Error executing SELECTSLOT query:" << selectQuery.lastError().text();
        return false;
    }
    while (selectSlotQuery.next()) {


        QSqlQuery deleteSlotQuery;
        deleteSlotQuery.prepare("DELETE FROM timeslots WHERE employeeid = ?");
        deleteSlotQuery.addBindValue(chosenEmployee->id);
        if (!deleteSlotQuery.exec()) {
            qDebug() << "Error executing DELETESLOT query:" << deleteSlotQuery.lastError().text();
            return false;
        }

    }

    for(int i = allOccupiedSlots.size()-1; i>=0; i--){
        if(allOccupiedSlots[i].employeeID == chosenEmployee->id){
            allOccupiedSlots.removeAt(i);
        }

    }

    for(Ticket &t : ticketList){
        if(t.employeeInCharge.id == chosenEmployee->id){
            Employee empty("No longer employed", 0, 0);
            t.employeeInCharge = empty;
        }
    }
    setupTicketListWidget();
    employeeList.remove(index);




return true;

}



void MainWindow::on_removeEmployeeButton_clicked()
{
    if(editTicketForm || newTicketForm || infoTicketForm){
        return;
    }
    ui->selectEmpLabel->setText("Select employee first!");
    ui->selectTicketLabel->setVisible(false);
    if(chosenEmployee != nullptr && removeEmployee(chosenEmployeeIndex)){
        ui->selectEmpLabel->setVisible(false);
        QListWidgetItem *item = ui->employeeList->takeItem(chosenEmployeeIndex);
        delete item;
    }
    else{
        ui->selectEmpLabel->setVisible(true);
    }
}



//TICKETS



void MainWindow::on_createTicketButton_clicked()
{
    ui->selectTicketLabel->setVisible(false);
    ui->selectEmpLabel->setVisible(false);
    if(newTicketForm == nullptr){
        newTicketForm = new TicketAddDialog(nullptr, &employeeList, &allOccupiedSlots, &ticketList);
        connect(newTicketForm, &TicketAddDialog::windowClosing, this, &MainWindow::onNewTicketFormDestroyed);
        connect(newTicketForm, &TicketAddDialog::dataEntered, this, &MainWindow::handleNewTicketData);

        newTicketForm->show();
    }
    else {
        newTicketForm->raise();
    }
}

void MainWindow::onNewTicketFormDestroyed(){
    newTicketForm = nullptr;

}


//correct after timeslots implemented
void MainWindow::handleNewTicketData(const Ticket &ticket)
{
    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO tickets (id, brand, model, regid, description, empid, state, cost, estimateaccepted, paidbyclient) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(ticket.GetTicketID());
    query.addBindValue(ticket.brand);
    query.addBindValue(ticket.model);
    query.addBindValue(ticket.regID);
    query.addBindValue(ticket.description);
    query.addBindValue(ticket.employeeInCharge.id);
    query.addBindValue(ticket.state);
    query.addBindValue(ticket.expectedCost);
    query.addBindValue(false);
    query.addBindValue(ticket.amountPaidByClient);

    if (!query.exec()) {
        //qDebug() << "Error executing newtickethandle query:" << query.lastError().text();
        return;
    }



    ticketList.append(ticket);
    QString label = "ID: "+QString::number(ticket.GetTicketID())+" Brand: "+ticket.brand+", Model: "+ticket.model+", RegID: "+ticket.regID+", Managing employee: "+ticket.employeeInCharge.name;
    ui->ticketList->addItem(label);

    for(TimeSlot i : ticket.timeSlots){
        allOccupiedSlots.append(i);
        QSqlQuery query2;
        query2.prepare("INSERT INTO timeslots (ticketid, employeeid, day, hour) VALUES (?, ?, ?, ?)");
        query2.addBindValue(i.ticketID);
        query2.addBindValue(i.employeeID);
        query2.addBindValue(i.day);
        query2.addBindValue(i.hour);
        if (!query2.exec()) {
            //qDebug() << "Error executing ticket insert query:" << query.lastError().text();
            return;
        }

    }

    db.close();
}


//correct after time slots implemented
void MainWindow::loadTickets(){
    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }
    QSqlQuery query;
    if(!query.exec("SELECT id, brand, model, regid, description, empid, state, cost, estimateaccepted, paidbyclient FROM tickets")){
        //qDebug() << "Error executing ticket load query" << query.lastError().text();
        return;
    }
    while (query.next()) {

        int id = query.value(0).toInt();
        QString brand = query.value(1).toString();
        QString model = query.value(2).toString();
        QString regid = query.value(3).toString();
        QString description = query.value(4).toString();
        int empid = query.value(5).toInt();
        Employee* emp = nullptr;
        for(Employee& e : employeeList){
            if(e.id == empid){
                emp = &e;

            }
        }

        QString state = query.value(6).toString();
        float cost = query.value(7).toFloat();
        bool estimateaccepted = query.value(8).toBool();
        float paidbyclient = query.value(9).toFloat();
        Ticket t;
        if(emp){
            Ticket temp(id, brand, model, regid, description, *emp, state, cost, estimateaccepted, paidbyclient);
            t = temp;
        }
        else{
            Employee empty("No longer employed", 0, 0);
            Ticket temp(id, brand, model, regid, description, empty, state, cost, estimateaccepted, paidbyclient);
            t = temp;
        }
        for(TimeSlot ts : allOccupiedSlots){
            if(ts.ticketID == t.GetTicketID()){
                t.timeSlots.append(ts);
            }
        }



        ticketList.append(t);

    }


     db.close();
}


void MainWindow::loadPartsToTickets(){
    QSqlQuery query;
    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }

    if(!query.exec("SELECT ticketid, item, amount, price FROM boughtparts")){
        //qDebug() << "Error executing ticket load query" << query.lastError().text();
        return;
    }
    while (query.next()) {
        int id = query.value(0).toInt();

        QString item = query.value(1).toString();
        float amount = query.value(2).toFloat();
        float price = query.value(3).toFloat();
        BoughtPart bp(id, item, amount, price);

        for(Ticket& t : ticketList){
            if(t.GetTicketID() == id){
                t.boughtParts.append(bp);


            }
        }


    }

}



void MainWindow::setupTicketListWidget(){
    QListWidget* listWidget = ui->ticketList;
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->clear();
    for(const Ticket &ticket : ticketList){
        QString l = "";
        if(ticket.state == "closed"){
            l = "[CLOSED] ";
        }
        l += "ID: "+QString::number(ticket.GetTicketID())+" Brand: "+ticket.brand+", Model: "+ticket.model+", RegID: "+ticket.regID+", Managing employee: "+ticket.employeeInCharge.name;
        ui->ticketList->addItem(l);
    }
}





void MainWindow::on_editTicketButton_clicked()
{

    ui->selectEmpLabel->setVisible(false);
    ui->selectTicketLabel->setVisible(true);
    if(!chosenTicket){
        ui->selectTicketLabel->setText("Select a ticket first!");
        return;
    }
    if(chosenTicket->state == "closed"){
        ui->selectTicketLabel->setText("Closed tickets cannot be edited.");
        return;

    }
    ui->selectTicketLabel->setVisible(false);
    if(editTicketForm == nullptr){
        editTicketForm = new TicketEditDialog(nullptr, chosenTicket, &employeeList, &allOccupiedSlots);
        connect(editTicketForm, &TicketEditDialog::windowClosing, this, &MainWindow::onEditTicketFormDestroyed);
        connect(editTicketForm, &TicketEditDialog::dataEntered, this, &MainWindow::handleEditTicketData);

        editTicketForm->show();
    }
    else {
        editTicketForm->raise();

    }
}

void MainWindow::onEditTicketFormDestroyed(){
    editTicketForm = nullptr;

}



void MainWindow::handleEditTicketData(const Ticket &ticket)
{
    for(int i = 0; i < ticketList.size(); i++){
        if(ticketList[i].GetTicketID() == ticket.GetTicketID()){
            ticketList.removeAt(i);
            QListWidgetItem *item = ui->ticketList->takeItem(i);
            delete item;

        }
    }

    ticketList.append(ticket);

    QString label = "ID: "+QString::number(ticket.GetTicketID())+" Brand: "+ticket.brand+", Model: "+ticket.model+", RegID: "+ticket.regID+", Managing employee: "+ticket.employeeInCharge.name;
    if(ticket.state == "closed"){
        ui->ticketList->addItem("[CLOSED] "+label);
    }
    else{
        ui->ticketList->addItem(label);
    }



    for(int i = allOccupiedSlots.size()-1; i >= 0; i--){
        if(allOccupiedSlots[i].ticketID == ticket.GetTicketID()){
            allOccupiedSlots.removeAt(i);
        }
    }  
    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }
    QSqlQuery query;
    QString updateQuery = "UPDATE tickets SET description = :description, "
                          "state = :state, cost = :cost, estimateaccepted = :estimateaccepted, "
                          "paidbyclient = :paidbyclient WHERE id = :id";
    query.prepare(updateQuery);
    query.bindValue(":description", ticket.description);
    query.bindValue(":state", ticket.state);
    query.bindValue(":cost", ticket.expectedCost);
    query.bindValue(":estimateaccepted", ticket.estimateAccepted);
    query.bindValue(":paidbyclient", ticket.amountPaidByClient);
    query.bindValue(":id", ticket.GetTicketID());
    if (!query.exec()) {
        //qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }





    QSqlQuery selectSlotsQuery;
    selectSlotsQuery.prepare("SELECT * FROM timeslots WHERE ticketid = ?");
    selectSlotsQuery.addBindValue(ticket.GetTicketID());
    if (!selectSlotsQuery.exec()) {
        //qDebug() << "Error executing SELECT query:" << selectSlotsQuery.lastError().text();
        return;
    }
    while (selectSlotsQuery.next()) {
        //int id = selectQuery.value("id").toInt();

        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM timeslots WHERE ticketid = ?");
        deleteQuery.addBindValue(ticket.GetTicketID());
        if (!deleteQuery.exec()) {
            //qDebug() << "Error executing DELETE query:" << deleteQuery.lastError().text();
            return;
        }
    }




    QSqlQuery selectPartsQuery;
    selectPartsQuery.prepare("SELECT * FROM boughtparts WHERE ticketid = ?");
    selectPartsQuery.addBindValue(ticket.GetTicketID());
    if (!selectPartsQuery.exec()) {
        //qDebug() << "Error executing SELECTPARTS query:" << selectPartsQuery.lastError().text();
        return;
    }
    while (selectPartsQuery.next()) {


        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM boughtparts WHERE ticketid = ?");
        deleteQuery.addBindValue(ticket.GetTicketID());
        if (!deleteQuery.exec()) {
            //qDebug() << "Error executing DELETEPART query:" << deleteQuery.lastError().text();
            return;
        }

    }

    for(const BoughtPart& bp : ticket.boughtParts){
        QSqlQuery addBoughtPartQuery;
        addBoughtPartQuery.prepare("INSERT INTO boughtparts (ticketid, item, amount, price) VALUES (?, ?, ?, ?)");
        addBoughtPartQuery.addBindValue(bp.ticketID);
        addBoughtPartQuery.addBindValue(bp.name);
        addBoughtPartQuery.addBindValue(bp.amount);
        addBoughtPartQuery.addBindValue(bp.price);
        if (!addBoughtPartQuery.exec()) {
            //qDebug() << "Error executing part insert query:" << addBoughtPartQuery.lastError().text();
            return;
        }


    }



    if(ticket.state != "closed"){
    for(TimeSlot i : ticket.timeSlots){
        allOccupiedSlots.append(i);
        QSqlQuery addTimeslotQuery;
        addTimeslotQuery.prepare("INSERT INTO timeslots (ticketid, employeeid, day, hour) VALUES (?, ?, ?, ?)");
        addTimeslotQuery.addBindValue(i.ticketID);
        addTimeslotQuery.addBindValue(i.employeeID);
        addTimeslotQuery.addBindValue(i.day);
        addTimeslotQuery.addBindValue(i.hour);
        if (!addTimeslotQuery.exec()) {
            //qDebug() << "Error executing ticket insert query:" << addTimeslotQuery.lastError().text();
            return;
        }

    }



    }

    db.close();




}




void MainWindow::loadTimeSlots(){
    if (!db.open()) {
        //qDebug() << "Error: Failed to open database";
        return;
    }
    QSqlQuery query;
    if(!query.exec("SELECT ticketid, employeeid, day, hour FROM timeslots")){
        //qDebug() << "Error executing load time query" << query.lastError().text();
        return;
    }
    while (query.next()) {
        int ticketid = query.value(0).toInt();
        int employeeid = query.value(1).toInt();
        int day = query.value(2).toInt();
        int hour = query.value(3).toInt();
        TimeSlot t(ticketid, employeeid, day, hour);
        allOccupiedSlots.append(t);
    }
}




void MainWindow::on_ticketList_itemClicked(QListWidgetItem *item)
{
    chosenEmployee = nullptr;
    ui->selectEmpLabel->setVisible(false);
    ui->selectTicketLabel->setVisible(false);
    for (int row = 0; row < ui->calendar->rowCount(); ++row) {
        for (int column = 0; column < ui->calendar->columnCount(); ++column) {
            QTableWidgetItem *item = ui->calendar->item(row, column);
            item->setBackground(Qt::white);
        }
    }

    chosenTicketIndex = ui->ticketList->row(item);
    chosenTicket = (Ticket*)&ticketList.at(chosenTicketIndex);
    for(TimeSlot i : allOccupiedSlots){
        if(i.ticketID == chosenTicket->GetTicketID()){
            QTableWidgetItem *item = ui->calendar->item(i.hour, i.day);
            item->setBackground(Qt::blue);
        }
    }
    if(chosenTicket->state == "closed"){
        ui->scheduleLabel->setText("No schedule - selected ticket closed");
    }
    else{
        ui->scheduleLabel->setText("Schedule: "+chosenTicket->brand+" "+chosenTicket->model+" (TicketID: "+QString::number(chosenTicket->GetTicketID())+")");
    }
}


void MainWindow::on_infoTicketButton_clicked()
{
    ui->selectEmpLabel->setVisible(false);
    ui->selectTicketLabel->setVisible(false);
    if(!chosenTicket){
        ui->selectTicketLabel->setText("Select a ticket first!");
        ui->selectTicketLabel->setVisible(true);
        return;
    }
    infoTicketForm = new TicketInfoDialog(nullptr, chosenTicket, &allOccupiedSlots, &employeeList, ui->calendar);
    connect(infoTicketForm, &TicketInfoDialog::windowClosing, this, &MainWindow::onInfoTicketFormDestroyed);
    infoTicketForm->setWindowTitle("Info about ticket: "+chosenTicket->brand+" "+chosenTicket->model);
    infoTicketForm->exec();

}

void MainWindow::onInfoTicketFormDestroyed(){
    infoTicketForm = nullptr;
}

void MainWindow::on_saveScheduleButton_clicked()
{
    ui->selectEmpLabel->setText("Select employee first!");
    ui->selectTicketLabel->setVisible(false);
    ui->selectEmpLabel->setVisible(false);
    if(!chosenEmployee){
        ui->selectEmpLabel->setVisible(true);
        return;
    }
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chosenEmployee->name+"'s schedule.pdf");



    QPainter painter(&printer);

    painter.setFont(QFont("Arial", 20));
    painter.drawText(100, 200, chosenEmployee->name+"'s"+"(ID:"+QString::number(chosenEmployee->id)+") schedule");
    painter.setFont(QFont("Arial", 12));


    qreal x = 1600;
    qreal y = 5500;
    painter.drawText(x, y, "Used time slots:");
    y += painter.fontMetrics().height();
    for(const TimeSlot& ts : allOccupiedSlots){
        if(ts.employeeID == chosenEmployee->id){
            painter.drawText(x, y, "Weekday: "+QString::number(ts.day+1)+" "+QString::number(ts.hour+8)+":00 - "+QString::number(ts.hour+9)+":00 TicketID: "+QString::number(ts.ticketID));
            y += painter.fontMetrics().height();
        }
    }

    QString calendarLabel("White - free time slots, Blue - used time slots");
    painter.drawText(1600, 1500, calendarLabel);
    int desiredWidth = 6500; // Example width
    int desiredHeight = 3250; // Example height


    float scaleX = desiredWidth / static_cast<float>(ui->calendar->width());
    float scaleY = desiredHeight / static_cast<float>(ui->calendar->height());


    float scaleFactor = std::min(scaleX, scaleY);


    painter.setViewport(0, 0, desiredWidth, desiredHeight);
    painter.setWindow(-1600, -1500, ui->calendar->width() * scaleFactor, ui->calendar->height() * scaleFactor);


    painter.scale(scaleFactor, scaleFactor);


    ui->calendar->render(&painter);
}

