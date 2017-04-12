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
