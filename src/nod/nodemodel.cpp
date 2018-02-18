
// ----------------------------------------------------------------------------

#include "nod/nodemodel.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

NodeModel::NodeModel(QObject *parent)
    : QObject(parent)
{
}

// ----------------------------------------------------------------------------

const char *NodeModel::roleName(DataRole role) const
{
    switch (role)
    {
    case DataRole::Display: return "display";
    case DataRole::Name: return "name";
    case DataRole::Icon: return "icon";
    case DataRole::Tooltip: return "tooltip";
    case DataRole::Color: return "color";
    case DataRole::Maximum: return "maximum";
    case DataRole::Minimum: return "minimum";
    case DataRole::SingleStep: return "single_step";
    case DataRole::PageStep: return "page_step";
    case DataRole::Index: return "index";
    case DataRole::Position: return "position";
    case DataRole::Size: return "size";
    case DataRole::Type: return "type";
    case DataRole::Value: return "value";
    case DataRole::User:
        break;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------

QString NodeModel::roleDisplayName(DataRole role) const
{
    switch (role)
    {
    case DataRole::Display: return QObject::tr("Display Value");
    case DataRole::Name: return QObject::tr("Name");
    case DataRole::Icon: return QObject::tr("Icon");
    case DataRole::Tooltip: return QObject::tr("Tool Tip");
    case DataRole::Color: return QObject::tr("Color");
    case DataRole::Maximum: return QObject::tr("Maximum");
    case DataRole::Minimum: return QObject::tr("Minimum");
    case DataRole::SingleStep: return QObject::tr("Single Step");
    case DataRole::PageStep: return QObject::tr("Page Step");
    case DataRole::Index: return QObject::tr("Index");
    case DataRole::Position: return QObject::tr("Position");
    case DataRole::Size: return QObject::tr("Size");
    case DataRole::Type: return QObject::tr("Type");
    case DataRole::Value: return QObject::tr("Value");
    case DataRole::User:
        break;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
