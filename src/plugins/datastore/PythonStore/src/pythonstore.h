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

#include "coredata.h"

//==============================================================================

#include "PythonQt.h"
#include "PythonQtConversion.h"

//==============================================================================

#include <QtGlobal>

//==============================================================================

namespace OpenCOR {
namespace PythonStore {

//==============================================================================

class DataVariableWrapper : public QObject {
  Q_OBJECT

 public:
  DataVariableWrapper(const CoreData::DataVariable *pVariable) ;

  static PyObject *new_wrapper(const CoreData::DataVariable *pVariable) ;

 public slots:

  QString getUri(void) const
    { return mVariable->getUri() ; }

  QString getUnits(void) const
    { return mVariable->getUnits() ; }

  QString getLabel(void) const
    { return mVariable->getLabel() ; }

  double getPoint(const CoreData::SizeType &pPos) const
    { return mVariable->getPoint(pPos) ; }

  const PyObject *getData(void) const ;

  qulonglong getSize(void) const
    { return mVariable->getSize() ; }

 private:
  const CoreData::DataVariable *mVariable ;
  } ;

//==============================================================================

class DataSetWrapper : public QObject {
  Q_OBJECT

 public:
  DataSetWrapper(const CoreData::DataSet *pDataset)
   : mDataset(pDataset) { }

 public slots:
  const PyObject *getVariables(void) const ;

  const PyObject *getVariable(long index) const ;

  const PyObject *getVoi(void) const ;

  qulonglong getSize(void) const
    { return mDataset->getSize() ; }

  long length(void) const
    { return mDataset->length() ; }

  const PyObject *py_get_attribute(const QString &name) const ;

 private:
  const CoreData::DataSet *mDataset ;
  } ;

//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
