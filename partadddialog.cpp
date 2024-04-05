#include "partadddialog.h"

#include "ui_partadddialog.h"

PartAddDialog::PartAddDialog(QWidget *parent, int ticketid)
    : QDialog(parent)
    , ui(new Ui::PartAddDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Part adding form");


    this->ticketid = ticketid;

}

PartAddDialog::~PartAddDialog()
{
    delete ui;
}


bool PartAddDialog::verifyAndSendData(){
    bool valid = true;
    if(ui->amountEdit->text().toFloat() <= 0){
        valid = false;
    }
    if(ui->priceEdit->text().toFloat() <= 0){
        valid = false;
    }
    if(ui->itemEdit->text() == ""){
        valid = false;
    }

    if(valid){
        BoughtPart bp(ticketid, ui->itemEdit->text(), ui->amountEdit->text().toFloat(), ui->priceEdit->text().toFloat());
        emit dataEntered(bp);
    }
    return valid;

}

void PartAddDialog::on_addButton_clicked()
{
    if(verifyAndSendData()){
        close();
    }
}

