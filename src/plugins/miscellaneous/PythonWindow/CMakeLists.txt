PROJECT(PythonWindowPlugin)

# Add the plugin

ADD_PLUGIN(PythonWindow
    SOURCES
        ../../i18ninterface.cpp
        ../../plugininfo.cpp

        src/pythonwindowplugin.cpp
        src/pythonwindow.cpp
    HEADERS_MOC
        src/pythonwindowplugin.h
        src/pythonwindow.h
    INCLUDE_DIRS
        src
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
