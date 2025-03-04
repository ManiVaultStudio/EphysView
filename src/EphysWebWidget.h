#pragma once

#include "EphysData/Experiment.h"

#include <widgets/WebWidget.h>

#include "Scene.h"

#include <vector>

class EphysView;

class JSCommunicationObject : public mv::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    JSCommunicationObject();

signals:
    // Signals from Qt to JS side
signals:
    void setData(QString data);
    void setFilterInJS(const QVariantList& data);
    void setHeaderOptions(const QVariantList& data);

public slots:
    // Invoked from JS side 
    void js_partitionHovered(const QString& data);

signals:
    // Signals from comm object to web widget
    void partitionHovered(QString name);

private:

};

class EphysWebWidget : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    EphysWebWidget(EphysView* plugin, Scene* scene);
    ~EphysWebWidget();

    JSCommunicationObject& getCommObject() { return _commObject; }

    void setData(const std::vector<Recording>& recordings);

private slots:
    void onWebPageFullyLoaded();
    void onPartitionHovered(QString name);

protected:
    void resizeEvent(QResizeEvent* event);

public slots:
    void applyAspectRatio();

private:
    JSCommunicationObject   _commObject;    // Communication Object between Qt (cpp) and JavaScript

    Scene* _scene;
};
