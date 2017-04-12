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
// Simulation Experiment view contents widget
//==============================================================================

#include "borderedwidget.h"
#include "graphpanelswidget.h"
#include "simulationexperimentviewcontentswidget.h"
#include "simulationexperimentviewinformationwidget.h"

//==============================================================================

#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace SimulationExperimentView {

//==============================================================================

SimulationExperimentViewContentsWidget::SimulationExperimentViewContentsWidget(SimulationExperimentViewWidget *pViewWidget,
                                                                               SimulationExperimentViewSimulationWidget *pSimulationWidget,
                                                                               QWidget *pParent) :
    Core::SplitterWidget(pParent)
{
    // Keep track of our movement

    connect(this, SIGNAL(splitterMoved(int, int)),
            this, SLOT(emitSplitterMoved()));

    // Create our information widget

    mInformationWidget = new SimulationExperimentViewInformationWidget(pViewWidget, pSimulationWidget, this);

    mInformationWidget->setObjectName("Information");

    // Create our graph panels widget

    mGraphPanelsWidget = new GraphPanelWidget::GraphPanelsWidget(this);

    mGraphPanelsWidget->setObjectName("GraphPanels");

    // Add our information and graph panels widgets to ourselves

    addWidget(new Core::BorderedWidget(mInformationWidget,
                                       false, false, true, true));
    addWidget(new Core::BorderedWidget(mGraphPanelsWidget,
                                       false, true, true, false));

    // Make our graph panels widget our focus proxy

    setFocusProxy(mGraphPanelsWidget);
}

//==============================================================================

void SimulationExperimentViewContentsWidget::retranslateUi()
{
    // Retranslate our information and graph panels widgets

    mInformationWidget->retranslateUi();
    mGraphPanelsWidget->retranslateUi();
}

//==============================================================================

SimulationExperimentViewInformationWidget * SimulationExperimentViewContentsWidget::informationWidget() const
{
    // Return our information widget

    return mInformationWidget;
}

//==============================================================================

GraphPanelWidget::GraphPanelsWidget * SimulationExperimentViewContentsWidget::graphPanelsWidget() const
{
    // Return our graph panels widget

    return mGraphPanelsWidget;
}

//==============================================================================

void SimulationExperimentViewContentsWidget::emitSplitterMoved()
{
    // Let people know that our splitter has been moved

    emit splitterMoved(sizes());
}

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
