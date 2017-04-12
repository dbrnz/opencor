PROJECT(SimulationSupportPlugin)

# Add the plugin

ADD_PLUGIN(SimulationSupport
    SOURCES
        ../../datastoreinterface.cpp
        ../../i18ninterface.cpp
        ../../plugininfo.cpp
        ../../plugininterface.cpp
        ../../solverinterface.cpp

        src/simulationsupport.cpp
        src/simulationsupportplugin.cpp
        src/simulationsupportsimulation.cpp
        src/simulationsupportsimulationworker.cpp
    HEADERS_MOC
        ../../datastoreinterface.h
        ../../plugin.h
        ../../pluginmanager.h
        ../../solverinterface.h

        src/simulationsupportplugin.h
        src/simulationsupportsimulation.h
        src/simulationsupportsimulationworker.h
    INCLUDE_DIRS
        src
    PLUGINS
        Core
        CellMLSupport
        COMBINESupport
        SEDMLSupport
)
