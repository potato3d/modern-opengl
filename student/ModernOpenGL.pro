TEMPLATE = app
CONFIG += console
CONFIG += c++14
CONFIG -= qt

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
OTHER_FILES += $$files(*.vert)
OTHER_FILES += $$files(*.frag)
OTHER_FILES += $$files(*.comp)

HEADERS += $$files(rvm/*.h)
SOURCES += $$files(rvm/*.cpp)

HEADERS += $$files(tess/*.h)
SOURCES += $$files(tess/*.cpp)
SOURCES += $$files(tess/glutess/*.c)

win32 {
    DEFINES += GLEW_STATIC
    DEFINES += FREEGLUT_STATIC
    INCLUDEPATH += ../dep/freeglut/inc
    INCLUDEPATH += ../dep/glew/inc
    INCLUDEPATH += ../dep/glm/inc
    LIBS += ../dep/glew/lib/libglew.a ../dep/freeglut/lib/libfreeglut.a -lopengl32 -lglu32 -lwinmm -lgdi32 -lpthread
}
unix {
    LIBS += -lGL -lGLU -lglut -lGLEW -lX11 -lpthread
}
