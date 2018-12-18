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
// COMBINE file manager
//==============================================================================

#pragma once

//==============================================================================

#include "combinearchive.h"
#include "combinesupportglobal.h"
#include "standardfilemanager.h"

//==============================================================================

namespace OpenCOR {
namespace COMBINESupport {

//==============================================================================

class COMBINESUPPORT_EXPORT CombineFileManager : public StandardSupport::StandardFileManager
{
    Q_OBJECT

public:
    static CombineFileManager * instance();

    bool isCombineArchive(const QString &pFileName) const;

    CombineArchive * combineArchive(const QString &pFileName) const;

protected:
    bool canLoad(const QString &pFileName) const override;

    StandardSupport::StandardFile * create(const QString &pFileName) const override;
};

//==============================================================================

}   // namespace COMBINESupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
