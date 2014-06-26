PROJECT(PythonQtPlugin)

# Add the plugin

ADD_PLUGIN(PythonQt
    SOURCES
        ../../plugininfo.cpp

        src/PythonQtPlugin.cpp
    HEADERS_MOC
        src/PythonQtPlugin.h
    INCLUDE_DIRS
        src
    QT_MODULES
        Core
    QT_LIBRARIES
        QtCore
)
