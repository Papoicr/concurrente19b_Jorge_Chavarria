TEMPLATE = app
QT -= core
CONFIG += console c11
CONFIG -= qt app_bundle

QMAKE_CFLAGS += -pthread
LIBS += -pthread

SOURCES += circular_collatz.c \

