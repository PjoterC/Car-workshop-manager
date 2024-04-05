QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    boughtpart.cpp \
    employee.cpp \
    employeeadddialog.cpp \
    main.cpp \
    mainwindow.cpp \
    partadddialog.cpp \
    ticket.cpp \
    ticketadddialog.cpp \
    ticketeditdialog.cpp \
    ticketinfodialog.cpp \
    timeslot.cpp

HEADERS += \
    boughtpart.h \
    employee.h \
    employeeadddialog.h \
    mainwindow.h \
    partadddialog.h \
    ticket.h \
    ticketadddialog.h \
    ticketeditdialog.h \
    ticketinfodialog.h \
    timeslot.h

FORMS += \
    employeeadddialog.ui \
    mainwindow.ui \
    partadddialog.ui \
    ticketadddialog.ui \
    ticketeditdialog.ui \
    ticketinfodialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
