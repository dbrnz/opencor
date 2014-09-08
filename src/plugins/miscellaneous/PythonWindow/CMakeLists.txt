PROJECT(PythonWindowPlugin)

# Shouldn't this come via PythonQt plugin?
find_package(PythonLibs)

# Add the plugin

ADD_PLUGIN(PythonWindow
    SOURCES
        ../../plugininfo.cpp

        src/pythonwindowplugin.cpp
        src/pythonwindow.cpp
    HEADERS_MOC
        src/pythonwindowplugin.h
        src/pythonwindow.h
    INCLUDE_DIRS
        src
# Shouldn't this come via PythonQt plugin?
        ${PYTHON_INCLUDE_DIRS}
    PLUGINS
        Core
        PythonQtAPI
    QT_MODULES
        Widgets
    QT_LIBRARIES
        QtCore
        QtGui
        QtWidgets
        QtNetwork
        QtXmlPatterns
)
