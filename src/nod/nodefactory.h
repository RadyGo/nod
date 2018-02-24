
// ----------------------------------------------------------------------------

#ifndef NOD_NODEFACTORY_H
#define NOD_NODEFACTORY_H

// ----------------------------------------------------------------------------

#include <QIcon>
#include <QString>
#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

/** Describes a node type.
 */
struct NodeType
{
    NodeTypeID                  id;
    QIcon                       icon;
    QString                     name;
    QString                     description;
};

// ----------------------------------------------------------------------------

inline bool operator==(const NodeType &a, const NodeType &b)
{
    return a.id == b.id;
}

// ----------------------------------------------------------------------------

/** The node factory creates new nodes.
 *
 * It is used during deserialization and when creating nodes via the user
 * interface. In addition to creating nodes, it allows iterating over all
 * known node types.
 *
 */
class NodeFactory
{
public:

    virtual ~NodeFactory();

    /** Returns node type information.
     *
     * @param id The type ID.
     *
     * @return The node type information or nullptr if not registered.
     *
     */
    const NodeType              *type(const NodeTypeID &id) const;

    /** Registers a node type.
     *
     * @param type The type to register.
     *
     */
    void                        registerType(const NodeType &type);

    /** Returns the number of registered types.
     *
     * @return The number of registered types.
     *
     */
    int                         typeCount() const { return mTypes.size(); }

    /** Returns a type by index.
     *
     * You can get the number of registered types using typeCount().
     *
     * @param index The index of the type.
     *
     * @return The type information.
     *
     */
    const NodeType              &type(int index) const { return mTypes[index]; }

    /** Creates a new node from scratch.
     *
     * This is used when initially creating nodes, restoring, copy & paste and undo / redo.
     *
     * A new node ID is generated if the passed node ID is invalid. Pass a valid node ID when
     * deserializing or an invalid ID when creating new nodes.
     *
     * @param model The model which stores the node data.
     * @param type The type of the node.
     * @param id The node ID.
     *
     * @return The node ID or invalid if failed.
     *
     */
    virtual NodeID              createNode(NodeModel &model, const NodeTypeID &type, const NodeID &id=NodeID::invalid())=0;

private:

    QVector<NodeType>           mTypes;
};

// ----------------------------------------------------------------------------

} // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEFACTORY_H

// ----------------------------------------------------------------------------
