#include "EphysWebWidget.h"

#include "EphysViewer.h"

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

void EphysWebWidget::setData(const std::vector<Recording>& recordings)
{
    QJsonObject finalObject;

    for (int i = 0; i < recordings.size(); i++)
    {
        const Recording& recording = recordings[i];

        QJsonArray xArray, yArray;

        // Convert std::vector<float> to QJsonArray
        for (float x : recording.data.xSeries)
            xArray.append(x);

        for (float y : recording.data.ySeries)
            yArray.append(y);

        // Create a JSON object
        QJsonObject jsonObject;
        jsonObject["xData"] = xArray;
        jsonObject["yData"] = yArray;

        finalObject.insert("data_" + QString::number(i), jsonObject);
    }

    QJsonDocument doc(finalObject);
    QString strJson(doc.toJson(QJsonDocument::Indented));
    //std::cout << strJson.toStdString() << std::endl;
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
