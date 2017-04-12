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
// Simulation Experiment view information simulation widget
//==============================================================================

#pragma once

//==============================================================================

#include "propertyeditorwidget.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace SimulationSupport {
    class SimulationSupportSimulation;
}   // namespace SimulationSupport

//==============================================================================

namespace SimulationExperimentView {

//==============================================================================

class SimulationExperimentViewInformationSimulationWidget : public Core::PropertyEditorWidget
{
    Q_OBJECT

public:
    explicit SimulationExperimentViewInformationSimulationWidget(QWidget *pParent);

    virtual void retranslateUi();

    void initialize(SimulationSupport::SimulationSupportSimulation *pSimulation);

    Core::Property * startingPointProperty() const;
    Core::Property * endingPointProperty() const;
    Core::Property * pointIntervalProperty() const;

    double startingPoint() const;
    double endingPoint() const;
    double pointInterval() const;

private:
    Core::Property *mStartingPointProperty;
    Core::Property *mEndingPointProperty;
    Core::Property *mPointIntervalProperty;

    void updateToolTips();
};

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
