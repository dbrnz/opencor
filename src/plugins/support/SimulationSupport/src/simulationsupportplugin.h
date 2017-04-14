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

#pragma once

//==============================================================================

#include "plugininfo.h"
#include "plugininterface.h"
#include "solverinterface.h"

//==============================================================================

namespace OpenCOR {
namespace SimulationSupport {

//==============================================================================

PLUGININFO_FUNC SimulationSupportPluginInfo();

//==============================================================================

class SimulationSupportPlugin : public QObject,
                                public PluginInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.SimulationSupportPlugin" FILE "simulationsupportplugin.json")

    Q_INTERFACES(OpenCOR::PluginInterface)

public:
    explicit SimulationSupportPlugin();

#include "plugininterface.inl"

    static SimulationSupportPlugin * instance();

    SolverInterfaces solverInterfaces() const;

private:
    SolverInterfaces mSolverInterfaces;
};

//==============================================================================

}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
