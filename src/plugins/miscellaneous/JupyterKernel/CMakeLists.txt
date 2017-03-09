PROJECT(JupyterKernelPlugin)

# Add the plugin

ADD_PLUGIN(JupyterKernel
    SOURCES
        ../../eventloopinterface.cpp
        ../../plugininfo.cpp
        ../../plugininterface.cpp

        src/jupyterkernelplugin.cpp
    HEADERS_MOC
        src/jupyterkernelplugin.h
    INCLUDE_DIRS
        src
    PLUGINS
        Core
        PythonQtSupport
        PythonWrapper
)

# Install our Jupyter server and kernel startup module

ADD_CUSTOM_COMMAND(TARGET JupyterKernelPlugin POST_BUILD
    COMMAND ${PYTHON_EXECUTABLE} -s setup.py --quiet build --build-base=${PROJECT_BINARY_DIR}/src install
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src
)
