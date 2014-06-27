//==============================================================================
// Python Window plugin
//==============================================================================

#ifndef PYTHONWINDOWPLUGIN_H
#define PYTHONWINDOWPLUGIN_H

//==============================================================================

#include "plugininfo.h"
#include "i18ninterface.h"
#include "plugininterface.h"
#include "windowinterface.h"

//==============================================================================

namespace OpenCOR {
namespace PythonWindow {

//==============================================================================

PLUGININFO_FUNC PythonWindowPluginInfo();

//==============================================================================

class PythonWindow;

//==============================================================================

class PythonWindowPlugin : public QObject, public I18nInterface,
                           public PluginInterface, public WindowInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.PythonWindowPlugin" FILE "pythonwindowplugin.json")

    Q_INTERFACES(OpenCOR::PluginInterface)
    Q_INTERFACES(OpenCOR::WindowInterface)
    Q_INTERFACES(OpenCOR::I18nInterface)
    Q_INTERFACES(OpenCOR::PluginInterface)
    Q_INTERFACES(OpenCOR::WindowInterface)

public:
#include "i18ninterface.inl"
#include "plugininterface.inl"
#include "windowinterface.inl"

private:
    QAction *mPythonWindowAction;
 
    PythonWindow *mPythonWindow;
};

//==============================================================================

}   // namespace PythonWindow
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
