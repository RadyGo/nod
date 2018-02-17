
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

int NodeModel::portCount(const NodeID &node, Direction direction) const
{
    int count = 0;
    for (int i=0, c=portCount(node); i<c; ++i)
    {
        auto prt = port(node, i);
        if (portDirection(node, prt) == direction)
            count++;
    }

    return count;
}

// ----------------------------------------------------------------------------

PortID NodeModel::port(const NodeID &node, Direction direction, int index) const
{
    int at = 0;
    for (int i=0, c=portCount(node); i<c; ++i)
    {
        auto prt = port(node, i);
        if (portDirection(node, prt) == direction)
        {
            if (at == index)
                return prt;
            ++at;
        }
    }

    return PortID::invalid();
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
