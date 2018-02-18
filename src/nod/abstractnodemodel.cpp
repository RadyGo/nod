
// ----------------------------------------------------------------------------

#include "nod/abstractnodemodel.h"
#include "nod/serializer.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

NodeID AbstractNodeModel::connectedNode(const NodeID &node, const PortID &port, PortID *other_port) const
{
    for (auto &c : mConnections)
    {
        if (node.value == c.node1.value && port.value == c.port1.value)
        {
            if (other_port)
                *other_port = c.port2;

            return c.node2;
        }

        if (node.value == c.node2.value && port.value == c.port2.value)
        {
            if (other_port)
                *other_port = c.port1;

            return c.node1;
        }
    }

    return NodeID::invalid();
}

// ----------------------------------------------------------------------------

PortID AbstractNodeModel::connectedPort(const NodeID &node, const PortID &port) const
{
    PortID other;
    if (connectedNode(node, port, &other).isValid())
        return other;

    return PortID::invalid();
}

// ----------------------------------------------------------------------------

Connection AbstractNodeModel::connection(const NodeID &node, const PortID &port) const
{
    for (auto &c : mConnections)
    {
        if (c.contains(node, port))
            return c;
    }

    return Connection::invalid();
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::canConnect(const NodeID &node1, const PortID &port1,
                                   const NodeID &node2, const PortID &port2) const
{
    Connection conn = { node1, port1, node2, port2 };
    for (auto &c : mConnections)
    {
        if (c.isEqual(conn))
            return false; // already connected
    }

    return true;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::connect(const NodeID &node1, const PortID &port1,
                                const NodeID &node2, const PortID &port2)
{
    if (!canConnect(node1, port1, node2, port2))
        return false;

    mConnections.push_back({ node1, port1, node2, port2 });

    emit nodeConnected(*this, node1, port1);
    emit nodeConnected(*this, node2, port2);

    return true;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::disconnect(const NodeID &node)
{
    for (auto it=mConnections.begin(); it!=mConnections.end(); )
    {
        if (it->contains(node))
        {
            emit portDisconnected(*this, it->node1, it->port1);
            emit portDisconnected(*this, it->node2, it->port2);
            it = mConnections.erase(it);
        } else
            ++it;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::disconnect(const NodeID &node, const PortID &port)
{
    for (auto it=mConnections.begin(); it!=mConnections.end(); ++it)
    {
        if (it->contains(node, port))
        {
            emit portDisconnected(*this, it->node1, it->port1);
            emit portDisconnected(*this, it->node2, it->port2);
            mConnections.erase(it);
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::isConnected(const NodeID &node) const
{
    for (auto &c : mConnections)
    {
        if (node.value == c.node1.value || node.value == c.node2.value)
            return true;
    }
    return true;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::isConnected(const NodeID &node, const PortID &port) const
{
    for (auto &c : mConnections)
    {
        if ((node.value == c.node1.value && port.value == c.port1.value) ||
            (node.value == c.node2.value && port.value == c.port2.value))
            return true;
    }

    return true;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::isConnected(const Connection &connection) const
{
    for (auto &c : mConnections)
    {
        if (c.isEqual(connection))
            return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

static QJsonArray packPorts(Serializer &serializer,
                            AbstractNodeModel &model, const NodeID &node_id,
                            QVector<Connection> &connections, Direction direction)
{
    QJsonArray items;
    for (auto pit=model.firstPort(node_id), end=model.endPort(node_id); pit!=end; pit.next())
    {
        if (model.portDirection(node_id, pit.port()) != direction)
            continue;

        QJsonObject prt;
        auto port_id = pit.port();
        prt["id"] = QString::fromLocal8Bit(port_id.value.toByteArray());

        PortID other_port;
        auto other_node = model.connectedNode(node_id, port_id, &other_port);
        if (other_node.isValid())
        {
            Connection conn;
            conn.node1 = node_id;
            conn.port1 = port_id;
            conn.node2 = other_node;
            conn.port2 = other_port;

            bool connected = false;
            for (auto &c : connections)
            {
                if (c.isEqual(conn))
                {
                    connected = true;
                    break;
                }
            }

            if (!connected)
                connections.append(conn);
        }

        QJsonObject port_data;
        for (int i=0; i<serializer.maxSerializedRole(); ++i)
        {
            auto role = DataRole(i);
            auto role_data = model.portData(node_id, port_id, role);
            if (role_data.isNull())
                continue;

            auto name = model.roleName(role);
            QString key = name ? QString::fromUtf8(name) : QString("%1").arg(i);
            port_data[key] = serializer.toJson(role_data);
        }

        prt["data"] = port_data;

        items.append(prt);
    }
    return items;
}

// ----------------------------------------------------------------------------

bool AbstractNodeModel::serialize(Serializer &serializer, Serialized &data)
{
    if (serializer.isReading())
    {

    } else
    {
        QVector<Connection> connections;

        QJsonObject root;
        root["version"] = SerializationVersion;

        QJsonArray nodes;
        for (auto nit=firstNode(), end=endNode(); nit!=end; nit.next())
        {
            auto node_id = nit.node();

            QJsonObject node_obj;
            node_obj["id"] = QString::fromLocal8Bit(node_id.value.toByteArray());

            node_obj["in"] = packPorts(serializer, *this, node_id, connections, Direction::Input);

            node_obj["out"] = packPorts(serializer, *this, node_id, connections, Direction::Output);

            QJsonObject node_data;
            for (int i=0; i<serializer.maxSerializedRole(); ++i)
            {
                auto role = DataRole(i);
                auto role_data = nodeData(node_id, role);
                if (role_data.isNull())
                    continue;

                auto name = roleName(role);
                QString key = name ? QString::fromUtf8(name) : QString("%1").arg(i);
                node_data[key] = serializer.toJson(role_data);
            }

            node_obj["data"] = node_data;

            nodes.append(node_obj);
        }

        root["nodes"] = nodes;

        QJsonArray conn_array;
        for (auto c : connections)
        {
            QJsonObject obj;
            obj["n1"] = QString::fromLocal8Bit(c.node1.value.toByteArray());
            obj["p1"] = QString::fromLocal8Bit(c.port1.value.toByteArray());
            obj["n2"] = QString::fromLocal8Bit(c.node2.value.toByteArray());
            obj["p2"] = QString::fromLocal8Bit(c.port2.value.toByteArray());
            conn_array.append(obj);
        }

        root["connections"] = conn_array;

        data.setObject(root);
    }

    return true;
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
