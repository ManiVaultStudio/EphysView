#include "SettingsAction.h"

#include "EphysViewer.h"

SettingsAction::SettingsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _plugin(dynamic_cast<EphysView*>(parent))
    //_lineRendererButton(this, "Line Renderer"),
    //_realRendererButton(this, "True Renderer")
{

}
