//==============================================================================
// CoreStore plugin
//==============================================================================

#include "pythonstoreplugin.h"

//==============================================================================

namespace OpenCOR {
namespace PythonStore {

//==============================================================================

PLUGININFO_FUNC PythonStorePluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python data store wrapper."));
    descriptions.insert("fr", QString::fromUtf8("the Python data store wrapper."));

    return new PluginInfo(PluginInfo::Datastore, true, false,
                          QStringList() << "Core" << "CoreData" << "PythonQtAPI",
                          descriptions);
}

//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
