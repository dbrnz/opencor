/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// Simulation support plugin
//==============================================================================

#include "corecliutils.h"
#include "simulationsupportplugin.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace SimulationSupport {

//==============================================================================

PLUGININFO_FUNC SimulationSupportPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to support <a href=\"https://models.physiomeproject.org/\">Simulations</a>."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour supporter <a href=\"https://models.physiomeproject.org/\">Simulations</a>."));

    return new PluginInfo(PluginInfo::Support, false, false,
                          QStringList() << "COMBINESupport" << "SEDMLSupport",
                          descriptions);
}

//==============================================================================

SimulationSupportPlugin::SimulationSupportPlugin() :
    mSolverInterfaces(SolverInterfaces())
{
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SimulationSupportPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SimulationSupportPlugin::pluginInterfacesOk(const QString &pFileName,
                                                        QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SimulationSupportPlugin::initializePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SimulationSupportPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SimulationSupportPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    // Retrieve the different solvers that are available to us

    foreach (Plugin *plugin, pLoadedPlugins) {
        // Look for a solver

        SolverInterface *solverInterface = qobject_cast<SolverInterface *>(plugin->instance());

        if (solverInterface)
            mSolverInterfaces << solverInterface;
    }
}

//==============================================================================

void SimulationSupportPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SimulationSupportPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SimulationSupportPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Plugin specific
//==============================================================================

SimulationSupportPlugin * SimulationSupportPlugin::instance()
{
    // Return the 'global' instance of our plugin

    static SimulationSupportPlugin instance;

    return static_cast<SimulationSupportPlugin *>(Core::globalInstance("OpenCOR::PMRSupport::SimulationSupportPlugin::instance()",
                                                                       &instance));
}

//==============================================================================

SolverInterfaces SimulationSupportPlugin::solverInterfaces() const
{
    // Return our solver interfaces

    return mSolverInterfaces;
}

//==============================================================================


}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
