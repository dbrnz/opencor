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
// PythonQt interface
//==============================================================================

#ifndef PYTHONQTAPI_H
#define PYTHONQTAPI_H

//==============================================================================

#include "PythonQtAPIGlobal.h"

//==============================================================================

#include "PythonQt.h"
// FUTURE #include "PythonQt_QtAll.h"

//==============================================================================

namespace OpenCOR {
namespace PythonQtAPI {

//==============================================================================

class PYTHONQTAPI_EXPORT PythonQtAPI : public QObject
{
    Q_OBJECT

public:
    PythonQtAPI(void);
    ~PythonQtAPI(void);
    static void initialise(void);
    static PythonQtAPI * instance(void);

    static void setPythonInstance(PythonQt *pPythonInstance);
    static PythonQt * pythonInstance(void);
    static PythonQtObjectPtr mainModule(void);
    static PythonQtObjectPtr opencorModule(void);

    static void addVariable(const QString &pName, QObject *pObject);

private:
    PythonQt *mPythonInstance;
    static PythonQtObjectPtr mMainModule;
    static PythonQtObjectPtr mOpenCOR;
};

//==============================================================================

}   // namespace PythonQtAPI
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
