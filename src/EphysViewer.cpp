#include "EphysViewer.h"

#include <event/Event.h>

#include <DatasetsMimeData.h>

#include <util/Timer.h>

#include <QDebug>
#include <QMimeData>
#include <fstream>
#include <sstream>

Q_PLUGIN_METADATA(IID "studio.manivault.EphysViewer")

using namespace mv;

namespace
{
    bool isEphysFeatures(mv::Dataset<DatasetImpl> dataset)
    {
        return dataset->hasProperty("PatchSeqType") && dataset->getProperty("PatchSeqType").toString() == "E";
    }

    bool isEphysTraces(mv::Dataset<DatasetImpl> dataset)
    {
        return dataset->hasProperty("PatchSeqType") && dataset->getProperty("PatchSeqType").toString() == "EphysTraces";
    }

    bool isMetadata(mv::Dataset<DatasetImpl> dataset)
    {
        return dataset->hasProperty("PatchSeqType") && dataset->getProperty("PatchSeqType").toString() == "Metadata";
    }
}

EphysView::EphysView(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dropWidget(nullptr),
    _scene(),
    _ephysWidget(new EphysWebWidget(this, &_scene)),
    _primaryToolbarAction(this, "PrimaryToolbar"),
    _settingsAction(this, "SettingsAction")
{

}

void EphysView::init()
{
    // Create layout
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    //_primaryToolbarAction.addAction(&_settingsAction.getLineRendererButton());
    //_primaryToolbarAction.addAction(&_settingsAction.getRealRendererButton());

        // Load webpage
    _ephysWidget->setPage(":ephys_viewer/ephys_viewer/trace_view.html", "qrc:/ephys_viewer/ephys_viewer/");

    layout->addWidget(_primaryToolbarAction.createWidget(&getWidget()), 1);
    layout->addWidget(_ephysWidget, 99);

    // Apply the layout
    getWidget().setLayout(layout);

    // Respond when the name of the dataset in the dataset reference changes
    connect(&_scene._cellMetadata, &Dataset<Text>::guiNameChanged, this, [this]()
    {
        // Only show the drop indicator when nothing is loaded in the dataset reference
        _dropWidget->setShowDropIndicator(_scene._cellMetadata->getGuiName().isEmpty());
    });

    connect(&_scene._ephysTraces, &Dataset<Text>::changed, this, [this]() { connect(&_scene._ephysTraces, &Dataset<Text>::dataSelectionChanged, this, &EphysView::onCellSelectionChanged); });

    // Alternatively, classes which derive from mv::EventListener (all plugins do) can also respond to events
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetRemoved));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(PointType, std::bind(&EphysView::onDataEvent, this, std::placeholders::_1));
    _eventListener.registerDataEventByType(TextType, std::bind(&EphysView::onDataEvent, this, std::placeholders::_1));
    _eventListener.registerDataEventByType(EphysType, std::bind(&EphysView::onDataEvent, this, std::placeholders::_1));
    
    // Check if any usable datasets are already available, if so, use them
    for (mv::Dataset dataset : mv::data().getAllDatasets())
    {
        if (isEphysFeatures(dataset))
            _scene._ephysFeatures = dataset;
        if (isEphysTraces(dataset))
            _scene._ephysTraces = dataset;
        if (isMetadata(dataset))
            _scene._cellMetadata = dataset;
    }
    qDebug() << _scene._ephysFeatures.isValid();
    qDebug() << _scene._ephysTraces.isValid();
    qDebug() << _scene._cellMetadata.isValid();
}

void EphysView::onDataEvent(mv::DatasetEvent* dataEvent)
{
    // Get smart pointer to dataset that changed
    const auto changedDataSet = dataEvent->getDataset();

    // Get GUI name of the dataset that changed
    const auto datasetGuiName = changedDataSet->getGuiName();

    // The data event has a type so that we know what type of data event occurred (e.g. data added, changed, removed, renamed, selection changes)
    switch (dataEvent->getType()) {

        // A points dataset was added
        case EventType::DatasetAdded:
        {
            // Cast the data event to a data added event
            const auto dataAddedEvent = static_cast<DatasetAddedEvent*>(dataEvent);

            if (isEphysFeatures(changedDataSet))
                _scene._ephysFeatures = changedDataSet;
            if (isEphysTraces(changedDataSet))
                _scene._ephysTraces = changedDataSet;
            if (isMetadata(changedDataSet))
                _scene._cellMetadata = changedDataSet;

            qDebug() << "Data added";
            qDebug() << _scene._ephysFeatures.isValid();
            qDebug() << _scene._ephysTraces.isValid();
            qDebug() << _scene._cellMetadata.isValid();

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was added";

            break;
        }
        case EventType::DatasetDataChanged:
        {
            if (isEphysTraces(changedDataSet))
            {
                qDebug() << "Data changed";
                //std::vector<Experiment> experiments = _scene._ephysTraces->getData();
                //const std::vector<Recording>& acquisitions = _scene._ephysTraces->getData()[0].getAcquisitions();

                //_ephysWidget->setData(recordings);
            }
        }
        default:
            break;
    }
}

void EphysView::onCellSelectionChanged()
{
    if (!_scene._ephysTraces.isValid())
        return;

    if (!_scene._cellMetadata.isValid())
    {
        qWarning() << "No cell metadata dataset set.";
        return;
    }

    if (!_scene._ephysFeatures.isValid())
    {
        qWarning() << "No ephys features dataset found by EphysView.";
        return;
    }

    const auto& selectionIndices = _scene._ephysTraces->getSelectionIndices();

    Timer t("OnCellSelectedChanged");
    //
    //std::vector<Recording> cellAcquisitions;
    //std::vector<Recording> cellStimuli;
    //for (uint32_t index: selectionIndices)
    //{
    //    const std::vector<Recording>& acquisitions = _scene._ephysTraces->getData()[index].getAcquisitions();
    //    const std::vector<Recording>& stimuli = _scene._ephysTraces->getData()[index].getStimuli();

    //    if (!acquisitions.empty() && !stimuli.empty())
    //    {
    //        for (int i = 0; i < acquisitions.size(); i++)
    //        {
    //            const Recording& acquisition = acquisitions[i];
    //            const Recording& stimulus = stimuli[i];

    //            //if (acquisition.GetStimulusDescription().contains("SupraThresh"))
    //            {
    //                cellAcquisitions.push_back(acquisition);
    //                cellStimuli.push_back(stimulus);
    //                break;
    //            }
    //            
    //        }
    //    }
    //}

    _ephysWidget->setData(_scene._ephysTraces->getData(), selectionIndices);
    //_ephysWidget->setData(cellAcquisitions, cellStimuli);

    qDebug() << "Selection indices ephys: " << selectionIndices.size();
    qDebug() << "Selection indices ephys2: " << _scene._ephysTraces->getSelection()->getSelectionIndices().size();
}

ViewPlugin* EphysViewFactory::produce()
{
    return new EphysView(this);
}

mv::DataTypes EphysViewFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This example analysis plugin is compatible with points datasets
    supportedTypes.append(TextType);

    return supportedTypes;
}

mv::gui::PluginTriggerActions EphysViewFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this]() -> EphysView* {
        return dynamic_cast<EphysView*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets >= 1 && PluginFactory::areAllDatasetsOfTheSameType(datasets, TextType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<EphysViewFactory*>(this), this, "Ephys Viewer", "View electrophysiology traces", icon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance();
        });

        pluginTriggerActions << pluginTriggerAction;
    }

    return pluginTriggerActions;
}
