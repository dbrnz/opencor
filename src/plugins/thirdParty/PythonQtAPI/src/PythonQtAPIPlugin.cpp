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
// Python Qt plugin
//==============================================================================

#include "PythonQtAPIPlugin.h"
#include "PythonQtAPI.h"

//==============================================================================

#include <QMainWindow>

#include <QtDebug>

//==============================================================================

namespace OpenCOR {
namespace PythonQtAPI {

//==============================================================================

PLUGININFO_FUNC PythonQtAPIPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python Qt plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Python Qt plugin."));

    return new PluginInfo(PluginInfo::ThirdParty, true, false,
                          QStringList() << "Core",
                          descriptions);
}

//==============================================================================

//==============================================================================
// I18n interface
//==============================================================================

void PythonQtAPIPlugin::retranslateUi()
{
    // We don't handle this interface...
}

//==============================================================================
// Plugin interface
//==============================================================================

void PythonQtAPIPlugin::initializePlugin(QMainWindow * pMainWindow)
{
    Q_UNUSED(pMainWindow);

    PythonQtAPI::initialise();
}

//==============================================================================

void PythonQtAPIPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void PythonQtAPIPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // We don't handle this interface...
}

//==============================================================================

void PythonQtAPIPlugin::loadSettings(QSettings * pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void PythonQtAPIPlugin::saveSettings(QSettings * pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void PythonQtAPIPlugin::handleAction(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================

}   // namespace PythonQtAPI
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
