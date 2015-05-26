QT += widgets

HEADERS       = lightsim.h \
    libfixmath/libfixmath/fix16.h \
    libfixmath/libfixmath/fixmath.h \
    libfixmath/libfixmath/int64.h \
    libfixmath/libfixmath/uint32.h
SOURCES       = lightsim.cpp \
                main.cpp \
    libfixmath/libfixmath/fix16.c \
    libfixmath/libfixmath/fix16_exp.c \
    libfixmath/libfixmath/fix16_sqrt.c \
    libfixmath/libfixmath/fix16_str.c \
    libfixmath/libfixmath/fix16_trig.c \
    libfixmath/libfixmath/fract32.c \
    libfixmath/libfixmath/uint32.c \
    lightgene.cpp \
    hsvrgb.cpp

QMAKE_PROJECT_NAME = lightsim

# install
target.path = /home/bunnie/code/lightsim/lightsim
INSTALLS += target

OTHER_FILES += \
    genes.txt \
    genes.md
