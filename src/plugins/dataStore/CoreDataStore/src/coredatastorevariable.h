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
// Core data store variable class
//==============================================================================

#ifndef COREDATASTOREVARIABLE_H
#define COREDATASTOREVARIABLE_H

//==============================================================================

#include "coredatastoreglobal.h"

//==============================================================================

#include <QtGlobal>

//==============================================================================

#include <QString>

//==============================================================================

namespace OpenCOR {
namespace CoreDataStore {

//==============================================================================

class COREDATASTORE_EXPORT CoreDataStoreVariable
{
public:
    explicit CoreDataStoreVariable(const qulonglong &pSize,
                                   const double *pValue = 0);
    virtual ~CoreDataStoreVariable();

    void setUri(const QString &pUri);
    void setUnits(const QString &pUnits);
    void setLabel(const QString &pLabel);

    QString getUri() const;
    QString getLabel() const;
    QString getUnits() const;

    void savePoint(const qulonglong &pPosition);
    void savePoint(const qulonglong &pPosition, const double &pValue);

    double getPoint(const qulonglong &pPosition) const;
    const double *getData() const;
    qulonglong getSize() const;

private:
    QString mUri;
    QString mUnits;
    QString mLabel;
    const double *mValue;
    double *mData;
    qulonglong mSize;
};

//==============================================================================

typedef QVector<CoreDataStoreVariable *> CoreDataStoreVariables;

//==============================================================================

}   // namespace CoreDataStore
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
