
// ----------------------------------------------------------------------------

#ifndef NOD_NODEMODEL_H
#define NOD_NODEMODEL_H

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

class NodeModel : public QObject
{
    Q_OBJECT
public:

    NodeModel(QObject *parent=nullptr);

    virtual const char          *roleName(DataRole role) const;

    virtual QString             roleHumanName(DataRole role) const;

    virtual QVariant            nodeData(const NodeID &node, DataRole role) const=0;

    virtual void                setNodeData(const NodeID &node, const QVariant &value, DataRole role)=0;

    virtual int                 nodeCount() const=0;

    virtual NodeID              node(int index) const=0;

    virtual int                 portCount(const NodeID &node) const=0;

    virtual int                 portCount(const NodeID &node, Direction direction) const;

    virtual PortID              port(const NodeID &node, int index) const=0;

    virtual PortID              port(const NodeID &node, Direction direction, int index) const;

    virtual QVariant            portData(const NodeID &node, const PortID &port, DataRole role)=0;

    virtual Direction           portDirection(const NodeID &node, const PortID &port) const=0;

    virtual NodeID              connectedNode(const NodeID &node, const PortID &port, PortID *other_port=nullptr) const=0;

    virtual PortID              connectedPort(const NodeID &node, const PortID &port) const=0;

    virtual Connection          connection(const NodeID &node, const PortID &port) const=0;

    virtual bool                canConnect(const NodeID &node1, const PortID &port1,
                                           const NodeID &node2, const PortID &port2) const=0;

    virtual bool                connect(const NodeID &node1, const PortID &port1,
                                        const NodeID &node2, const PortID &port2)=0;

    virtual bool                disconnect(const NodeID &node)=0;

    virtual bool                disconnect(const NodeID &node, const PortID &port)=0;

    virtual bool                isConnected(const NodeID &node) const=0;

    virtual bool                isConnected(const NodeID &node, const PortID &port) const=0;

    virtual bool                isConnected(const Connection &connection) const=0;

    virtual NodeFlags           flags(const NodeID &node) const=0;

    virtual bool                serialize(Serializer &serializer, Serialized &data)=0;

signals:

    void                        nodeCreated(NodeModel &model, const NodeID &node);

    void                        nodeDeleted(NodeModel &model, const NodeID &node);

    void                        nodePositionChanged(NodeModel &model, const NodeID &node);

    void                        nodeSizeChanged(NodeModel &model, const NodeID &node);

    void                        nodeConnected(NodeModel &model, const NodeID &node, const PortID &port);

    void                        nodeDisconnected(NodeModel &model, const NodeID &node);

    void                        portDisconnected(NodeModel &model, const NodeID &node, const PortID &port);

private:
};

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

#endif // NOD_NODEMODEL_H

// ----------------------------------------------------------------------------
