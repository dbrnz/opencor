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
// PMR workspace
//==============================================================================

#pragma once

//==============================================================================

#include "pmrsupportglobal.h"

//==============================================================================

#include <QList>
#include <QObject>
#include <QString>

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

class PMRSUPPORT_EXPORT PmrWorkspace : public QObject
{
    Q_OBJECT

public:
    explicit PmrWorkspace(QObject *parent=0);
    PmrWorkspace(const QString &pUrl, const QString &pName, QObject *parent);

    static bool compare(const PmrWorkspace *pFirst, const PmrWorkspace *pSecond);

    bool isLocal(void) const;
    bool isNull(void) const;

    void setDescription(const QString &pDescription);
    void setName(const QString &pName);
    void setOwner(const QString &pOwner);

    QString description() const;
    QString name() const;
    QString owner() const;
    QString path() const;
    QString url() const;

    void clone(const QString &pDirName);

    static const QString WorkspacesDirectory ;

private:
    QString mDescription;
    QString mName;
    QString mOwner;
    QString mPath;
    QString mUrl;

Q_SIGNALS:
    void warning(const QString &pMessage) const;
    void workspaceCloned(PmrWorkspace *pWorkspace);
};

//==============================================================================
//==============================================================================

class PmrWorkspaceList : public QList<PmrWorkspace *>
{
public:
    PmrWorkspaceList();

    void add(const QString &pUrl, const QString &pName, QObject *parent);
};

//==============================================================================

}   // namespace PMRSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================