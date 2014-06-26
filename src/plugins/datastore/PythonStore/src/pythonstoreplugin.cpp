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

    descriptions.insert("en", QString::fromUtf8("the core data store plugin."));
    descriptions.insert("fr", QString::fromUtf8("the core data store plugin."));

    return new PluginInfo(PluginInfo::Datastore, false, false,
                          QStringList(),
                          descriptions);
}

//==============================================================================

}   // namespace PythonStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
