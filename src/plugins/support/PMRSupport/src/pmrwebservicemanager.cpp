/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// PMR web service manager
//==============================================================================

#include "corecliutils.h"
#include "coreguiutils.h"
#include "pmrauthentication.h"
#include "pmrsupportpreferenceswidget.h"
#include "pmrwebservice.h"
#include "pmrwebservicemanager.h"
#include "pmrwebserviceresponse.h"
#include "progressbarwidget.h"
#include "webviewerwidget.h"

//==============================================================================

#include <QLayout>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QWebView>

//==============================================================================

#include "o2/o1requestor.h"

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

PmrWebServiceManager::PmrWebServiceManager(const QString &pPmrUrl,
                                           PmrWebService *pPmrWebService) :
    QNetworkAccessManager(pPmrWebService),
    mPmrWebService(pPmrWebService),
    mPmrAuthentication(nullptr),
    mWebViewerDialog(nullptr),
    mWebViewer(nullptr),
    mWebViewerUsed(false)
{
    // Create our 'special' settings
    // Note: special in the sense that we don't retrieve them from the plugin
    //       itself since this is not a view, a window or anything like that...

    mSettings = new QSettings();

    mSettings->beginGroup(SettingsPlugins);
    mSettings->beginGroup(PluginName);
    mSettings->beginGroup("WebViewerDialog");

    // Make sure that we get told when there are SSL errors (which would happen
    // if the website's certificate is invalid, e.g. it has expired)

    connect(this, &PmrWebServiceManager::sslErrors,
            this, &PmrWebServiceManager::ignoreSslErrors);

    // Create, by updating ourselves, our PMR authentication object

    update(pPmrUrl);
}

//==============================================================================

PmrWebServiceManager::~PmrWebServiceManager()
{
    // Delete some interal objects

    delete mSettings;
}

//==============================================================================

bool PmrWebServiceManager::isAuthenticated() const
{
    // Return whether we are authenticated to PMR

    return mPmrAuthentication->linked();
}

//==============================================================================

void PmrWebServiceManager::authenticate(bool pAuthenticate)
{
    // Authenticate ourselves to PMR

    mWebViewerUsed = false;

    if (pAuthenticate)
        mPmrAuthentication->link();
    else
        mPmrAuthentication->unlink();
}

//==============================================================================

void PmrWebServiceManager::authenticationSucceeded()
{
    // Let people know whether we are really authenticated, i.e. linked

    emit authenticated(qobject_cast<PmrAuthentication *>(sender())->linked());
}

//==============================================================================

void PmrWebServiceManager::authenticationFailed()
{
    // Let the user know if s/he tried to authenticate him/herself but was just
    // not able to do so

    if (!mWebViewerUsed) {
        Core::warningMessageBox(tr("PMR Authentication"),
                                 tr("PMR Authentication could not be performed.")+"<br/><br/>"
                                +tr("<strong>Note:</strong> you might want to check that your system time is correct."));
    }

    // Let people know that authentication failed

    emit authenticated(false);
}

//==============================================================================

void PmrWebServiceManager::openBrowser(const QUrl &pUrl)
{
    // Open the given URL in a temporary web browser of ours

    mWebViewerUsed = true;

    if (!mWebViewerDialog) {
        mWebViewerDialog = new Core::Dialog(mSettings, Core::mainWindow());

        connect(mWebViewerDialog, &Core::Dialog::rejected,
                this, &PmrWebServiceManager::authenticationCancelled);

        mWebViewer = new WebViewerWidget::WebViewerWidget(mWebViewerDialog);

        mWebViewer->webView()->setContextMenuPolicy(Qt::NoContextMenu);

        mWebViewer->showProgressBar();

        QVBoxLayout *layout = new QVBoxLayout(mWebViewerDialog);

        layout->setContentsMargins(QMargins());

        layout->addWidget(mWebViewer);

        mWebViewerDialog->setLayout(layout);
    } else {
        mWebViewer->goToHomePage();
    }

    mWebViewerDialog->setWindowTitle(tr("PMR Authentication"));

    QMainWindow *mainWindow = Core::mainWindow();

    mWebViewerDialog->resize(int(0.7*mainWindow->width()),
                             int(0.7*mainWindow->height()));

    mWebViewer->webView()->load(pUrl);

    mWebViewerDialog->exec();
}

//==============================================================================

void PmrWebServiceManager::closeBrowser()
{
    // Close our temporary web browser, but only if the current page has
    // finished loading otherwise try again in a bit

    if (qFuzzyIsNull(mWebViewer->progressBarWidget()->value()))
        mWebViewerDialog->close();
    else
        QTimer::singleShot(169, this, &PmrWebServiceManager::closeBrowser);
}

//==============================================================================

void PmrWebServiceManager::ignoreSslErrors(QNetworkReply *pNetworkReply,
                                           const QList<QSslError> &pSslErrors)
{
    // Ignore SSL errors since we trust the website and therefore its
    // certificate (even if it is invalid, e.g. it has expired)

    pNetworkReply->ignoreSslErrors(pSslErrors);
}

//==============================================================================

PmrWebServiceResponse * PmrWebServiceManager::request(const QString &pUrl,
                                                      bool pSecureRequest,
                                                      bool pUsePost,
                                                      const QJsonDocument &pJsonDocument)
{
    // Check that we are connected to the Internet

    if (!Core::internetConnectionAvailable()) {
        emit error(Core::noInternetConnectionAvailableMessage());

        return nullptr;
    }

    // Let the user know that we are (going to be) busy

    emit busy(true);

    // Send our request to PMR, asking for the response to be compressed

    QNetworkRequest networkRequest;

    networkRequest.setRawHeader("Accept", "application/json");
    networkRequest.setRawHeader("Accept-Encoding", "gzip");

    networkRequest.setUrl(QUrl(pUrl));

    // Use the authenticated link if it's available

    QNetworkReply *networkReply;

    if (pSecureRequest && mPmrAuthentication->linked()) {
        O1Requestor *requestor = new O1Requestor(this, mPmrAuthentication, this);

        if (!pUsePost && pJsonDocument.isEmpty()) {
            networkReply = requestor->get(networkRequest, QList<O0RequestParameter>());
        } else {
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, RequestMimeType);

            networkReply = requestor->post(networkRequest, QList<O0RequestParameter>(),
                                           pJsonDocument.isNull()?
                                               QByteArray():
                                               pJsonDocument.toJson(QJsonDocument::Compact));
        }
    } else {
        if (!pUsePost && pJsonDocument.isEmpty()) {
            networkReply = get(networkRequest);
        } else {
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, RequestMimeType);

            networkReply = post(networkRequest,
                                pJsonDocument.isNull()?
                                    QByteArray():
                                    pJsonDocument.toJson(QJsonDocument::Compact));
        }
    }

    PmrWebServiceResponse *pmrWebServiceResponse = new PmrWebServiceResponse(networkReply);

    connect(pmrWebServiceResponse, &PmrWebServiceResponse::busy,
            mPmrWebService, &PmrWebService::busy);
    connect(pmrWebServiceResponse, &PmrWebServiceResponse::error,
            mPmrWebService, &PmrWebService::error);
    connect(pmrWebServiceResponse, &PmrWebServiceResponse::forbidden,
            mPmrWebService, &PmrWebService::forbidden);

    return pmrWebServiceResponse;
}

//==============================================================================

void PmrWebServiceManager::update(const QString &pPmrUrl)
{
    // Delete any previous instance of our PMR authentication object

    if (mPmrAuthentication)
        delete mPmrAuthentication;

    // Create our PMR authentication object

    mPmrAuthentication = new PmrAuthentication(pPmrUrl, this);

    // Connect some signals

    connect(mPmrAuthentication, &PmrAuthentication::linkingSucceeded,
            this, &PmrWebServiceManager::authenticationSucceeded);
    connect(mPmrAuthentication, &PmrAuthentication::linkingFailed,
            this, &PmrWebServiceManager::authenticationFailed);

    connect(mPmrAuthentication, &PmrAuthentication::openBrowser,
            this, &PmrWebServiceManager::openBrowser);
    connect(mPmrAuthentication, &PmrAuthentication::closeBrowser,
            this, &PmrWebServiceManager::closeBrowser);
}

//==============================================================================

}   // namespace PMRSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
