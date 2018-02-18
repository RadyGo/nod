
// ----------------------------------------------------------------------------

#ifndef NOD_NODEMODEL_H
#define NOD_NODEMODEL_H

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------
// NodeIt
// ----------------------------------------------------------------------------

class NodeIt
{
public:

    NodeIt(NodeModel &model, const NodeID &node, uint64_t data);

    const NodeModel             &model() const { return *mModel; }

    uint64_t                    data() const { return mData; }

    bool                        atEnd() const;

    void                        next();

    const NodeID                &node() const { return mNode; }

    void                        update(const NodeID &node, uint64_t data);

private:

    NodeModel                   *mModel;
    NodeID                      mNode;
    uint64_t                    mData;
};

// ----------------------------------------------------------------------------

inline bool operator==(const NodeIt &a, const NodeIt &b)
{
    return &a.model() == &b.model() && a.data() == b.data();
}

// ----------------------------------------------------------------------------

inline bool operator!=(const NodeIt &a, const NodeIt &b)
{
    return !operator==(a, b);
}

// ----------------------------------------------------------------------------
// PortIt
// ----------------------------------------------------------------------------

class PortIt
{
public:

    PortIt(NodeModel &model, const NodeID &node, uint64_t node_data, const PortID &port, uint64_t port_data);

    const NodeModel             &model() const { return *mModel; }

    uint64_t                    nodeData() const { return mNodeData; }

    uint64_t                    portData() const { return mPortData; }

    void                        update(const PortID &port, uint64_t port_data);

    bool                        atEnd() const;

    void                        next();

    const NodeID                &node() const { return mNode; }

    const PortID                &port() const { return mPort; }

private:

    NodeModel                   *mModel;
    NodeID                      mNode;
    uint64_t                    mNodeData;
    PortID                      mPort;
    uint64_t                    mPortData;
};

// ----------------------------------------------------------------------------

inline bool operator==(const PortIt &a, const PortIt &b)
{
    return &a.model() == &b.model() && a.nodeData() == b.nodeData() && a.portData() == b.portData();
}

// ----------------------------------------------------------------------------

inline bool operator!=(const PortIt &a, const PortIt &b)
{
    return !operator==(a, b);
}

// ----------------------------------------------------------------------------

class NodeModel : public QObject
{
    Q_OBJECT
public:

    NodeModel(QObject *parent=nullptr);

    /* Roles */

    virtual const char          *roleName(DataRole role) const;

    virtual QString             roleDisplayName(DataRole role) const;

    /* Nodes */

    virtual bool                createNode(const NodeID &node)=0;

    virtual QVariant            nodeData(const NodeID &node, DataRole role) const=0;

    virtual void                setNodeData(const NodeID &node, const QVariant &value, DataRole role)=0;

    virtual NodeIt              firstNode() const=0;

    virtual NodeIt              endNode() const=0;

    virtual void                nextNode(NodeIt &it) const=0;

    /* Ports */

    virtual PortIt              firstPort(const NodeID &node) const=0;

    virtual PortIt              endPort(const NodeID &node) const=0;

    virtual void                nextPort(PortIt &it) const=0;

    virtual QVariant            portData(const NodeID &node, const PortID &port, DataRole role)=0;

    virtual Direction           portDirection(const NodeID &node, const PortID &port) const=0;

    /* Connections */

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

    /* Flags */

    virtual NodeFlags           flags(const NodeID &node) const=0;

    /* Serialization */

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

inline NodeIt::NodeIt(NodeModel &model, const NodeID &node, uint64_t data)
    : mModel(&model),
      mNode(node),
      mData(data)
{
}

// ----------------------------------------------------------------------------

inline bool NodeIt::atEnd() const
{
    return *this == mModel->endNode();
}

// ----------------------------------------------------------------------------

inline void NodeIt::next()
{
    mModel->nextNode(*this);
}

// ----------------------------------------------------------------------------

inline void NodeIt::update(const NodeID &node, uint64_t data)
{
    mNode = node;
    mData = data;
}

// ----------------------------------------------------------------------------

inline PortIt::PortIt(NodeModel &model, const NodeID &node, uint64_t node_data, const PortID &port, uint64_t port_data)
    : mModel(&model),
      mNode(node),
      mNodeData(node_data),
      mPort(port),
      mPortData(port_data)
{
}

// ----------------------------------------------------------------------------

inline void PortIt::update(const PortID &port, uint64_t port_data)
{
    mPort = port;
    mPortData = port_data;
}

// ----------------------------------------------------------------------------

inline bool PortIt::atEnd() const
{
    return *this == mModel->endPort(mNode);
}

// ----------------------------------------------------------------------------

inline void PortIt::next()
{
    mModel->nextPort(*this);
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

#endif // NOD_NODEMODEL_H

// ----------------------------------------------------------------------------
