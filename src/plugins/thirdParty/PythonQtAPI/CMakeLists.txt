project(PythonQtAPIPlugin)

# Name and version of our package

set(PACKAGE_NAME PythonQt)
set(PACKAGE_VERSION "3.2.2")

# Release tag of our packaged library

set(RELEASE_TAG opencor_0.7)

# Version of our library

set(MAJOR_LIBRARY_VERSION "3")
set(LIBRARY_VERSION "${MAJOR_LIBRARY_VERSION}.2.0")

# Git tag for our library

set(GIT_TAG opencor_r502.1)

# Specify where our local package will be installed

set(FULL_LOCAL_EXTERNAL_PACKAGE_DIR ${PROJECT_SOURCE_DIR}/${LOCAL_EXTERNAL_PACKAGE_DIR})

# Name of our package's shared libraries (and of their import libraries, if any)

if(WIN32)
    set(SHARED_LIBRARY PythonQt${MAJOR_LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(IMPORT_LIBRARY PythonQt${MAJOR_LIBRARY_VERSION}${CMAKE_IMPORT_LIBRARY_SUFFIX})
else()
    if(APPLE)
        set(SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}PythonQt${MAJOR_LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
    else()
        set(SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}PythonQt${CMAKE_SHARED_LIBRARY_SUFFIX}.${MAJOR_LIBRARY_VERSION})
    endif()
    set(IMPORT_LIBRARY ${SHARED_LIBRARY})
endif()

# Retrieve our package's binaries

set(EXTERNAL_BINARIES_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib)

set(EXTERNAL_BINARIES
    ${SHARED_LIBRARY}
)

# Specify the files that need to have their SHA-1 value checked

set(SHA1_FILES
    lib/${SHARED_LIBRARY}
)

if(WIN32)
    list(APPEND SHA1_FILES
        lib/${IMPORT_LIBRARY}
    )
endif()

# Use the pre-built version of our package unless instructed otherwise

if(USE_PREBUILT_PYTHONQT_PACKAGE)
    # Retrieve the plugin's package

    string(REPLACE "${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/" ""
           RELATIVE_PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${REMOTE_EXTERNAL_PACKAGE_DIR}")

    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${RELATIVE_PROJECT_SOURCE_DIR} f34cc0b62f6278b6d868a1b58ba047cb383425ce
                                  RELEASE_TAG ${RELEASE_TAG}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES 842bf2909de590a095dd6a22599e40902b2cc10d
                                              4da8ffd18a26e5b52cfe0ac5f8054617e2aa9386)
        endif()
    elseif(APPLE)
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} c4b6ebbb49cfe100f3417ed825d62800283dcd47
                              RELEASE_TAG ${RELEASE_TAG}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES db5365de3f4927cbf4ce516a1002840c42a99363)
    else()
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} 0909c215c1fc19a40ffe47741f3d9ac592c6051a
                              RELEASE_TAG ${RELEASE_TAG}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES de354e780acce6d43719e45ca13f213d40e2f780)
    endif()
else()
    # Set platform specific configuration options and tool names

    if(WIN32)
        set(QMAKE_LFLAGS_OPTION)
        set(MAKE_COMMAND nmake)
        set(QUIET_MAKE -c)
    else()
        if(NOT APPLE)
          # Make sure correct value of RPATH is set in library

            set(QMAKE_LFLAGS_OPTION "QMAKE_LFLAGS+=\\'-Wl,-rpath,\\\$\\\$ORIGIN/../lib\\'")
        elseif()
            set(QMAKE_LFLAGS_OPTION)
        endif()

        set(MAKE_COMMAND make)
        set(QUIET_MAKE --quiet)
    endif()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(QMAKE_DEBUG_OPTION "CONFIG+=debug")
    elseif()
        set(QMAKE_DEBUG_OPTION)
    endif()

    # Wrap our WebKit

    set(QMAKE_INCLUDE_PATH "INCLUDEPATH+=${QTWEBKIT_INCLUDE_DIR}")

    # Configure qmake project files with our settings

    set(PYTHONQT_INSTALL_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR})
    set(PYTHONQT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/proj)

    configure_file(${PROJECT_SOURCE_DIR}/qmake/PythonQtOpenCOR.pro.in
                   ${PROJECT_BINARY_DIR}/PythonQtOpenCOR.pro
                   @ONLY)
    configure_file(${PROJECT_SOURCE_DIR}/qmake/build/common.prf.in
                   ${PROJECT_BINARY_DIR}/build/common.prf
                   @ONLY)
    configure_file(${PROJECT_SOURCE_DIR}/qmake/build/python.prf.in
                   ${PROJECT_BINARY_DIR}/build/python.prf
                   @ONLY)

    # Build PythonQt as an external project

    set(PACKAGE_BUILD ${PACKAGE_NAME}Build)

    ExternalProject_Add(${PACKAGE_BUILD}
        DEPENDS
            ${PYTHON_DEPENDENCIES}
        DOWNLOAD_DIR
            ${PYTHONQT_SOURCE_DIR}
        SOURCE_DIR
            ${PYTHONQT_SOURCE_DIR}
        BINARY_DIR
            ${PYTHONQT_SOURCE_DIR}
        INSTALL_DIR
            ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
        GIT_REPOSITORY
            https://github.com/opencor/PythonQt
        GIT_TAG
            ${GIT_TAG}
        CONFIGURE_COMMAND
            ${QMAKE_COMMAND} ${QMAKE_DEBUG_OPTION} ${QMAKE_INCLUDE_PATH} ${QMAKE_LFLAGS_OPTION} -r PythonQtOpenCOR.pro
        BUILD_COMMAND
            ${MAKE_COMMAND}
        INSTALL_COMMAND
            ${MAKE_COMMAND} ${MAKE_QUIET} install
        BUILD_BYPRODUCTS
            <INSTALL_DIR>/lib/${IMPORT_LIBRARY}
    )

    # Copy our configured qmake project files into the build

    ExternalProject_Add_Step(${PACKAGE_BUILD} update_configuration
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/PythonQtOpenCOR.pro
                                         ${PYTHONQT_SOURCE_DIR}/PythonQtOpenCOR.pro
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/build/common.prf
                                         ${PYTHONQT_SOURCE_DIR}/build/common.prf
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/build/python.prf
                                         ${PYTHONQT_SOURCE_DIR}/build/python.prf
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/qmake/extensions/PythonQt_QtAll/PythonQt_QtAll.pri
                                         ${PYTHONQT_SOURCE_DIR}/extensions/PythonQt_QtAll
        DEPENDEES update
        DEPENDERS configure
        )

    # Do some post-processing that is specific to each platform

    if(NOT WIN32)
        # Dereference the symbolic link to the library since CMake's tar can't
        # do this

        set(FULL_SHARED_LIBRARY ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib/${SHARED_LIBRARY})

        if(APPLE)
            set(REAL_FULL_SHARED_LIBRARY ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PythonQt.${LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
        else()
            set(REAL_FULL_SHARED_LIBRARY ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PythonQt${CMAKE_SHARED_LIBRARY_SUFFIX}.${LIBRARY_VERSION})
        endif()

        ExternalProject_Add_Step(${PACKAGE_BUILD} CopyLibrary
                                 COMMAND ${CMAKE_COMMAND} -E remove -f ${FULL_SHARED_LIBRARY}
                                 COMMAND ${CMAKE_COMMAND} -E copy ${REAL_FULL_SHARED_LIBRARY} ${FULL_SHARED_LIBRARY}
                                 DEPENDEES install)

        if(APPLE)
            # Set RPATH in library to enable linking independent of location

            ExternalProject_Add_Step(${PACKAGE_BUILD} set_rpath
                                     COMMAND install_name_tool -id @rpath/${SHARED_LIBRARY} ${FULL_SHARED_LIBRARY}
                                     DEPENDEES CopyLibrary)
        endif()

    endif()

    # Package our external project

    create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                        ${LOCAL_EXTERNAL_PACKAGE_DIR}
                        RELEASE_TAG ${RELEASE_TAG}
                        PACKAGED_FILES include ${SHA1_FILES}
                        SHA1_FILES ${SHA1_FILES}
                        TARGET ${PACKAGE_BUILD})

    set(DEPENDS_ON ${PACKAGE_BUILD})
endif()

# Allow other external projects to find our PythonQt package

set(PYTHONQT_ROOT_DIR ${LOCAL_EXTERNAL_PACKAGE_DIR} PARENT_SCOPE)

# Add the plugin

add_plugin(PythonQtAPI
    SOURCES
        ../../plugininfo.cpp

        src/pythonqtapiplugin.cpp
    HEADERS_MOC
        src/pythonqtapiplugin.h
    PLUGINS
        Python
    EXTERNAL_BINARIES_DIR
        ${EXTERNAL_BINARIES_DIR}
    EXTERNAL_BINARIES
        ${EXTERNAL_BINARIES}
    DEPENDS_ON
        ${DEPENDS_ON}
)

# Update our list of Python dependencies for other plugins

set(PYTHON_DEPENDENCIES ${PYTHON_DEPENDENCIES} ${INSTALL_EXTERNAL_FILES_TARGET} PARENT_SCOPE)
