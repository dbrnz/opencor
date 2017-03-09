/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// Jupyter kernel plugin
//==============================================================================

#include "ctkAbstractPythonManager.h"
#include "jupyterkernelplugin.h"
#include "PythonQt.h"
#include "pythonqtsupportplugin.h"

//==============================================================================

#include <QCoreApplication>
#include <QSettings>
#include <QTimer>

#include <QDebug>

//==============================================================================

namespace OpenCOR {
namespace JupyterKernel {

//==============================================================================

PLUGININFO_FUNC JupyterKernelPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Jupyter kernel plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Jupyter kernel plugin."));

    return new PluginInfo(PluginInfo::Miscellaneous, true, false,
                          QStringList() << "Core" << "PythonQtSupport" << "PythonWrapper",
                          descriptions);
}

//==============================================================================
// Event loop interface
//==============================================================================

bool JupyterKernelPlugin::useExec()
{
    return mRunKernel;
}

//==============================================================================

int JupyterKernelPlugin::exec()
{
    if (mRunKernel)
        return runKernel();

    return 0;
}

//==============================================================================
// Plugin interface
//==============================================================================

bool JupyterKernelPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool JupyterKernelPlugin::pluginInterfacesOk(const QString &pFileName,
                                                   QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void JupyterKernelPlugin::initializePlugin()
{
    mRunKernel = false;
}

//==============================================================================

void JupyterKernelPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void JupyterKernelPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // We don't handle this interface...
}

//==============================================================================

void JupyterKernelPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void JupyterKernelPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void JupyterKernelPlugin::handleUrl(const QUrl &pUrl)
{
    // Create a kernel with the specified connection file

    initialiseKernel(pUrl.path().mid(1));

    // All ready to be started

    mRunKernel = true;
}

//==============================================================================
// Plugin specific
//==============================================================================

// The OpenCOR Jupyter kernel

static QString jupyterKernel = R"PYTHON(
import os, sys

from ipykernel.eventloops import register_integration, enable_gui, loop_qt4
from ipykernel.ipkernel import IPythonKernel
from ipykernel.kernelapp import IPKernelApp

import matplotlib


@register_integration('opencor')
def loop_opencor(kernel):
    """Start a kernel with PyQt5 event loop integration."""
    os.environ['QT_API'] = 'opencor'
    return loop_qt4(kernel)


class OpenCORKernel(IPythonKernel):
    implementation = 'OpenCOR'
    implementation_version = '0.1'
    banner = "Jupyter kernel for OpenCOR"

    def __init__(self, *args, **kwds):
        super().__init__(*args, **kwds)
        # Use the Jupyter notebook backend for matplotlib
        matplotlib.use('nbagg')
        # Work nicely with OpenCOR's Qt exec() loop
        enable_gui('opencor', self)

    def do_shutdown(self, restart):
        from PythonQt import QtGui
        QtGui.QApplication.instance().exit(0)
        return super().do_shutdown(restart)


class OCKernelApp(IPKernelApp):
    name='opencor-kernel'
    kernel_class = OpenCORKernel
    log_format = '[%(levelname)1.1s %(asctime)s.%(msecs).03d %(name)s] %(message)s'
    log_datefmt = "%H:%M:%S"


if __name__ == '__main__':
    import OpenCOR

    connection_file = '%1'   # Set by `initialiseKernel()`
    if connection_file:
        OpenCOR.serverProcess = None
    else:
        # If no connection file has been given then we run a cut-down Jupyter server.

        import jupyter_opencor.server
        import multiprocessing as mp
        ctx.set_executable(os.path.join(sys.exec_prefix, 'bin/python'))  ## .exe under windows...

        ctx = mp.get_context('spawn')
        jupyter_connection, opencor_connection = ctx.Pipe()

        OpenCOR.serverProcess = ctx.Process(name='jupyter', target=jupyter_opencor.server.main, args=(opencor_connection,))
        OpenCOR.serverProcess.start()

        connection_file = jupyter_connection.recv()
        jupyter_connection.close()

    # We save the initialised application instance in our module
    # ready for starting when all of OpenCOR has been initialised.

    OpenCOR.kernelApp = OCKernelApp.instance(connection_file=connection_file)
    OpenCOR.kernelApp.initialize()
    )PYTHON";

//==============================================================================

void JupyterKernelPlugin::initialiseKernel(const QString &pConnectionFile)
{
    // Setup the the kernel using our connection file

    PythonQtSupport::PythonQtSupportPlugin::pythonManager()->executeString(jupyterKernel.arg(pConnectionFile));
}

//==============================================================================

static QString startKernel = R"PYTHON(

if __name__ == '__main__':
    import OpenCOR

    OpenCOR.kernelApp.start()
    )PYTHON";

//==============================================================================

int JupyterKernelPlugin::runKernel()
{
    // Start the initialised kernel

    PythonQtSupport::PythonQtSupportPlugin::pythonManager()->executeString(startKernel);

    // TODO: return gui->exec()
    return 0;
}

//==============================================================================

}   // namespace JupyterKernel
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
