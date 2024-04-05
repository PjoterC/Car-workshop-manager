#include "employeeadddialog.h"
#include "ui_employeeadddialog.h"

employeeAddDialog::employeeAddDialog(QWidget *parent, QList<Employee>* employeeList)
    : QDialog(parent)
    , ui(new Ui::employeeAddDialog)
{

    if(employeeList){
        employees = *employeeList;
    }
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("New Employee Form");

    ui->inputReqLabel->setVisible(false);
    ui->intLabel->setVisible(false);
    ui->floatLabel->setVisible(false);
}

employeeAddDialog::~employeeAddDialog()
{
    delete ui;
}


void employeeAddDialog::verifyAndSendData(){
    bool valid = true;
    ui->intLabel->setText("positive integer required!");
    ui->inputReqLabel->setVisible(false);
    ui->intLabel->setVisible(false);
    ui->floatLabel->setVisible(false);
    if(ui->nameEdit->text() == ""){
        ui->inputReqLabel->setVisible(true);
        valid = false;
    }
    if(ui->idEdit->text().toInt() <= 0){
        ui->intLabel->setVisible(true);
        valid = false;
    }
    if(ui->salaryEdit->text().toFloat() <= 0){
        ui->floatLabel->setVisible(true);
        valid = false;
    }
    for(const Employee& i : employees){
        if(i.id == ui->idEdit->text().toInt()){
            ui->intLabel->setText("Id already exists, pick another one");
            ui->intLabel->setVisible(true);
            valid = false;
            break;
        }
    }

    int id = ui->idEdit->text().toInt();
    QString name = ui->nameEdit->text();
    float salary = ui->salaryEdit->text().toFloat();
    Employee emp(name, id, salary);


    if(valid){

        emit dataEntered(emp);
        close();
    }

}


void employeeAddDialog::on_sendDataButton_clicked()
{
    verifyAndSendData();
}

