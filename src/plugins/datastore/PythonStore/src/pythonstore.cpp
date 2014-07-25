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

//==============================================================================

namespace PythonStore {

//==============================================================================

DataVariableWrapper::DataVariableWrapper(const CoreData::DataVariable *pVariable)
/*-----------------------------------------------------------------------------*/
: mVariable(pVariable)
{
  if (OpenCOR_Python_Store_PyArray_API == NULL) import_array() ;
  }

const PyObject *DataVariableWrapper::getData(void) const
/*----------------------------------------------------*/
{
   npy_intp dims[1] ;
   dims[0] = mVariable->getSize() ;
   return PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, (void *)mVariable->getData()) ;
   }

//==============================================================================

PyObject *DataVariableWrapper::new_wrapper(const CoreData::DataVariable *pVariable)
/*-------------------------------------------------------------------------------*/
{
  if (pVariable) {
    return PythonQt::priv()->wrapPtr(new DataVariableWrapper(pVariable), "DataVariableWrapper");
    }
  else {
    Py_INCREF(Py_None);
    return Py_None;
    }
  }

const PyObject *DataSetWrapper::getVariables(void) const
/*----------------------------------------------------*/
{
  const QVector<CoreData::DataVariable *> vars = mDataset->getVariables() ;
  PyObject *varlist = PyList_New(vars.size()) ;
  for (auto i = 0 ;  i < vars.size() ;  ++i)
    PyList_SET_ITEM(varlist, i, DataVariableWrapper::new_wrapper(vars[i])) ;
  return varlist ;
  }

const PyObject *DataSetWrapper::getVariable(long index) const
/*---------------------------------------------------------*/
{
  return DataVariableWrapper::new_wrapper(mDataset->getVariable(index)) ;
  }

const PyObject *DataSetWrapper::getVoi(void) const
/*----------------------------------------------*/
{
  return DataVariableWrapper::new_wrapper(mDataset->getVoi()) ;
  }


const PyObject *DataSetWrapper::py_get_attribute(const QString &name) const
/*------------------------------------------------------------------*/
{
  Q_UNUSED(name) ;
  return getVoi() ;
  }

//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
