
// ----------------------------------------------------------------------------

#ifndef NOD_DEFAULTNODEITEMFACTORY_H
#define NOD_DEFAULTNODEITEMFACTORY_H

// ----------------------------------------------------------------------------

#include "nod/nodeitemfactory.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

/** A NodeItemFactory creating default item implementations.
 *
 */
class DefaultNodeItemFactory : public NodeItemFactory
{
public:

    using NodeItemFactory::NodeItemFactory;

    /* NodeItemFactory */

    NodeItem                    *createNodeItem(const NodeID &node) override;

    ConnectionShape             *createConnectionShape() override;

    ConnectionItem              *createConnectionItem(const NodeID &node, const PortID &port) override;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_DEFAULTNODEITEMFACTORY_H

// ----------------------------------------------------------------------------
