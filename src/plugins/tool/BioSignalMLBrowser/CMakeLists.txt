PROJECT(BioSignalMLBrowserPlugin)


MACRO(RETRIEVE_BioSignalML_Browser_SETTINGS)
    IF(WIN32)
        SET(BioSignalML_Browser_EXTERNAL_BINARIES
            browserlib${CMAKE_IMPORT_LIBRARY_SUFFIX}
        )
    ELSEIF(APPLE)
        SET(BioSignalML_Browser_EXTERNAL_BINARIES
            libbrowserlib${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ELSE()
        SET(BioSignalML_Browser_EXTERNAL_BINARIES
            libbrowserlib${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ENDIF()
ENDMACRO()

# Retrieve the plugin's binary file(s)

STRING(REPLACE "${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/" "" RELATIVE_PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${REMOTE_EXTERNAL_BINARIES_DIR}")

##IF(WIN32)
##    IF(RELEASE_MODE)
##        RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} browserlib.dll e441a499ae510dd1d1d202d54ca61fd06e5c7972)
##        RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} browserlib.lib 62d44d40d0f5ab044973e713016ce376f0b0cad4)
##    ELSE()
##        RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} browserlib.dll 0810a7d8bc7cedfd79fa617983be5f4e50aacb75)
##        RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} browserlib.lib 4fa1c9f7df1ac2dc4f325517a1b2f41e5d214c3f)
##    ENDIF()
##ELSEIF(APPLE)
##    RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} libbrowserlib.0.dylib 08c77f2e5ff4a983674b4a014f9394505a8314f7)
##ELSE()
##    RETRIEVE_BINARY_FILE_FROM("http://biosignalml.org/binaries" ${RELATIVE_PROJECT_SOURCE_DIR} libbrowserlib.so.0 00a6e77a7d968c588db6863eef87888bd8fc63b6)
##ENDIF()

# Retrieve some BioSignalML API settings

RETRIEVE_BioSignalML_Browser_SETTINGS()

# Add the plugin

SET(BioSignalML_Browser_EXTERNAL_BINARIES_DIR ${PROJECT_SOURCE_DIR}/${LOCAL_EXTERNAL_BINARIES_DIR})

ADD_PLUGIN(BioSignalMLBrowser
    SOURCES
        ../../guiinterface.cpp
        ../../i18ninterface.cpp
        ../../plugininfo.cpp

        src/biosignalmlbrowserplugin.cpp
        src/biosignalmlbrowser.cpp
    HEADERS_MOC
        src/biosignalmlbrowserplugin.h
        src/biosignalmlbrowser.h
    INCLUDE_DIRS
        src
        include

    EXTERNAL_BINARIES_DIR
        ${BioSignalML_Browser_EXTERNAL_BINARIES_DIR}
    EXTERNAL_BINARIES
        ${BioSignalML_Browser_EXTERNAL_BINARIES}

    PLUGINS
        Core
        BioSignalMLAPI
    QT_MODULES
        Widgets
)

# Deploy the plugin's external binaries
# Note: it must be done here since ADD_PLUGIN doesn't support the fact that an
#       external binary may have dependencies (something that is important on OS
#       X)...

IF(WIN32)
    WINDOWS_DEPLOY_LIBRARY(${BioSignalML_Browser_EXTERNAL_BINARIES_DIR}
                           browserlib${CMAKE_SHARED_LIBRARY_SUFFIX})
ELSEIF(APPLE)
    OS_X_DEPLOY_LIBRARY(${BioSignalML_Browser_EXTERNAL_BINARIES_DIR} browserlib)
ELSE()
    FOREACH(BioSignalML_Browser_EXTERNAL_BINARY ${BioSignalML_Browser_EXTERNAL_BINARIES})
        LINUX_DEPLOY_LIBRARY(${BioSignalML_Browser_EXTERNAL_BINARIES_DIR}
                             ${BioSignalML_Browser_EXTERNAL_BINARY})
    ENDFOREACH()
ENDIF()
