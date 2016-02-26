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
// BioSignalML data store exporter class
//==============================================================================

#include "biosignalmldatastoreexporter.h"
#include "biosignalml/biosignalml.h"
#include "biosignalml/data/hdf5.h"

//==============================================================================

#include "corecliutils.h"

//==============================================================================

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QUrl>

//==============================================================================

namespace OpenCOR {
namespace BioSignalMLDataStore {

//==============================================================================

BioSignalMLExporter::BioSignalMLExporter(const QString &pId) :
    DataStore::DataStoreExporter(pId),
    mSaveDialog(new BioSignalMLSaveDialog())
{
}

//==============================================================================

void BioSignalMLExporter::execute(const QString &pFileName,
                                  DataStore::DataStore *pDataStore) const
{
    // Export the given data store to a BioSignalML file

    QString comment = QObject::tr("Generated by") + " " + Core::version()
                    + " " + QObject::tr("at") + " " + QDateTime::currentDateTimeUtc().toString(Qt::ISODate)
                    + " " + QObject::tr("from") + " " + pDataStore->uri();
    mSaveDialog->setComment(comment);
    mSaveDialog->setDefaultFileName(Core::newFileName(pFileName, "", false, "biosignalml"));
    DataStore::DataStoreVariables variables = pDataStore->variables();
    mSaveDialog->setSelectedVariables(variables);

    if (mSaveDialog->run()) {

        QString fileName = mSaveDialog->fileName();
        std::string rec_uri = QUrl::fromLocalFile(fileName).toEncoded().toStdString();
        std::string base_units = pDataStore->uri().toStdString() + "/units#";

        bsml::HDF5::Recording *recording = nullptr;
        try {
            recording = new bsml::HDF5::Recording(rec_uri, fileName.toStdString(), true);
            recording->set_comment(comment.toStdString()); // Set language in rdf::Literal()
            // Requires https://github.com/dbrnz/typedobject/issues/6
            // Language code is QLocale::bcp47Name().toStdString()
            recording->set_label(mSaveDialog->shortName().toStdString()) ;
            recording->set_description(mSaveDialog->description().toStdString()) ;
            recording->set_investigator(rdf::Literal(mSaveDialog->author().toStdString())) ;

            recording->add_prefix(rdf::Namespace("units", base_units)) ;

            DataStore::DataStoreVariable *voi = pDataStore->voi();
            auto clock = recording->new_clock(rec_uri + "/clock/" + voi->uri().toStdString(),
                                              rdf::URI(base_units + voi->unit().toStdString()),
                                              voi->values(), voi->size());
            clock->set_label(voi->label().toStdString()) ;

            double duration = voi->value(voi->size()-1) - voi->value(0);
            recording->set_duration(xsd::Duration(duration, voi->unit().toStdString()));

            std::vector<std::string> uris;
            std::vector<rdf::URI> units;
            std::vector<size_t> varindex;
            size_t nvars = variables.size();
            for (size_t i = 0 ;  i < nvars ;  ++i) {
                if (mSaveDialog->selectedVariable(i)) {
                    uris.push_back(rec_uri + "/signal/" + variables[i]->uri().toStdString());
                    units.push_back(rdf::URI(base_units + variables[i]->unit().toStdString()));
                    varindex.push_back(i);
                }
            }
            auto sigs = recording->new_signalarray(uris, units, clock) ;
            nvars = sigs->size();
            for (size_t i = 0 ;  i < nvars ;  ++i) {
                (*sigs)[i]->set_label(variables[varindex[i]]->label().toStdString()) ;
            }

#define BUFFER_ROWS 50000
            double *data = new double[BUFFER_ROWS*nvars];
            double *dp = data;
            int rowcount = 0;

            for (qulonglong i = 0; i < pDataStore->size(); ++i) {
                for (size_t j = 0 ;  j < nvars ;  ++j) {
                    *dp++ = variables[varindex[j]]->value(i);
                    }
                ++rowcount;
                if (rowcount >= BUFFER_ROWS) {
                    sigs->extend(data, BUFFER_ROWS*nvars);
                    dp = data;
                    rowcount = 0;
                }

                qApp->processEvents();
//---GRY--- THE CALL TO qApp->processEvents() SHOULD BE REMOVED AND THE EXPORTER
//              BE SUCH THAT IT DOESN'T BLOCK THE MAIN THREAD (E.G. WHEN EXPORTING
//              LONG SIMULATIONS). MAYBE THIS COULD BE DONE BY MAKING THE EXPORTER
//              WORK IN ITS OWN THREAD?...
            }

            sigs->extend(data, rowcount*nvars);

            delete[] data;
        }

        catch (bsml::data::Exception e) {
            std::cerr << "EXCEPTION: " << e.what() << std::endl;
                                        // **** Need to bring up alert....
        }

        if (recording != nullptr) {
            recording->close();
            delete recording;
        }

    }

}

//==============================================================================

}   // namespace BioSignalMLDataStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
