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
// CLI utilities
//==============================================================================

#ifndef CLIUTILS_H
#define CLIUTILS_H

//==============================================================================

#include "coreglobal.h"
#include "plugin.h"

//==============================================================================

#include <QByteArray>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

static const QString SettingsGlobal = "Global";

static const QString SettingsLocale = "Locale";

static const QString SettingsActiveDirectory = "ActiveDirectory";

//==============================================================================

QString CORE_EXPORT locale();

qulonglong totalMemory();
qulonglong CORE_EXPORT freeMemory();

QString CORE_EXPORT sizeAsString(const double &pSize,
                                 const int &pPrecision = 1);

QByteArray CORE_EXPORT resourceAsByteArray(const QString &pResource);
bool CORE_EXPORT saveResourceAs(const QString &pResource,
                                const QString &pFilename);

void CORE_EXPORT * globalInstance(const QString &pObjectName,
                                  void *pDefaultGlobalInstance);

void CORE_EXPORT setActiveDirectory(const QString &pDirName);
QString CORE_EXPORT activeDirectory();

QString CORE_EXPORT nativeCanonicalFileName(const QString &pFileName);

QString CORE_EXPORT formatErrorMsg(const QString &pErrorMsg);

QString CORE_EXPORT nonDiacriticString(const QString &pString);

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
