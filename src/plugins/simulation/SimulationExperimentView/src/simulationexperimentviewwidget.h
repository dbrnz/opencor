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
// Simulation Experiment view widget
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfile.h"
#include "combinearchive.h"
#include "corecliutils.h"
#include "sedmlfile.h"
#include "simulationsupport.h"
#include "viewwidget.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace SimulationSupport {
    class SimulationSupportSimulation;
}   // namespace SimulationSupport

//==============================================================================

namespace SimulationExperimentView {

//==============================================================================

class SimulationExperimentViewPlugin;
class SimulationExperimentViewSimulationWidget;

//==============================================================================

class SimulationExperimentViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit SimulationExperimentViewWidget(SimulationExperimentViewPlugin *pPlugin,
                                            QWidget *pParent);

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void retranslateUi();

//    bool isIndirectRemoteFile(const QString &pFileName);

    void initialize(const QString &pFileName);
    void finalize(const QString &pFileName);

    QIcon fileTabIcon(const QString &pFileName) const;

    bool saveFile(const QString &pOldFileName, const QString &pNewFileName);

    void fileOpened(const QString &pFileName);
    void filePermissionsChanged(const QString &pFileName);
    void fileModified(const QString &pFileName);
    void fileReloaded(const QString &pFileName);
    void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);
    void fileClosed(const QString &pFileName);

    QStringList fileNames() const;

    SimulationExperimentViewSimulationWidget * simulationWidget(const QString &pFileName) const;
    SimulationSupport::SimulationSupportSimulation * simulation(const QString &pFileName) const;
    CellMLSupport::CellmlFileRuntime * runtime(const QString &pFileName) const;

    virtual QWidget * widget(const QString &pFileName);

    qulonglong simulationResultsSize(const QString &pFileName) const;

    void checkSimulationResults(const QString &pFileName,
                                const bool &pClearGraphs = false);

    void retrieveFileDetails(const QString &pFileName,
                             CellMLSupport::CellmlFile *&pCellmlFile,
                             SEDMLSupport::SedmlFile *&pSedmlFile,
                             COMBINESupport::CombineArchive *&pCombineArchive,
                             SimulationSupport::FileType &pFileType,
                             SEDMLSupport::SedmlFileIssues &pSedmlFileIssues,
                             COMBINESupport::CombineArchiveIssues &pCombineArchiveIssues,
                             bool *pIsDirectOrIndirectRemoteFile = 0);

private:
    SimulationExperimentViewPlugin *mPlugin;

    QIntList mSimulationWidgetSizes;
    QIntList mContentsWidgetSizes;

    QBoolList mCollapsibleWidgetCollapsed;

    QIntList mSimulationWidgetColumnWidths;
    QIntList mSolversWidgetColumnWidths;
    QIntList mGraphsWidgetColumnWidths;
    QIntList mParametersWidgetColumnWidths;

    SimulationExperimentViewSimulationWidget *mSimulationWidget;
    QMap<QString, SimulationExperimentViewSimulationWidget *> mSimulationWidgets;

    QStringList mFileNames;

    QMap<QString, qulonglong> mSimulationResultsSizes;
    QStringList mSimulationCheckResults;

    QMap<QString, QString> mLocallyManagedCellmlFiles;
    QMap<QString, QString> mLocallyManagedSedmlFiles;

    void updateContentsInformationGui(SimulationExperimentViewSimulationWidget *pSimulationWidget);

private slots:
    void simulationWidgetSplitterMoved(const QIntList &pSizes);
    void contentsWidgetSplitterMoved(const QIntList &pSizes);

    void collapsibleWidgetCollapsed(const int &pIndex, const bool &pCollapsed);

    void simulationWidgetHeaderSectionResized(const int &pIndex,
                                              const int &pOldSize,
                                              const int &pNewSize);
    void solversWidgetHeaderSectionResized(const int &pIndex,
                                           const int &pOldSize,
                                           const int &pNewSize);
    void graphsWidgetHeaderSectionResized(const int &pIndex,
                                          const int &pOldSize,
                                          const int &pNewSize);
    void parametersWidgetHeaderSectionResized(const int &pIndex,
                                              const int &pOldSize,
                                              const int &pNewSize);

    void callCheckSimulationResults();
};

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
