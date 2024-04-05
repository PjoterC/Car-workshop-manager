#include "ticketinfodialog.h"
#include "ui_ticketinfodialog.h"
#include <QPdfWriter>
TicketInfoDialog::TicketInfoDialog(QWidget *parent, const Ticket* ticket, const QList<TimeSlot>* timeslots, const QList<Employee>* employees, QTableWidget *table)
    : QDialog(parent)
    , ui(new Ui::TicketInfoDialog), t(ticket), ts(timeslots), emps(employees), calendar(table)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);



    ui->brand->setText(t->brand);
    ui->model->setText(t->model);
    ui->regid->setText(t->regID);
    ui->description->setText(t->description);
    ui->cost->setText(QString::number(t->expectedCost));
    for(const BoughtPart& bp : t->boughtParts){
        QString label("Item: "+bp.name+" Amount: "+QString::number(bp.amount)+" Price each: $"+QString::number(bp.price)+" Total: $"+QString::number(bp.totalPrice));
        ui->parts->addItem(label);
    }
    ui->state->setText(t->state);
    if(t->estimateAccepted){
        ui->accepted->setText("Yes");
    }
    else{
        ui->accepted->setText("No");
    }
    ui->clientPaid->setText(QString::number(t->amountPaidByClient));

}

TicketInfoDialog::~TicketInfoDialog()
{
    delete ui;
}

void TicketInfoDialog::on_estimateButton_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(t->brand+t->model+"estimate.pdf");

    QPainter painter(&printer);

    qreal x = 100;
    qreal y = 100;

    painter.setFont(QFont("Arial", 12));
    painter.drawText(x, y, "Brand:  "+t->brand);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Model:  "+t->model);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Registration ID:  "+t->regID);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Issue description: ");
    y += painter.fontMetrics().height();
    painter.drawText(x, y, t->description);
    y += 2*painter.fontMetrics().height();
    painter.drawText(x, y, "Expected cost of labour($):  "+QString::number(t->expectedCost));
    QString accepted;
    if(t->estimateAccepted){
        accepted = "Yes";
    }
    else{
        accepted = "No";
    }
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "\nWas estimate accepted by client?  "+accepted);

    // End painting
    painter.end();
}


void TicketInfoDialog::on_costButton_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(t->brand+t->model+"finalcost.pdf");



    QPainter painter(&printer);

    qreal x = 100;
    qreal y = 100;

    painter.setFont(QFont("Arial", 13));
    painter.drawText(x, y, "Brand:  "+t->brand);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Model:  "+t->model);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Registration ID:  "+t->regID);
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Issue description: ");
    y += painter.fontMetrics().height();
    painter.drawText(x, y, t->description);
    y += 2*painter.fontMetrics().height();
    painter.drawText(x, y, "Managing employee: "+t->employeeInCharge.name);
    y += 2*painter.fontMetrics().height();
    painter.drawText(x, y, "Bought parts:");
    y += painter.fontMetrics().height();

    float finalcost = t->expectedCost;
    for(const BoughtPart& entry : t->boughtParts){
        painter.drawText(x, y, "Item: "+entry.name+" Amount: "+QString::number(entry.amount)+" Price each: $"+QString::number(entry.price)+" Total: $"+QString::number(entry.totalPrice));
        y += painter.fontMetrics().height();
        finalcost+=entry.totalPrice;
    }
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Amount paid by client:  $"+QString::number(t->amountPaidByClient));
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Total cost of repair:  $"+QString::number(finalcost));
    y += painter.fontMetrics().height();
    painter.drawText(x, y, "Profit:  $"+QString::number(t->amountPaidByClient-finalcost));


    QString calendarLabel("White - free time slots, Blue - used time slots");
    if(t->state == "closed"){
        calendarLabel = "No schedule available - ticket is closed";
    }
    painter.drawText(1600, 4800, calendarLabel);

    x = 1600;
    y = 9000;
    painter.drawText(x, y, "Used time slots:");
    y += painter.fontMetrics().height();
    for(const TimeSlot& ts : *ts){
        if(ts.ticketID == t->GetTicketID()){
        painter.drawText(x, y, "Weekday: "+QString::number(ts.day+1)+"   "+QString::number(ts.hour+8)+":00 - "+QString::number(ts.hour+9)+":00   EmployeeID: "+QString::number(ts.employeeID));
        y += painter.fontMetrics().height();
        }
    }



    int desiredWidth = 6500;
    int desiredHeight = 3250;


    float scaleX = desiredWidth / static_cast<float>(calendar->width());
    float scaleY = desiredHeight / static_cast<float>(calendar->height());


    float scaleFactor = std::min(scaleX, scaleY);


    painter.setViewport(0, 0, desiredWidth, desiredHeight);
    painter.setWindow(-1600, -4000, calendar->width() * scaleFactor, calendar->height() * scaleFactor);


    painter.scale(scaleFactor, scaleFactor);


    calendar->render(&painter);



}

