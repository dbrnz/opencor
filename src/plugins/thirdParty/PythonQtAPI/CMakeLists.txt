PROJECT(PythonQtAPIPlugin)

SET(PythonQt_VERSION 2.2.0)

### Debug
option(PythonQt_DEBUG "Enable/Disable PythonQt debug output" OFF)
if(PythonQt_DEBUG)
	add_definitions(-DPYTHONQT_DEBUG)
else()
	remove_definitions(-DPYTHONQT_DEBUG)
endif()

include(PythonQt/cmake/PythonQt_Qt_5x.cmake)

# The variable "generated_cpp_suffix" allows to conditionnally compile the generated wrappers
# All 5.x use the same for now
set(generated_cpp_suffix "_50")

add_subdirectory(PythonQt/generator_50 EXCLUDE_FROM_ALL)
add_custom_target(generator)
add_dependencies(generator pythonqt_generator)

### Find Python
find_package(PythonLibs)

# Add the plugin

ADD_PLUGIN(PythonQtAPI

    THIRD_PARTY         # Disable wraning as error

    SOURCES
        ../../plugininfo.cpp

        src/PythonQtAPI.cpp
        src/PythonQtAPIPlugin.cpp

        PythonQt/src/PythonQtClassInfo.cpp
        PythonQt/src/PythonQtClassWrapper.cpp
        PythonQt/src/PythonQtConversion.cpp
        PythonQt/src/PythonQt.cpp
        PythonQt/src/PythonQtImporter.cpp
        PythonQt/src/PythonQtInstanceWrapper.cpp
        PythonQt/src/PythonQtMethodInfo.cpp
        PythonQt/src/PythonQtMisc.cpp
        PythonQt/src/PythonQtObjectPtr.cpp
        PythonQt/src/PythonQtQFileImporter.cpp
        PythonQt/src/PythonQtSignalReceiver.cpp
        PythonQt/src/PythonQtSlot.cpp
        PythonQt/src/PythonQtSignal.cpp
        PythonQt/src/PythonQtStdDecorators.cpp
        PythonQt/src/PythonQtStdIn.cpp
        PythonQt/src/PythonQtStdOut.cpp
        PythonQt/src/gui/PythonQtScriptingConsole.cpp

        PythonQt/generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.cpp

        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp
        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp

    HEADERS_MOC
        src/PythonQtAPI.h
        src/PythonQtAPIPlugin.h

        PythonQt/src/PythonQt.h
        PythonQt/src/PythonQtSignalReceiver.h
        PythonQt/src/PythonQtStdDecorators.h
        PythonQt/src/gui/PythonQtScriptingConsole.h

        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h
        PythonQt/generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h

    INCLUDE_DIRS
        src
        PythonQt/src/
        ${PYTHON_INCLUDE_DIRS}

    DEFINITIONS
        PYTHONQT_USE_RELEASE_PYTHON_FALLBACK
        PYTHONQT_EXPORTS

    QT_MODULES
        Core
        Gui
        Widgets

    QT_LIBRARIES
        QtCore

    EXTERNAL_BINARIES_DIR
        /

    EXTERNAL_BINARIES
        ${PYTHON_LIBRARIES}
)
