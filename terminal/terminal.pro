QT += widgets serialport sql

TARGET = terminal
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp \
    Ymodem.cpp \
    crc16.c \
    SendSave.cpp \
    SSWorker.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    console.h \
    Ymodem.h \
    crc.h \
    SendSave.h \
    SSWorker.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    SendSave.ui

RESOURCES += \
    terminal.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
