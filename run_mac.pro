QMAKE_MAC_SDK = macosx10.9
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt

SOURCES += \
    src/walker.cpp \
    src/run.cpp \
    src/group.cpp \
    src/graph.cpp \
    src/FixedSizeFIFOQ.cpp \
    src/statistic.cpp

HEADERS += \
    src/walker.h \
    src/graph.h \
    src/statistic.h

INCLUDEPATH += /opt/local/include/
