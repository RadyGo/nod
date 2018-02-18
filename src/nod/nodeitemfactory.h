
// ----------------------------------------------------------------------------

#ifndef NOD_NODEITEMFACTORY_H
#define NOD_NODEITEMFACTORY_H

// ----------------------------------------------------------------------------

#include "nod/common.h"
#include "nod/nodefactory.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

/** The node item factory creates various QGraphicsItem based objects used in a NodeScene.
 *
 */
class NodeItemFactory
{
    Q_DISABLE_COPY(NodeItemFactory)
public:

    NodeItemFactory(NodeFactory &node_factory);
    virtual ~NodeItemFactory();

    void                        setScene(NodeScene &scene) { mScene = &scene; }

    NodeScene                   &scene() const { return *mScene; }

    NodeFactory                 &nodeFactory() { return mNodeFactory; }

    /** Creates a new node item from scratch.
     *
     * @param node The node to create the item for.
     *
     * @return The node ID or invalid if failed.
     *
     */
    virtual NodeItem            *createNodeItem(const NodeID &node)=0;

    virtual ConnectionShape     *createConnectionShape()=0;

    virtual ConnectionItem      *createConnectionItem(const NodeID &node, const PortID &port)=0;

private:

    NodeFactory                 &mNodeFactory;
    NodeScene                   *mScene = nullptr;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEITEMFACTORY_H

// ----------------------------------------------------------------------------
