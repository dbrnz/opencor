PROJECT(PythonStorePlugin)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(NumPy REQUIRED)


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
        ${NUMPY_INCLUDE_DIR}
    PLUGINS
        CoreDataStore
        PythonQtAPI
    QT_MODULES
        Core
    QT_LIBRARIES
        QtCore
        QtGui
        QtWidgets
        QtNetwork
        QtXmlPatterns
)
