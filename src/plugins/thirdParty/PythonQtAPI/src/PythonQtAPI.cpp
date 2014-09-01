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

#include "PythonQtAPI.h"

//==============================================================================

#include "cliutils.h"

//==============================================================================

#include <QtDebug>

//==============================================================================

namespace OpenCOR {
namespace PythonQtAPI {

//==============================================================================

void PythonQtAPI::initialise(void)
{
// Can we use system Python and it's site packages?
// Maybe do a sanity check via sys.version string??
    PythonQt::init(/*PythonQt::IgnoreSiteModule | */PythonQt::RedirectStdOut);
    PythonQtAPI::setPythonInstance(PythonQt::self());
// FUTURE    PythonQt_QtAll::init();
}

//==============================================================================

PythonQtAPI *PythonQtAPI::instance(void)
{
    // Return the 'global' instance of our Python Qt API class

    static PythonQtAPI instance;
    return static_cast<PythonQtAPI *>(Core::globalInstance("OpenCOR::PythonQtAPI::PythonQtAPI",
                                                           &instance));
}

//==============================================================================

void PythonQtAPI::setPythonInstance(PythonQt *pPythonInstance)
{
    PythonQtAPI::instance()->mPythonInstance = pPythonInstance;
}

//==============================================================================

PythonQt * PythonQtAPI::PythonInstance(void)
{
    return PythonQtAPI::instance()->mPythonInstance;
}

//==============================================================================

}   // namespace PythonQtAPI
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
