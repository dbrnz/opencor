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
// PMR Workspaces window new workspace dialog
//==============================================================================

#include "coreguiutils.h"
#include "pmrwebservice.h"
#include "pmrworkspace.h"
#include "pmrworkspaceswindownewworkspacedialog.h"

//==============================================================================

#include <QDir>
#include <QPushButton>
#include <QString>

//==============================================================================

#include "ui_pmrworkspaceswindownewworkspacedialog.h"

//==============================================================================

namespace OpenCOR {
namespace PMRWorkspacesWindow {

//==============================================================================

PmrWorkspacesWindowNewWorkspaceDialog::PmrWorkspacesWindowNewWorkspaceDialog(QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::PmrWorkspacesWindowNewWorkspaceDialog)
{
    // Set up the GUI

    mGui->setupUi(this);

#ifdef Q_OS_MAC
    mGui->titleValue->setAttribute(Qt::WA_MacShowFocusRect, false);
    mGui->folderValue->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

    // The Ok button is disabled to start with

    mGui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // Connect some signals

    connect(mGui->titleValue, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateOkButton()));
    connect(mGui->folderValue, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateOkButton()));

    connect(mGui->folderToolButton, SIGNAL(clicked()),
            this, SLOT(choosePath()));

    connect(mGui->buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(mGui->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
}

//==============================================================================

PmrWorkspacesWindowNewWorkspaceDialog::~PmrWorkspacesWindowNewWorkspaceDialog()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void PmrWorkspacesWindowNewWorkspaceDialog::retranslateUi()
{
    // Retranslate our GUI

    mGui->retranslateUi(this);
}

//==============================================================================

const QString PmrWorkspacesWindowNewWorkspaceDialog::title() const
{
    // Return our title

    return mGui->titleValue->text().trimmed();
}

//==============================================================================

const QString PmrWorkspacesWindowNewWorkspaceDialog::description() const
{
    // Return our description

    return mGui->descriptionValue->toPlainText().trimmed();
}

//==============================================================================

const QString PmrWorkspacesWindowNewWorkspaceDialog::path() const
{
    // Return our path

    return mGui->folderValue->text();
}

//==============================================================================

void PmrWorkspacesWindowNewWorkspaceDialog::updateOkButton()
{
    // Enable the Ok button only if we have both a title and a folder

    mGui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(   !mGui->titleValue->text().trimmed().isEmpty()
                                                              && !mGui->folderValue->text().trimmed().isEmpty());
}

//==============================================================================

void PmrWorkspacesWindowNewWorkspaceDialog::choosePath()
{
    // Choose an empty path for our new workspace

    QString dirName = PMRSupport::PmrWebService::getEmptyDirectory();

    if (!dirName.isEmpty()) {
        mGui->folderValue->setText(dirName);

        mGui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!title().isEmpty());
    } else {
        mGui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

//==============================================================================

}   // namespace PMRWorkspacesWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================