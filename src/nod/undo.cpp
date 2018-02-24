
// ----------------------------------------------------------------------------

#include <QDebug>

// ----------------------------------------------------------------------------

#include "nod/nodemodel.h"
#include "nod/nodeitem.h"
#include "nod/undo.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

DeleteSelectionCommand::DeleteSelectionCommand(NodeScene &scene)
    : SerializedSelectionCommand(scene)
{

}

// ----------------------------------------------------------------------------

void DeleteSelectionCommand::redo()
{

}

// ----------------------------------------------------------------------------

void DeleteSelectionCommand::undo()
{

}

// ----------------------------------------------------------------------------

SerializedSelectionCommand::SerializedSelectionCommand(NodeScene &scene)
    : mData(false)
{
    QVector<NodeID> nodes;

    auto items = scene.selectedItems();
    for (auto item : items)
    {
        auto node_item = qgraphicsitem_cast<NodeItem*>(item);
        if (node_item)
            nodes.append(node_item->node());
    }

    if (!scene.model()->writeNodes(mData, nodes))
        qWarning() << "Model serialization failed";

    qDebug() << "SerializedSelectionCommand: serialized selection" << qPrintable(mData.doc().toJson(QJsonDocument::Indented));

    mData.setReading();
}

// ----------------------------------------------------------------------------

CutSelectionCommand::CutSelectionCommand(NodeScene &scene)
    : SerializedSelectionCommand(scene)
{

}

// ----------------------------------------------------------------------------

void CutSelectionCommand::redo()
{

}

// ----------------------------------------------------------------------------

void CutSelectionCommand::undo()
{

}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
