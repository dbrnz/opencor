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
// Simulation Experiment view plugin
//==============================================================================

#pragma once

//==============================================================================

#include "datastoreinterface.h"
#include "filehandlinginterface.h"
#include "filetypeinterface.h"
#include "i18ninterface.h"
#include "plugininfo.h"
#include "plugininterface.h"
#include "viewinterface.h"

//==============================================================================

namespace OpenCOR {
namespace SimulationExperimentView {

//==============================================================================

PLUGININFO_FUNC SimulationExperimentViewPluginInfo();

//==============================================================================

class SimulationExperimentViewWidget;

//==============================================================================

class SimulationExperimentViewPlugin : public QObject,
                                       public FileHandlingInterface,
                                       public I18nInterface,
                                       public PluginInterface,
                                       public ViewInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.SimulationExperimentViewPlugin" FILE "simulationexperimentviewplugin.json")

    Q_INTERFACES(OpenCOR::FileHandlingInterface)
    Q_INTERFACES(OpenCOR::I18nInterface)
    Q_INTERFACES(OpenCOR::PluginInterface)
    Q_INTERFACES(OpenCOR::ViewInterface)

public:
    explicit SimulationExperimentViewPlugin();

#include "filehandlinginterface.inl"
#include "i18ninterface.inl"
#include "plugininterface.inl"
#include "viewinterface.inl"

    SimulationExperimentViewWidget * viewWidget() const;

    DataStoreInterfaces dataStoreInterfaces() const;

    Plugins cellmlEditingViewPlugins() const;
    Plugins cellmlSimulationViewPlugins() const;

    FileTypeInterface * sedmlFileTypeInterface() const;
    FileTypeInterface * combineFileTypeInterface() const;

private:
    SimulationExperimentViewWidget *mViewWidget;

    DataStoreInterfaces mDataStoreInterfaces;
    Plugins mCellmlEditingViewPlugins;
    Plugins mCellmlSimulationViewPlugins;

    FileTypeInterface *mSedmlFileTypeInterface;
    FileTypeInterface *mCombineFileTypeInterface;
};

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
