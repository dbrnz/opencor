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
// Python Qt Support plugin
//==============================================================================

#pragma once

//==============================================================================

#include "plugininfo.h"
#include "plugininterface.h"

//==============================================================================

class ctkAbstractPythonManager;

//==============================================================================

namespace OpenCOR {
namespace PythonQtSupport {

//==============================================================================

PLUGININFO_FUNC PythonQtSupportPluginInfo();

//==============================================================================

class PythonQtSupportPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.PythonQtSupportPlugin" FILE "pythonqtsupportplugin.json")

    Q_INTERFACES(OpenCOR::PluginInterface)

public:
#include "plugininterface.inl"

    static ctkAbstractPythonManager *pythonManager();

    static PythonQtSupportPlugin *instance();

private:
    ctkAbstractPythonManager *mPythonManager;
};

//==============================================================================

}   // namespace PythonQtSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================