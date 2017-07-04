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
// Simulation Support functions
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfile.h"
#include "cellmlfileruntime.h"
#include "combinearchive.h"
#include "corecliutils.h"
#include "sedmlfile.h"
#include "solverinterface.h"

//==============================================================================

#include <QIcon>
#include <QString>

//==============================================================================

namespace libsedml {
    class SedAlgorithm;
}   // namespace libsedml

//==============================================================================

namespace OpenCOR {
namespace SimulationSupport {

//==============================================================================

enum FileType {
    CellmlFile,
    SedmlFile,
    CombineArchive
};

//==============================================================================

QIcon parameterIcon(const CellMLSupport::CellmlFileRuntimeParameter::ParameterType &pParameterType);

QString retrieveFileDetails(const QString &pFileName,
                            CellMLSupport::CellmlFile *&pCellmlFile,
                            SEDMLSupport::SedmlFile *&pSedmlFile,
                            COMBINESupport::CombineArchive *&pCombineArchive,
                            FileType &pFileType,
                            SEDMLSupport::SedmlFileIssues &pSedmlFileIssues,
                            COMBINESupport::CombineArchiveIssues &pCombineArchiveIssues);

//==============================================================================

}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
