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
// A store for simulation data
//==============================================================================

#include "pythonstore.h"

//==============================================================================

#define NPY_NO_DEPRECATED_API NPY_1_8_API_VERSION

#define PY_ARRAY_UNIQUE_SYMBOL OpenCOR_Python_Store_PyArray_API
#include <numpy/arrayobject.h>

//==============================================================================

namespace OpenCOR {
namespace PythonStore {

//==============================================================================

DataStoreVariableWrapper::DataStoreVariableWrapper(const CoreDataStore::DataStoreVariable * pVariable) :
    mVariable(pVariable)
{
    if (OpenCOR_Python_Store_PyArray_API == NULL) import_array();
}

//==============================================================================

PyObject * DataStoreVariableWrapper::new_wrapper(const CoreDataStore::DataStoreVariable * pVariable)
{
  return PythonQt::priv()->wrapQObject(new DataStoreVariableWrapper(pVariable));
  }

//==============================================================================

QString DataStoreVariableWrapper::uri(void) const
{
    return mVariable->uri();
}

//==============================================================================

QString DataStoreVariableWrapper::unit(void) const
{
    return mVariable->unit();
}

//==============================================================================

QString DataStoreVariableWrapper::label(void) const
{
    return mVariable->name();
}

//==============================================================================

double DataStoreVariableWrapper::value(const qulonglong &pPos) const
{
    return mVariable->value(pPos);
}

//==============================================================================

const PyObject * DataStoreVariableWrapper::values(void) const
{
    npy_intp dims[1];
    dims[0] = mVariable->size();
    return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, (void *)mVariable->values());
}

//==============================================================================

qulonglong DataStoreVariableWrapper::size(void) const
{
    return mVariable->size();
}

//==============================================================================
//==============================================================================

CoreDataStoreWrapper::CoreDataStoreWrapper(CoreDataStore::CoreDataStore * pDataStore) :
    mDataStore(pDataStore)
{
}

//==============================================================================

const PyObject * CoreDataStoreWrapper::voi(void) const
{
    return DataStoreVariableWrapper::new_wrapper(mDataStore->voi());
}

//==============================================================================

const PyObject * CoreDataStoreWrapper::variables(void) const
{
    CoreDataStore::DataStoreVariables vars = mDataStore->variables();
    PyObject *varlist = PyList_New(vars.size());
    for (auto i = 0 ;  i < vars.size() ;  ++i)
      PyList_SET_ITEM(varlist, i, DataStoreVariableWrapper::new_wrapper(vars[i]));
    return varlist ;
}

//==============================================================================


qulonglong CoreDataStoreWrapper::size(void) const
{
    return mDataStore->size();
}

//==============================================================================
//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
