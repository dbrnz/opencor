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

#include "cellmlfilemanager.h"
#include "cellmlsupportplugin.h"
#include "combinefilemanager.h"
#include "combinesupportplugin.h"
#include "coreguiutils.h"
#include "sedmlfilemanager.h"
#include "sedmlsupportplugin.h"
#include "simulationexperimentviewplugin.h"
#include "simulationexperimentviewsimulationwidget.h"
#include "simulationexperimentviewwidget.h"

//==============================================================================

#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace SimulationExperimentView {

//==============================================================================

PLUGININFO_FUNC SimulationExperimentViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to run a simulation experiment."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour exécuter une expérience de simulation."));

    return new PluginInfo(PluginInfo::Simulation, true, false,
                          QStringList() << "COMBINESupport"<< "GraphPanelWidget" << "Qwt" << "SEDMLSupport",
                          descriptions);
}

//==============================================================================

SimulationExperimentViewPlugin::SimulationExperimentViewPlugin() :
    mDataStoreInterfaces(DataStoreInterfaces()),
    mCellmlEditingViewPlugins(Plugins()),
    mCellmlSimulationViewPlugins(Plugins()),
    mSedmlFileTypeInterface(0),
    mCombineFileTypeInterface(0)
{
}

//==============================================================================
// File handling interface
//==============================================================================

bool SimulationExperimentViewPlugin::isIndirectRemoteFile(const QString &pFileName)
{
    // Check whether the given file is an indirect remote file

    return mViewWidget->isIndirectRemoteFile(pFileName);
}

//==============================================================================

bool SimulationExperimentViewPlugin::saveFile(const QString &pOldFileName,
                                              const QString &pNewFileName,
                                              bool &pNeedFeedback)
{
    Q_UNUSED(pNeedFeedback);

    // Let our view widget know that we want to save a file

    return mViewWidget->saveFile(pOldFileName, pNewFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileOpened(const QString &pFileName)
{
    // Let our view widget know that a file has been opened

    mViewWidget->fileOpened(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::filePermissionsChanged(const QString &pFileName)
{
    // The given file has been un/locked, so let our view widget know about it

    mViewWidget->filePermissionsChanged(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileModified(const QString &pFileName)
{
    // Let our view widget know that a file has been modified

    mViewWidget->fileModified(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileReloaded(const QString &pFileName,
                                                  const bool &pFileChanged)
{
    Q_UNUSED(pFileChanged);

    // The given file has been reloaded, so let its corresponding view widget
    // know about it

    mViewWidget->fileReloaded(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileRenamed(const QString &pOldFileName,
                                                 const QString &pNewFileName)
{
    // Let our view widget know that a file has been renamed

    mViewWidget->fileRenamed(pOldFileName, pNewFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileClosed(const QString &pFileName)
{
    // Let our view widget know that a file has been closed

    mViewWidget->fileClosed(pFileName);
}

//==============================================================================
// I18n interface
//==============================================================================

void SimulationExperimentViewPlugin::retranslateUi()
{
    // Retranslate our Simulation Experiment view widget

    mViewWidget->retranslateUi();
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SimulationExperimentViewPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SimulationExperimentViewPlugin::pluginInterfacesOk(const QString &pFileName,
                                                        QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SimulationExperimentViewPlugin::initializePlugin()
{
    // Create our Simulation Experiment view widget

    mViewWidget = new SimulationExperimentViewWidget(this, Core::mainWindow());

    mViewWidget->setObjectName("SimulationExperimentViewWidget");

    // Hide our Simulation Experiment view widget since it may not initially be
    // shown in  our central widget

    mViewWidget->setVisible(false);
}

//==============================================================================

void SimulationExperimentViewPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SimulationExperimentViewPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    // Retrieve the different solvers and data stores that are available to us,
    // as well as the file types supported by the SEDMLSupport plugin

    foreach (Plugin *plugin, pLoadedPlugins) {
        // Look for a data store

        DataStoreInterface *dataStoreInterface = qobject_cast<DataStoreInterface *>(plugin->instance());

        if (dataStoreInterface)
            mDataStoreInterfaces << dataStoreInterface;

        // Look for a CellML capable editing or simulation view

        ViewInterface *viewInterface = qobject_cast<ViewInterface *>(plugin->instance());

        if (   viewInterface
            && (   (viewInterface->viewMode() == EditingMode)
                || (viewInterface->viewMode() == SimulationMode))) {
            QStringList viewMimeTypes = viewInterface->viewMimeTypes(OpenMimeTypeMode);

            if (   viewMimeTypes.isEmpty()
                || viewMimeTypes.contains(CellMLSupport::CellmlMimeType)) {
                if (viewInterface->viewMode() == EditingMode)
                    mCellmlEditingViewPlugins << plugin;
                else
                    mCellmlSimulationViewPlugins << plugin;
            }
        }

        // Keep track of the file type interfaces for the SEDMLSupport and
        // COMBINESupport plugins

        FileTypeInterface *fileTypeInterface = qobject_cast<FileTypeInterface *>(plugin->instance());

        if (fileTypeInterface) {
            if (!plugin->name().compare("SEDMLSupport"))
                mSedmlFileTypeInterface = fileTypeInterface;
            else if (!plugin->name().compare("COMBINESupport"))
                mCombineFileTypeInterface = fileTypeInterface;
        }
    }
}

//==============================================================================

void SimulationExperimentViewPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve our Simulation Experiment view settings

    pSettings->beginGroup(mViewWidget->objectName());
        mViewWidget->loadSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void SimulationExperimentViewPlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of our Simulation Experiment view settings

    pSettings->beginGroup(mViewWidget->objectName());
        mViewWidget->saveSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void SimulationExperimentViewPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// View interface
//==============================================================================

ViewInterface::Mode SimulationExperimentViewPlugin::viewMode() const
{
    // Return our mode

    return SimulationMode;
}

//==============================================================================

QStringList SimulationExperimentViewPlugin::viewMimeTypes(const MimeTypeMode &pMimeTypeMode) const
{
    // Return the MIME types we support

    if (pMimeTypeMode == OpenMimeTypeMode) {
        return QStringList() << CellMLSupport::CellmlMimeType
                             << SEDMLSupport::SedmlMimeType
                             << COMBINESupport::CombineMimeType;
    } else {
        return QStringList() << CellMLSupport::CellmlMimeType;
    }
}

//==============================================================================

QString SimulationExperimentViewPlugin::viewDefaultFileExtension() const
{
    // Return the default file extension we support

    return CellMLSupport::CellmlFileExtension;
}

//==============================================================================

QWidget * SimulationExperimentViewPlugin::viewWidget(const QString &pFileName)
{
    // Make sure that we are dealing with a CellML file, a SED-ML file or a
    // COMBINE archive

    if (   !CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName)
        && !SEDMLSupport::SedmlFileManager::instance()->sedmlFile(pFileName)
        && !COMBINESupport::CombineFileManager::instance()->combineArchive(pFileName)) {
        return 0;
    }

    // Update and return our simulation view widget using the given CellML file,
    // SED-ML file or COMBINE archive

    mViewWidget->initialize(pFileName);

    return mViewWidget;
}

//==============================================================================

void SimulationExperimentViewPlugin::removeViewWidget(const QString &pFileName)
{
    // Ask our view widget to finalise the given CellML file

    mViewWidget->finalize(pFileName);
}

//==============================================================================

QString SimulationExperimentViewPlugin::viewName() const
{
    // Return our Simulation Experiment view's name

    return tr("Simulation Experiment");
}

//==============================================================================

QIcon SimulationExperimentViewPlugin::fileTabIcon(const QString &pFileName) const
{
    // Return the requested file tab icon

    return mViewWidget->fileTabIcon(pFileName);
}

//==============================================================================
// Plugin specific
//==============================================================================

SimulationExperimentViewWidget * SimulationExperimentViewPlugin::viewWidget() const
{
    // Return our view widget

    return mViewWidget;
}

//==============================================================================

DataStoreInterfaces SimulationExperimentViewPlugin::dataStoreInterfaces() const
{
    // Return our data store interfaces

    return mDataStoreInterfaces;
}

//==============================================================================

Plugins SimulationExperimentViewPlugin::cellmlEditingViewPlugins() const
{
    // Return our CellML editing view plugins

    return mCellmlEditingViewPlugins;
}

//==============================================================================

Plugins SimulationExperimentViewPlugin::cellmlSimulationViewPlugins() const
{
    // Return our CellML simulation view plugins

    return mCellmlSimulationViewPlugins;
}

//==============================================================================

FileTypeInterface * SimulationExperimentViewPlugin::sedmlFileTypeInterface() const
{
    // Return our SED-ML file type interface

    return mSedmlFileTypeInterface;
}

//==============================================================================

FileTypeInterface * SimulationExperimentViewPlugin::combineFileTypeInterface() const
{
    // Return our COMBINE file type interface

    return mCombineFileTypeInterface;
}

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
