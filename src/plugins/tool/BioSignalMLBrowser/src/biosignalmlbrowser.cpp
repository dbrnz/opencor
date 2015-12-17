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
// BioSignalML browser class
//==============================================================================

#include "biosignalmlbrowser.h"
#include "browser/browser.h"

#include "biosignalml/data/hdf5.h"

//==============================================================================

#include <QApplication>

//==============================================================================

#include <vector>
#include <cmath>
#include <iostream>
#include <exception>

//==============================================================================

namespace OpenCOR {
namespace BioSignalMLBrowser {

//==============================================================================

BioSignalMLBrowser::BioSignalMLBrowser(QObject * parent)
: QObject(parent) 
{
}

//==============================================================================
 
BioSignalMLBrowser::~BioSignalMLBrowser()
{
}

//==============================================================================
 
void BioSignalMLBrowser::runHelpCommand()
{
    std::cout << "Commands supported by BioSignalMLBrowser:" << std::endl;
    std::cout << " * Display this message:" << std::endl;
    std::cout << "      help" << std::endl;
    std::cout << " * Browse a BioSignalML dataset:" << std::endl;
    std::cout << "      browse DATASET [START [DURATION]]" << std::endl;
}

//==============================================================================

int BioSignalMLBrowser::runBioSignalMLBrowser(const QStringList &pArguments)
{
    if (pArguments.count() < 1) {
        runHelpCommand();
        return -1;
    }

    const QString uri = pArguments[0];

    float start = 0.0;
    float end = NAN;
    bool ok = false;
    if (pArguments.count() >= 2) {
        start = pArguments[1].toFloat(&ok);
        if (!ok) {
            std::cerr << "Invalid start time" << std::endl;
            return -1;
        }
    }
    if (pArguments.count() >= 3) {
        end = pArguments[2].toFloat(&ok);
        if (!ok) {
            std::cerr << "Invalid duration" << std::endl;
            return -1;
        }
    }

    auto semantic_tags = browser::StringDictionary{};
    bsml::HDF5::Recording::Ptr hdf5 = nullptr;
    try {
        if (uri.startsWith("http://")) {
//TODO      store = biosignalml.client.Repository(uri);
//TODO      recording = store.get_recording(uri);
//TODO      semantic_tags = store.get_semantic_tags();
        }
        else {                        //open ??
            hdf5 = bsml::HDF5::Recording::create(uri.toStdString(), false);  // Open for reading, read/write
            semantic_tags = browser::StringDictionary {  // Load from file
                {"http://standards/org/ontology#tag1", "Tag 1"},
                {"http://standards/org/ontology#tag2", "Tag 2"},
                {"http://standards/org/ontology#tag3", "Tag 3"},
                {"http://standards/org/ontology#tag4", "Tag 4"}
            };
        }
    }
    catch (std::exception &e) {
        //throw;  //##################
        qCritical("Exception: %s", e.what());
        return -1;
    }

    auto viewer = new browser::Browser(hdf5, start, end, semantic_tags); //TODO, annotator=wfdbAnnotation);
    viewer->show();
    return 0;
}

//==============================================================================

void BioSignalMLBrowser::browseBioSignalML()
{
// get args from UI and call runBioSignalMLBrowser()
// But then error messages need to go to an alert box, not std::cerr
    runBioSignalMLBrowser(QStringList() << "/Users/dave/biosignalml/libbsml/osx/test.h5");
    runBioSignalMLBrowser(QStringList() << "/Users/dave/biosignalml/workspace/cellml/workspaces/dbrooks/noble62/noble_1962_data.bsml");
}

//==============================================================================

}   // namespace BioSignalMLBrowser
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
