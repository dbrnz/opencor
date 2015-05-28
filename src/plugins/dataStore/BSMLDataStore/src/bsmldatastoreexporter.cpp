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

#include "corecliutils.h"
#include "coreguiutils.h"
#include "coredatastore.h"
#include "bsmldatastoreexporter.h"
#include "datastorevariable.h"

//==============================================================================

#include <biosignalml.h>
#include <data/hdf5.h>              // ****
//#include <biosignalml/data/hdf5.h>

//==============================================================================

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <iostream>

//==============================================================================

namespace OpenCOR {
namespace BSMLDataStore {

//==============================================================================

void BioSignalMLExporter::execute(CoreDataStore::CoreDataStore *pDataStore) const
{
    // Export the given data store to a BioSignalML file

    QString fileName = Core::getSaveFileName(QObject::tr("Export to a BioSignalML file"),
                                             QString(),
                                             QObject::tr("BioSignalML")+" (*.h5)");

// Repository v's file -- user input
// ***** Also capture URI, comments, etc...



    if (!fileName.isEmpty()) {


        // To come from user dialog... (And restrict to http: scheme ??)
        // Or use URI in repository??
        std::string rec_uri = pDataStore->modelUri().toStdString();

        std::string base_units = "http://www.cellml.org/units/"
                                + pDataStore->modelId().toStdString()
                                + "#";

        bsml::HDF5::Recording *recording = nullptr;
        try {
            recording = new bsml::HDF5::Recording(rec_uri, fileName.toStdString(), true);

            recording->add_namespace(rdf::Namespace("units", base_units)) ;
//            recording->set_description("testing 123...") ;

            CoreDataStore::DataStoreVariable *voi = pDataStore->voi();
            auto clock = recording->new_clock(voi->uri().toStdString(),
                                              rdf::URI(base_units + voi->unit().toStdString()),
                                              voi->values(), voi->size());
            clock->set_label(voi->label().toStdString()) ;
//std::cout << clock->uri() << "  |  " << clock->units() << "  |  " << clock->label() << std::endl ;

            CoreDataStore::DataStoreVariables variables = pDataStore->variables();
            auto variableBegin = variables.constBegin();
            auto variableEnd = variables.constEnd();

            std::vector<const std::string> uris;
            std::vector<const rdf::URI> units;
            for (auto variable = variableBegin; variable != variableEnd; ++variable) {
                uris.push_back((*variable)->uri().toStdString());
                units.push_back(rdf::URI(base_units + (*variable)->unit().toStdString()));
            }

            auto sigs = recording->new_signalarray(uris, units, clock) ;
            size_t nvars = variables.size();
            for (size_t i = 0 ;  i < nvars ;  ++i) {
                (*sigs)[i]->set_label(variables[i]->label().toStdString()) ;
//std::cout << (*sigs)[i]->uri()   << "  |  "
//          << (*sigs)[i]->units() << "  |  "
//          << (*sigs)[i]->label() << std::endl ;
            }

#define BUFFER_ROWS 50000
            double *data = new double[BUFFER_ROWS*nvars];
            double *dp = data;
            int rowcount = 0;

            for (qulonglong i = 0; i < pDataStore->size(); ++i) {
                for (auto variable = variableBegin; variable != variableEnd; ++variable)
                    *dp++ = (*variable)->value(i);
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

}   // namespace BSMLDataStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
