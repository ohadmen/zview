QT += core 

DEFINES += QT_NO_VERSION_TAGGING

TEMPLATE = lib
CONFIG += c++17 
unix:QMAKE_CXXFLAGS += -std=c++17

TARGET = zview_inf
DESTDIR = ./build
OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc




# Input

SOURCES += \
    zview/interface/zview_inf_impl.cpp\
    zview/interface/zview_inf.cpp\
    zview/io/read_file_list.cpp \
    zview/io/readers/read_obj.cpp \
    zview/io/readers/read_ply.cpp \
    zview/io/readers/read_stl.cpp \
    zview/io/write_shape_to_file.cpp \



HEADERS += \
    zview/interface/zview_inf.h\
