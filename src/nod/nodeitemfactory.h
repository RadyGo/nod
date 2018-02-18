
// ----------------------------------------------------------------------------

#ifndef NOD_NODEITEMFACTORY_H
#define NOD_NODEITEMFACTORY_H

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class NodeItemFactory
{
public:

    virtual ~NodeItemFactory() {}

    void                        setScene(NodeScene &scene) { mScene = &scene; }

    NodeScene                   &scene() const { return *mScene; }

    /** Creates a new node item from scratch.
     *
     * @param node The node to create the item for.
     *
     * @return The node ID or invalid if failed.
     *
     */
    virtual NodeItem            *createNodeItem(const NodeID &node)=0;

    virtual ConnectionShape     *createConnectionShape()=0;

    virtual ConnectionItem      *createConnectionItem(const Connection &connection)=0;

private:

    NodeScene                   *mScene = nullptr;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEITEMFACTORY_H

// ----------------------------------------------------------------------------
