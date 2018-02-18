
// ----------------------------------------------------------------------------

#include "nod/connectionitem.h"
#include "nod/defaultconnectionshape.h"
#include "nod/defaultnodeitemfactory.h"
#include "nod/defaultnodeitem.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeItem *DefaultNodeItemFactory::createNodeItem(const NodeID &node)
{
    return new DefaultNodeItem(scene(), node);
}

// ----------------------------------------------------------------------------

ConnectionShape *DefaultNodeItemFactory::createConnectionShape()
{
    return new DefaultConnectionShape(scene().grid());
}

// ----------------------------------------------------------------------------

ConnectionItem *DefaultNodeItemFactory::createConnectionItem(const NodeID &node, const PortID &port)
{
    auto connection = scene().model()->connection(node, port);
    if (!connection.isValid())
        return nullptr;

    auto shape = createConnectionShape();
    auto item = new ConnectionItem(scene(), connection, shape);
    item->updatePath();
    item->updateGrid();
    return item;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
