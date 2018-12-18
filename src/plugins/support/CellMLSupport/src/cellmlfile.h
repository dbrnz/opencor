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
// CellML file
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfileissue.h"
#include "cellmlfilerdftriple.h"
#include "cellmlfileruntime.h"
#include "cellmlsupportglobal.h"
#include "standardfile.h"

//==============================================================================

#include <QDomElement>
#include <QMap>

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

static const auto Cellml_1_0       = QStringLiteral("1.0");
static const auto Cellml_1_1       = QStringLiteral("1.1");
static const auto CellmlRefVersion = Cellml_1_1;

//==============================================================================

static const auto Cellml_1_0_Namespace = QStringLiteral("http://www.cellml.org/cellml/1.0#");
static const auto Cellml_1_1_Namespace = QStringLiteral("http://www.cellml.org/cellml/1.1#");
static const auto TmpDocumentation     = QStringLiteral("http://cellml.org/tmp-documentation");
static const auto CmetaIdNamespace     = QStringLiteral("http://www.cellml.org/metadata/1.0#");
static const auto MathmlNamespace      = QStringLiteral("http://www.w3.org/1998/Math/MathML");
static const auto RdfNamespace         = QStringLiteral("http://www.w3.org/1999/02/22-rdf-syntax-ns#");
static const auto XlinkNamespace       = QStringLiteral("http://www.w3.org/1999/xlink");

//==============================================================================

struct CellmlFileException
{
public:
    CellmlFileException(const QString &pMessage);

    QString message() const;

private:
    QString mMessage;
};

//==============================================================================

class CELLMLSUPPORT_EXPORT CellmlFile : public StandardSupport::StandardFile
{
    Q_OBJECT

public:
    enum Version {
        Unknown,
        Cellml_1_0,
        Cellml_1_1
    };

    explicit CellmlFile(const QString &pFileName);
    ~CellmlFile() override;

    iface::cellml_api::Model * model();

    iface::rdf_api::DataSource * rdfDataSource();

    bool load() override;
    bool save(const QString &pFileName = QString()) override;

    bool update(const QString &pFileName = QString());

    bool isValid(const QString &pFileContents, CellmlFileIssues &pIssues,
                 bool pWithBusyWidget = false);
    bool isValid(bool pWithBusyWidget = false);

    CellmlFileIssues issues() const;

    CellmlFileRuntime * runtime(bool pWithBusyWidget = false);

    QStringList dependencies(bool pWithBusyWidget = false);

    CellmlFileRdfTriples & rdfTriples();
    CellmlFileRdfTriples rdfTriples(iface::cellml_api::CellMLElement *pElement) const;

    CellmlFileRdfTriple * rdfTriple(iface::cellml_api::CellMLElement *pElement,
                                    CellmlFileRdfTriple::ModelQualifier pModelQualifier,
                                    const QString &pResource,
                                    const QString &pId) const;
    CellmlFileRdfTriple * rdfTriple(iface::cellml_api::CellMLElement *pElement,
                                    CellmlFileRdfTriple::BioQualifier pBioQualifier,
                                    const QString &pResource,
                                    const QString &pId) const;

    CellmlFileRdfTriple * addRdfTriple(iface::cellml_api::CellMLElement *pElement,
                                       CellmlFileRdfTriple::ModelQualifier pModelQualifier,
                                       const QString &pResource,
                                       const QString &pId);
    CellmlFileRdfTriple * addRdfTriple(iface::cellml_api::CellMLElement *pElement,
                                       CellmlFileRdfTriple::BioQualifier pBioQualifier,
                                       const QString &pResource,
                                       const QString &pId);

    bool removeRdfTriple(iface::cellml_api::CellMLElement *pElement,
                         CellmlFileRdfTriple::ModelQualifier pModelQualifier,
                         const QString &pResource, const QString &pId);
    bool removeRdfTriple(iface::cellml_api::CellMLElement *pElement,
                         CellmlFileRdfTriple::BioQualifier pBioQualifier,
                         const QString &pResource, const QString &pId);

    QStringList importedFileNames() const;

    QString importedFileContents(const QString &pImportedFileName) const;

    QString cmetaId();
    QString xmlBase();

    bool exportTo(const QString &pFileName, Version pVersion,
                  bool pWithBusyWidget = false);
    bool exportTo(const QString &pFileName,
                  const QString &pUserDefinedFormatFileName,
                  bool pWithBusyWidget = false);

    Version version();

    static Version modelVersion(iface::cellml_api::Model *pModel);
    static Version fileVersion(const QString &pFileName);
    static Version fileContentsVersion(const QString &pFileContents);

    static QString versionAsString(Version pVersion);

private:
    ObjRef<iface::cellml_api::Model> mModel;

    ObjRef<iface::rdf_api::RDFAPIRepresentation> mRdfApiRepresentation;
    ObjRef<iface::rdf_api::DataSource> mRdfDataSource;

    CellmlFileRdfTriples mRdfTriples;

    CellmlFileIssues mIssues;

    bool mLoadingNeeded;
    bool mFullInstantiationNeeded;
    bool mDependenciesNeeded;

    QMap<QString, QString> mImportContents;

    QStringList mUsedCmetaIds;

    bool mUpdated;

    void reset() override;

    void retrieveImports(const QString &pXmlBase,
                         iface::cellml_api::Model *pModel,
                         QList<iface::cellml_api::CellMLImport *> &pImportList,
                         QStringList &pImportXmlBaseList);

    bool fullyInstantiateImports(iface::cellml_api::Model *pModel,
                                 CellmlFileIssues &pIssues,
                                 bool pWithBusyWidget = false);

    bool load(const QString &pFileContents,
              ObjRef<iface::cellml_api::Model> *pModel,
              CellmlFileIssues &pIssues);

    bool isValid(const QString &pFileContents,
                 ObjRef<iface::cellml_api::Model> *pModel,
                 CellmlFileIssues &pIssues, bool pWithBusyWidget);

    void retrieveCmetaIdsFromCellmlElement(iface::cellml_api::CellMLElement *pElement);
    void clearCmetaIdsFromCellmlElement(const QDomElement &pElement,
                                        const QStringList &pUsedCmetaIds);

    bool isValid(iface::cellml_api::Model *pModel, CellmlFileIssues &pIssues);

    CellmlFileRdfTriple * rdfTriple(iface::cellml_api::CellMLElement *pElement,
                                    const QString &pQualifier,
                                    const QString &pResource,
                                    const QString &pId) const;

    QString rdfTripleSubject(iface::cellml_api::CellMLElement *pElement);
};

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
