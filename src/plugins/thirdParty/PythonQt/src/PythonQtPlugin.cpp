//==============================================================================
// CoreStore plugin
//==============================================================================

#include "PythonQtPlugin.h"

//==============================================================================

namespace OpenCOR {
namespace PythonQt {

//==============================================================================

PLUGININFO_FUNC PythonQtPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("the Python Qt store plugin."));
    descriptions.insert("fr", QString::fromUtf8("the Python Qt store plugin."));

    return new PluginInfo(PluginInfo::ThirdParty, false, false,
                          QStringList(),
                          descriptions);
}

//==============================================================================

}   // namespace PythonQt
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
