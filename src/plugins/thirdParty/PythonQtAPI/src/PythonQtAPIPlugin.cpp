//==============================================================================
// Python Qt plugin
//==============================================================================

#include "PythonQtAPIPlugin.h"

//==============================================================================

namespace OpenCOR {
namespace PythonQtAPI {

//==============================================================================

PLUGININFO_FUNC PythonQtAPIPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python Qt store plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Python Qt store plugin."));

    return new PluginInfo(PluginInfo::ThirdParty, false, false,
                          QStringList(),
                          descriptions);
}

//==============================================================================

}   // namespace PythonQtAPI
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
