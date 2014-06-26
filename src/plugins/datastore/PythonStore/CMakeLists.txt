PROJECT(PythonStorePlugin)

# Add the plugin

ADD_PLUGIN(PythonStore
    SOURCES
        ../../plugininfo.cpp

        src/pythonstoreplugin.cpp
        src/pythonstore.cpp
    HEADERS_MOC
        src/pythonstoreplugin.h
        src/pythonstore.h
    INCLUDE_DIRS
        src
    PLUGINS
        CoreDatastore
    QT_MODULES
        Core
    QT_LIBRARIES
        QtCore
)
