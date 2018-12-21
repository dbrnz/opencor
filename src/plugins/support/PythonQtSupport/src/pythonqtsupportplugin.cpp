/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Python Qt Support plugin
//==============================================================================

#include "CTK/ctkAbstractPythonManager.h"
#include "corecliutils.h"
#include "pythoninterface.h"
#include "pythonqtsupportplugin.h"
#include "solverinterface.h"

//==============================================================================

#include <PythonQt/PythonQt.h>

//==============================================================================

#include <Qt>

//==============================================================================

namespace OpenCOR {
namespace PythonQtSupport {

//==============================================================================

PLUGININFO_FUNC PythonQtSupportPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python Qt support plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Python Qt support plugin."));

    return new PluginInfo(PluginInfo::Support, false, false,
                          QStringList() << "PythonQtAPI" << "PythonSupport",
                          descriptions);
}

//==============================================================================
// Plugin interface
//==============================================================================

bool PythonQtSupportPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool PythonQtSupportPlugin::pluginInterfacesOk(const QString &pFileName,
                                               QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void PythonQtSupportPlugin::initializePlugin()
{
    // Create and initialise a new CTK Python manager

    mPythonManager = new ctkAbstractPythonManager(this);

    // This also initialises Python Qt

    mPythonManager->initialize();

    // Create a Python module to access OpenCOR's objects

    mOpenCORModule = PythonQt::self()->createModuleFromScript("OpenCOR");
}

//==============================================================================

void PythonQtSupportPlugin::finalizePlugin()
{
    delete mPythonManager;
}

//==============================================================================

void PythonQtSupportPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    // We need to register the Solver::Properties class with Qt so it gets automatically
    // wrapped to Python

    qRegisterMetaType<OpenCOR::Solver::Solver::Properties>("Solver::Solver::Properties");

    // Register wrappers for every plugin that has a Python interface

    foreach (Plugin *plugin, pLoadedPlugins) {
        PythonInterface *pythonInterface = qobject_cast<PythonInterface *>(plugin->instance());

        if (pythonInterface) {
            pythonInterface->registerPythonClasses(mOpenCORModule);
        }
    }
}

//==============================================================================

void PythonQtSupportPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void PythonQtSupportPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void PythonQtSupportPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================

}   // namespace PythonQtSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
