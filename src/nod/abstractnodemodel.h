
// ----------------------------------------------------------------------------

#ifndef NOD_ABSTRACTNODEMODEL_H
#define NOD_ABSTRACTNODEMODEL_H

// ----------------------------------------------------------------------------

#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/nodemodel.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

class AbstractNodeModel : public NodeModel
{
    Q_OBJECT
public:

    enum
    {
        SerializationVersion    = 100
    };

    using NodeModel::NodeModel;

    /* NodeModel */

    NodeID                      connectedNode(const NodeID &node, const PortID &port, PortID *other_port=nullptr) const override;

    PortID                      connectedPort(const NodeID &node, const PortID &port) const override;

    Connection                  connection(const NodeID &node, const PortID &port) const override;

    bool                        canConnect(const NodeID &node1, const PortID &port1,
                                           const NodeID &node2, const PortID &port2) const override;

    bool                        connect(const NodeID &node1, const PortID &port1,
                                        const NodeID &node2, const PortID &port2) override;

    bool                        disconnect(const NodeID &node) override;

    bool                        disconnect(const NodeID &node, const PortID &port) override;

    bool                        isConnected(const NodeID &node) const override;

    bool                        isConnected(const NodeID &node, const PortID &port) const override;

    bool                        isConnected(const Connection &connection) const override;

    bool                        serialize(Serialized &data) override;

private:

    QVector<Connection>         mConnections;
};

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

#endif // NOD_ABSTRACTNODEMODEL_H

// ----------------------------------------------------------------------------
