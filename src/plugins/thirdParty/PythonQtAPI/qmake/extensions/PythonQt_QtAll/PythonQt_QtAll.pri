CONFIG += qt

DEFINES += PYTHONQT_QTALL_EXPORTS

HEADERS +=                \
  $$PWD/PythonQt_QtAll.h

# Header files we want installed
INCLUDES +=               \
  $$PWD/PythonQt_QtAll.h

SOURCES +=                \
  $$PWD/PythonQt_QtAll.cpp

QT += printsupport

QT += gui opengl network svg xml xmlpatterns

DEFINES += PYTHONQT_WITH_CORE
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_core/com_trolltech_qt_core.pri)

DEFINES += PYTHONQT_WITH_GUI
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_gui/com_trolltech_qt_gui.pri)

DEFINES += PYTHONQT_WITH_OPENGL
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_opengl/com_trolltech_qt_opengl.pri)

##DEFINES += PYTHONQT_WITH_NETWORK
##include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_svg/com_trolltech_qt_network.pri)

DEFINES += PYTHONQT_WITH_SVG
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_svg/com_trolltech_qt_svg.pri)

DEFINES += PYTHONQT_WITH_XML
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_xml/com_trolltech_qt_xml.pri)

DEFINES += PYTHONQT_WITH_XMLPATTERNS
include ($$PYTHONQT_GENERATED_PATH/com_trolltech_qt_xmlpatterns/com_trolltech_qt_xmlpatterns.pri)
