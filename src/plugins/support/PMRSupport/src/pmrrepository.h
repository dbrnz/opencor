/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// PMR repository
//==============================================================================

#pragma once

//==============================================================================

#include "pmrauthentication.h"
#include "pmrexposure.h"
#include "pmrsupportglobal.h"
#include "pmrworkspace.h"

//==============================================================================

#include <QList>
#include <QObject>
#include <QString>

//==============================================================================

class QNetworkAccessManager;
class QNetworkReply;

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

class PmrRepositoryManager;

//==============================================================================

class PMRSUPPORT_EXPORT PmrRepository : public QObject
{
    Q_OBJECT

public:
    explicit PmrRepository(QObject *parent = 0);
    ~PmrRepository();

    static const QString Url(void);
    static const QByteArray MimeType(void);

    void requestExposureFiles(const QString &pUrl);
    void requestExposuresList(void);

    void requestWorkspacesList(void);
    void requestWorkspaceInformation(const QString &pUrl);

    void requestCloneExposureWorkspace(const QString &pUrl, const QString &pDirName);
    void requestNewWorkspace(const QString &pName, const QString &pDescription,
                             const QString &pDirName);

private:

    enum Action {
        None,
        CloneExposureWorkspace,
        RequestExposureFiles
    };

    PmrRepositoryManager *mPmrRepositoryManager;

    QMap<QString, PmrExposure *> mExposures;    // Exposure Url --> Exposure

    void emitInformation(const QString &pMessage);
    QString informationNoteMessage() const;

    void requestExposureFileInformation(PmrExposure *pExposure, const QString &pUrl);
    void requestExposureInformation(PmrExposure *pExposure, const Action &pNextAction);

    void requestCloneWorkspace(PmrWorkspace *pWorkspace, const QString &pDirName);
    void requestWorkspaceInformation(const QString &pUrl, const QString &pDirName,
                                     PmrExposure *pExposure=nullptr);

Q_SIGNALS:
    void authenticated(const bool &pAuthenticated);
    //void authenticationChanged(const bool &pAuthenticated);

    void busy(const bool &pBusy);

    void error(const QString &pErrorMessage, const bool &pInternetConnectionAvailable);
    void information(const QString &pMessage);
    void warning(const QString &pMessage);

    void exposureFilesList(const QString &pUrl, const QStringList &pExposureFiles);
    void exposuresList(const PMRSupport::PmrExposureList &pExposureList);

    void workspaceCloned(PMRSupport::PmrWorkspace *pWorkspace);
    void workspaceCreated(const QString &pUrl);
    void workspaceInformation(const QString &pUrl, const QString &pName,
                              const QString &pDescription, const QString &pOwner);
    void workspacesList(const PMRSupport::PmrWorkspaceList &pWorkspaceList);

public Q_SLOTS:
    void authenticate(const bool &pLink = true);
    void getAuthenticationStatus(void);

private Q_SLOTS:
    void exposureFileInformationResponse(const QJsonDocument &pJsonDocument);
    void exposureInformationResponse(const QJsonDocument &pJsonDocument);
    void exposuresListResponse(const QJsonDocument &pJsonDocument);

    void workspaceInformationResponse(const QJsonDocument &pJsonDocument);
    void workspacesListResponse(const QJsonDocument &pJsonDocument);

    void cloneWorkspaceResponse(PmrWorkspace *pWorkspace);
    void createWorkspaceResponse(const QString &pUrl);
};

//==============================================================================

}   // namespace PMRWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================