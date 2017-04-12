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
// Simulation Experiment view information widget
//==============================================================================

#pragma once

//==============================================================================

#include "commonwidget.h"

//==============================================================================

#include <QScrollArea>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class CollapsibleWidget;
}   // namespace Core

//==============================================================================

namespace SimulationExperimentView {

//==============================================================================

class SimulationExperimentViewInformationGraphsWidget;
class SimulationExperimentViewInformationParametersWidget;
class SimulationExperimentViewInformationSimulationWidget;
class SimulationExperimentViewInformationSolversWidget;
class SimulationExperimentViewSimulationWidget;
class SimulationExperimentViewWidget;

//==============================================================================

class SimulationExperimentViewInformationWidget : public QScrollArea,
                                                  public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit SimulationExperimentViewInformationWidget(SimulationExperimentViewWidget *pViewWidget,
                                                       SimulationExperimentViewSimulationWidget *pSimulationWidget,
                                                       QWidget *pParent);

    virtual void retranslateUi();

    Core::CollapsibleWidget * collapsibleWidget() const;

    SimulationExperimentViewInformationSimulationWidget * simulationWidget() const;
    SimulationExperimentViewInformationSolversWidget * solversWidget() const;
    SimulationExperimentViewInformationGraphsWidget * graphsWidget() const;
    SimulationExperimentViewInformationParametersWidget * parametersWidget() const;

    void finishEditing(const bool &pPausedSimulation);

private:
    Core::CollapsibleWidget *mCollapsibleWidget;

    SimulationExperimentViewInformationSimulationWidget *mSimulationWidget;
    SimulationExperimentViewInformationSolversWidget *mSolversWidget;
    SimulationExperimentViewInformationGraphsWidget *mGraphsWidget;
    SimulationExperimentViewInformationParametersWidget *mParametersWidget;
};

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
