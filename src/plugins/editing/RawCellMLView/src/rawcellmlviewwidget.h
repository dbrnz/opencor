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
// Raw CellML view widget
//==============================================================================

#ifndef RAWCELLMLVIEWWIDGET_H
#define RAWCELLMLVIEWWIDGET_H

//==============================================================================

#include "cliutils.h"
#include "viewwidget.h"

//==============================================================================

#include <QDomNode>
#include <QMap>

//==============================================================================

namespace Ui {
    class RawCellmlViewWidget;
}

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class XslTransformer;
}   // namespace Core

//==============================================================================

namespace CoreCellMLEditing {
    class CoreCellmlEditingWidget;
}   // namespace CoreCellMLEditing

//==============================================================================

namespace Editor {
    class EditorWidget;
}   // namespace Editor

//==============================================================================

namespace RawCellMLView {

//==============================================================================

class RawCellmlViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit RawCellmlViewWidget(QWidget *pParent);
    ~RawCellmlViewWidget();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void retranslateUi();

    bool contains(const QString &pFileName) const;

    void initialize(const QString &pFileName);
    void finalize(const QString &pFileName);

    void fileReloaded(const QString &pFileName);
    void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);

    Editor::EditorWidget * editor(const QString &pFileName) const;

    virtual QList<QWidget *> statusBarWidgets() const;

private:
    Ui::RawCellmlViewWidget *mGui;

    CoreCellMLEditing::CoreCellmlEditingWidget *mEditingWidget;
    QMap<QString, CoreCellMLEditing::CoreCellmlEditingWidget *> mEditingWidgets;

    QIntList mEditingWidgetSizes;
    int mEditorZoomLevel;

    bool mViewerOptimiseFontSizeEnabled;
    bool mViewerSubscriptsEnabled;
    bool mViewerGreekSymbolsEnabled;
    bool mViewerDigitGroupingEnabled;

    QMap<QString, QString> mPresentationMathmlEquations;

    Core::XslTransformer *mXslTransformer;

    void cleanUpXml(const QDomNode &pDomNode) const;
    QString cleanUpXml(const QString &pMathml) const;

    QString retrieveContentMathmlEquation(const QString &pContentMathmlBlock,
                                          const int &pPosition) const;

private Q_SLOTS:
    void splitterMoved();
    void zoomLevelChanged(const int &pZoomLevel);

    void updateViewer();

    void optimiseFontSizeChanged(const bool &pEnabled);
    void subscriptsChanged(const bool &pEnabled);
    void greekSymbolsChanged(const bool &pEnabled);
    void digitGroupingChanged(const bool &pEnabled);

    void xslTransformationDone(const QString &pInput, const QString &pOutput);
};

//==============================================================================

}   // namespace RawCellMLView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
