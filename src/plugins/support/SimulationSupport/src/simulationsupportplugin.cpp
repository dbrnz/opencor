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
            instance()->mSolverInterfaces << solverInterface;
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
