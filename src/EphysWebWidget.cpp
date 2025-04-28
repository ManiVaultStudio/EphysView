#include "EphysWebWidget.h"

#include "EphysViewer.h"

#include <util/Timer.h>

#include <QLayout>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QHash>

#include <iostream>

// =============================================================================
// JSCommunicationObject
// =============================================================================

JSCommunicationObject::JSCommunicationObject()
{

}

// =============================================================================
// EphysWebWidget
// =============================================================================

EphysWebWidget::EphysWebWidget(EphysView* plugin, Scene* scene) :
    _commObject(),
    _scene(scene)
{
    connect(this, &WebWidget::webPageFullyLoaded, this, &EphysWebWidget::onWebPageFullyLoaded);
    qDebug() << "Connect to event";
    // For more info on drag&drop behavior, see the ExampleViewPlugin project
    setAcceptDrops(true);

    // Ensure linking to the resources defined in res/ephys_viewer_resources.qrc
    Q_INIT_RESOURCE(ephys_viewer_resources);

    // ManiVault and Qt create a "QtBridge" object on the js side which represents _comObject
    // there, we can connect the signals qt_js_* and call the slots js_qt_* from our communication object
    init(&_commObject);

    setContentsMargins(0, 0, 0, 0);
    layout()->setContentsMargins(0, 0, 0, 0);
}

EphysWebWidget::~EphysWebWidget()
{

}

void EphysWebWidget::setData(const std::vector<Recording>& acquisitions, const std::vector<Recording>& stimuli)
{
    Timer t("SetData");

    if (acquisitions.size() != stimuli.size())
    {
        qWarning() << "[EphysViewer] Num acquisitions not same as num stimuli, skipping drawing..";
        return;
    }

    QJsonArray graphArray;
    
    int numRecordings = acquisitions.size(); // Should be the same as stimuli.size()
    for (int i = 0; i < numRecordings; i++)
    {
        QJsonArray acqXData, acqYData, stimXData, stimYData;
        QJsonObject acquisitionObj, stimulusObj;

        const Recording& acquisition = acquisitions[i];
        const Recording& stimulus = stimuli[i];

        for (float x : acquisition.GetData().xSeries)
            acqXData.append(x);
        for (float y : acquisition.GetData().ySeries)
            acqYData.append(y);
        for (float x : stimulus.GetData().xSeries)
            stimXData.append(x);
        for (float y : stimulus.GetData().ySeries)
            stimYData.append(y);

        acquisitionObj["xData"] = acqXData;
        acquisitionObj["yData"] = acqYData;
        stimulusObj["xData"] = stimXData;
        stimulusObj["yData"] = stimYData;

        QJsonObject graphObj;
        graphObj.insert("acquisition", acquisitionObj);
        graphObj.insert("stimulus", stimulusObj);
        graphObj.insert("title", acquisition.GetStimulusDescription());

        graphArray.append(graphObj);
    }

    QJsonObject rootObj;
    rootObj.insert("graphs", graphArray);

    QJsonDocument doc(rootObj);
    QString strJson(doc.toJson(QJsonDocument::Indented));
    std::cout << strJson.toStdString() << std::endl;

    t.printElapsedTime("SetData", true);
    _commObject.setData(strJson);
    qDebug() << ">>>>>>>>>>>>>>>>>>>>>>>>>>>> Set Data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
}

void JSCommunicationObject::js_partitionHovered(const QString& data) {
    if (!data.isEmpty())
    {
        qDebug() << "PARTITION SIGNAL" << data;
        emit partitionHovered(data);
    }
}

void EphysWebWidget::onWebPageFullyLoaded()
{
    qDebug() << "EphysWebWidget::onWebPageFullyLoaded: Web page completely loaded.";
    //emit webPageLoaded();

    qDebug() << "EphysWebWidget size: " << width() << height();
}

void EphysWebWidget::onPartitionHovered(QString name)
{
    qDebug() << "You hovered over partition: " << name;
}

void EphysWebWidget::resizeEvent(QResizeEvent* event)
{
    (void)event;
    //applyAspectRatio();
}

void EphysWebWidget::applyAspectRatio()
{
    int w = this->width();
    int h = this->height();
    double aspect = static_cast<double>(h) / static_cast<double>(w);

    if (aspect < 1.0f)
    {
        int targetSize = std::max(w, h);
        setMinimumWidth(targetSize);
        setMinimumHeight(targetSize);
        setMaximumWidth(targetSize);
        setMaximumHeight(targetSize);
    }
}
