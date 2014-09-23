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
// Python Window plugin
//==============================================================================

#include "pythonwindowplugin.h"
#include "pythonwindow.h"

//==============================================================================

#include "guiutils.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace PythonWindow {

//==============================================================================

PLUGININFO_FUNC PythonWindowPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python window plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Python window plugin."));

    return new PluginInfo(PluginInfo::Miscellaneous, true, false,
                          QStringList() << "Core" << "PythonQtAPI",
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================
 
void PythonWindowPlugin::retranslateUi()
{
  // Retranslate our sample window action
  retranslateAction(mPythonWindowAction, tr("Python"),
                                         tr("Show/hide the Python window"));
  }
     
//==============================================================================
// Plugin interface
//==============================================================================
 
void PythonWindowPlugin::initializePlugin(QMainWindow *pMainWindow)
{
  // Create an action to show/hide our sample window
  mPythonWindowAction = Core::newAction(true, pMainWindow);
   
  // Create our sample window
  mPythonWindow = new PythonWindow(pMainWindow);
  }
 
//==============================================================================
 
void PythonWindowPlugin::finalizePlugin()
{
  // We don't handle this interface...
  }
 
//==============================================================================
 
void PythonWindowPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
  Q_UNUSED(pLoadedPlugins);
 
  // We don't handle this interface...
  }
 
//==============================================================================
 
void PythonWindowPlugin::loadSettings(QSettings *pSettings)
{
  Q_UNUSED(pSettings);
 
  // We don't handle this interface...
  }
 
//==============================================================================
 
void PythonWindowPlugin::saveSettings(QSettings *pSettings) const
{
  Q_UNUSED(pSettings);
 
  // We don't handle this interface...
  }
 
//==============================================================================
 
void PythonWindowPlugin::handleAction(const QUrl &pUrl)
{
  Q_UNUSED(pUrl);
 
  // We don't handle this interface...
  }
 
//==============================================================================


//==============================================================================
// Window interface
//==============================================================================
 
Qt::DockWidgetArea PythonWindowPlugin::windowDefaultDockArea() const
{
  // Return our default dock area
  return Qt::TopDockWidgetArea;
  }
 
//==============================================================================
 
QAction * PythonWindowPlugin::windowAction() const
{
  // Return our window action
  return mPythonWindowAction;
  }
 
//==============================================================================
 
QDockWidget * PythonWindowPlugin::windowWidget() const
{
  // Return our window widget
  return mPythonWindow;
  }

//==============================================================================

}   // namespace PythonWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
