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
// A Pythonic interface to simulation data
//==============================================================================

#ifndef PYTHONSTORE_H
#define PYTHONSTORE_H

//==============================================================================

#include "coredatastore.h"
#include "datastorevariable.h"

//==============================================================================

#include "PythonQt.h"
#include "PythonQtConversion.h"

//==============================================================================

#include <QtGlobal>

//==============================================================================

namespace OpenCOR {
namespace PythonStore {

//==============================================================================

class DataStoreVariableWrapper : public QObject {
    Q_OBJECT

public:
    DataStoreVariableWrapper(const CoreDataStore::DataStoreVariable * pVariable);
    static PyObject * new_wrapper(const CoreDataStore::DataStoreVariable * pVariable);

public slots:
    QString uri(void) const;
    QString unit(void) const;
    QString label(void) const;
    double value(const qulonglong &pPos) const;
    const PyObject * values(void) const;
    qulonglong size(void) const;

private:
    const CoreDataStore::DataStoreVariable *mVariable;
};

//==============================================================================

class CoreDataStoreWrapper : public QObject {
    Q_OBJECT

public:
    CoreDataStoreWrapper(CoreDataStore::CoreDataStore * pDataStore);

public slots:
    const PyObject * voi(void) const;
    const PyObject * variables(void) const;
    qulonglong size(void) const;

private:
    CoreDataStore::CoreDataStore *mDataStore;
};

//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
