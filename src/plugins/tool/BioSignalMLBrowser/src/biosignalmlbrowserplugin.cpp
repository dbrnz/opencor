/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// BioSignalMLBrowser plugin
//==============================================================================

#include "biosignalmlbrowserplugin.h"
#include "biosignalmlbrowser.h"

//==============================================================================

#include "coreguiutils.h"

//==============================================================================

#include <QMainWindow>
#include <QAction>

//==============================================================================

namespace OpenCOR {
namespace BioSignalMLBrowser {

//==============================================================================

PLUGININFO_FUNC BioSignalMLBrowserPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a BioSignalML browser plugin."));
    descriptions.insert("fr", QString::fromUtf8("a BioSignalML browser plugin."));

    return new PluginInfo("Tool", true, true,
                          QStringList() << "BioSignalMLAPI",
                          descriptions);
}

//==============================================================================
// CLI interface
//==============================================================================
 
int BioSignalMLBrowserPlugin::executeCommand(const QString &pCommand,
                                             const QStringList &pArguments)
{
    if (!pCommand.compare("help")) {
        mBrowser->runHelpCommand();
        return 0;
    } else if (!pCommand.compare("browse")) {
        return mBrowser->runBioSignalMLBrowser(pArguments);
    } else {
        mBrowser->runHelpCommand();
        return -1;
    }
}

//==============================================================================
// GUI interface
//==============================================================================
 
void BioSignalMLBrowserPlugin::updateGui(Plugin *pViewPlugin, const QString &pFileName)
{
    Q_UNUSED(pViewPlugin);
    Q_UNUSED(pFileName);
}
 
//==============================================================================
 
Gui::Menus BioSignalMLBrowserPlugin::guiMenus() const
{
    return Gui::Menus();
}
 
//==============================================================================
 
Gui::MenuActions BioSignalMLBrowserPlugin::guiMenuActions() const
{
    return Gui::MenuActions() << Gui::MenuAction(Gui::MenuAction::Tools, mBrowseBioSignalMLAction)
                              << Gui::MenuAction(Gui::MenuAction::Tools);
}
 
//==============================================================================
// I18n interface
//==============================================================================

void BioSignalMLBrowserPlugin::retranslateUi()
{
    retranslateAction(mBrowseBioSignalMLAction, tr("Browse BioSignalML..."), tr("Browse BioSignalML datasets"));
}

//==============================================================================
// Plugin interface
//==============================================================================

void BioSignalMLBrowserPlugin::initializePlugin()
{
    mBrowser = new BioSignalMLBrowser();
    mBrowseBioSignalMLAction = new QAction(Core::mainWindow());
    connect(mBrowseBioSignalMLAction, SIGNAL(triggered()),
            mBrowser, SLOT(browseBioSignalML()));
}

//==============================================================================

void BioSignalMLBrowserPlugin::finalizePlugin()
{
}

//==============================================================================

void BioSignalMLBrowserPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins)
}

//==============================================================================

void BioSignalMLBrowserPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);
}

//==============================================================================

void BioSignalMLBrowserPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);
}

//==============================================================================

void BioSignalMLBrowserPlugin::handleAction(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);
}

//==============================================================================

}   // namespace BioSignalMLBrowser
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
