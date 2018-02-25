
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

// ----------------------------------------------------------------------------

#include "nod/connectionitem.h"
#include "nod/connectionshape.h"
#include "nod/nodeitem.h"
#include "nod/nodeitemfactory.h"
#include "nod/nodegrid.h"
#include "nod/nodemodel.h"
#include "nod/nodescene.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeScene::NodeScene(NodeItemFactory &factory, QObject *parent)
    : QGraphicsScene(parent),
      mFactory(factory),
      mGrid(*this)
{
    mFactory.setScene(*this);

    connect(this, &QGraphicsScene::sceneRectChanged,
            this, &NodeScene::sceneRectChanged);
}

// ----------------------------------------------------------------------------

void NodeScene::setModel(NodeModel *model)
{
    if (mModel)
        disconnect(mModel);

    mModel = model;
    mNodeItems.clear();
    mConnectionItems.clear();

    clear();

    if (mModel)
    {
        connect(mModel, &NodeModel::destroyed, this, &NodeScene::modelDestroyed);
        connect(mModel, &NodeModel::nodeCreated, this, &NodeScene::nodeCreated);
        connect(mModel, &NodeModel::nodeDeleted, this, &NodeScene::nodeDeleted);
        connect(mModel, &NodeModel::nodeConnected, this, &NodeScene::nodeConnected);
        connect(mModel, &NodeModel::nodeDisconnected, this, &NodeScene::nodeDisconnected);


        auto nit = mModel->firstNode();
        while (!nit.atEnd())
        {
            auto node = nit.node();
            nodeCreated(*mModel, node);
            nit.next();
        }

        nit = mModel->firstNode();
        while (!nit.atEnd())
        {
            auto node = nit.node();

            auto pit = mModel->firstPort(node);
            while (!pit.atEnd())
            {
                nodeConnected(*mModel, node, pit.port());
                pit.next();
            }

            nit.next();
        }
    }

    updateSceneRect();
}

// ----------------------------------------------------------------------------

NodeItem *NodeScene::nodeItem(const NodeID &node)
{
    for (auto item : mNodeItems)
    {
        if (item->node() == node)
            return item;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------

NodeItem *NodeScene::itemAt(const QPointF &pt, PortID &port_id)
{
    port_id = PortID::invalid();

    auto itms = items(pt);
    for (auto item : itms)
    {
        auto node = qgraphicsitem_cast<NodeItem*>(item);
        if (node)
        {
            auto position = node->mapFromScene(pt);
            auto port = node->portAt(position);
            if (port.isValid())
                port_id = port;

            return node;
        }
    }

    return nullptr;
}

// ----------------------------------------------------------------------------

void NodeScene::updateSceneRect()
{
    QRectF rc;
    auto all = items();
    for (auto item : all)
        rc = rc.united(item->sceneBoundingRect());

    auto gs = grid().gridSize();
    rc.adjust(-gs, -gs, gs, gs);
    rc = grid().snap(rc, false);
    if (rc != sceneRect())
    {
        setSceneRect(rc);
        invalidate(rc);
    }
}

// ----------------------------------------------------------------------------

void NodeScene::nodeMoved(NodeItem *item)
{
    qDebug() << "NodeScene: node moved" << item;

    // TODO: maintiain damage area, add item, partial grid update, partial invalidate
    mGrid.updateGrid();
    invalidate(sceneRect(), QGraphicsScene::ForegroundLayer);
    updateSceneRect();
}

// ----------------------------------------------------------------------------

bool NodeScene::beginCreateConnection(const QPointF &pt, const NodeID &node, const PortID &port)
{
    mCreateConnection = true;
    mCreatePoint = mGrid.snapAt(pt);
    mCreateNode = node;
    mCreatePort = port;
    mCreateShape.reset(mFactory.createConnectionShape());

    invalidate(sceneRect(), ForegroundLayer);

    return true;
}

// ----------------------------------------------------------------------------

void NodeScene::updateCreateConnection(const QPointF &pt)
{
    if (!mCreateConnection)
        return;

    auto sp = mGrid.snapAt(pt);
    mCreateOffset = QPointF(mCreatePoint.x() < sp.x() ? mCreatePoint.x() : sp.x(),
                            mCreatePoint.y() < sp.y() ? mCreatePoint.y() : sp.y());

    mCreateShape->updatePath(mCreatePoint, mGrid.snapAt(pt));

    invalidate(sceneRect(), ForegroundLayer);
}

// ----------------------------------------------------------------------------

bool NodeScene::endCreateConnection(const QPointF &pt, const Connection &connection)
{
    mCreateConnection = false;

    invalidate(sceneRect(), ForegroundLayer);

    return true;
}

// ----------------------------------------------------------------------------

void NodeScene::drawBackground(QPainter *painter, const QRectF &rect)
{
//    QGraphicsScene::drawBackground(painter, rect);

    // viewport BG
    painter->fillRect(rect, Qt::white);

    if (mDrawGrid)
        mGrid.draw(*painter);
}

// ----------------------------------------------------------------------------

void NodeScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);

    if (mDebug)
        mGrid.debugDraw(*painter);

    if (mCreateConnection)
    {
        auto old = painter->transform();
        painter->translate(mCreateOffset);
        mCreateShape->draw(*painter);
        painter->drawRect(mCreateShape->boundingRect());
        painter->setTransform(old);
    }
}

// ----------------------------------------------------------------------------

void NodeScene::modelDestroyed()
{
    setModel(nullptr);
}

// ----------------------------------------------------------------------------

void NodeScene::nodeConnected(NodeModel &model, const NodeID &node, const PortID &port)
{
    Q_UNUSED(model);

    for (auto item : mConnectionItems)
    {
        if (item->connection().contains(node, port))
            return;
    }

    auto item = mFactory.createConnectionItem(node, port);
    if (item)
        mConnectionItems.append(item);
}

// ----------------------------------------------------------------------------

void NodeScene::nodeDisconnected(NodeModel &model, const NodeID &node)
{
    for (auto it=model.firstPort(node), end=model.endPort(node); it!=end; it.next())
        portDisconnected(model, node, it.port());
}

// ----------------------------------------------------------------------------

void NodeScene::portDisconnected(NodeModel &model, const NodeID &node, const PortID &port)
{
    for (auto it=mConnectionItems.begin(); it!=mConnectionItems.end(); )
    {
        auto c = (*it)->connection();

        if (c.contains(node, port))
        {
            removeItem(*it);
            delete *it;
            it = mConnectionItems.erase(it);
        } else
            ++it;
    }
}

// ----------------------------------------------------------------------------

void NodeScene::nodeCreated(NodeModel &model, const NodeID &node)
{
    Q_UNUSED(model);

    for (auto item : mNodeItems)
    {
        if (item->node() == node)
            return;
    }

    auto item = mFactory.createNodeItem(node);
    if (item)
    {
        auto pos = model.nodeData(node, DataRole::Position);
        if (pos.isNull())
            pos = QPointF(0, 0);

        item->setPos(pos.toPointF());

        auto size = model.nodeData(node, DataRole::Size);
        if (size.isNull())
        {
            size = item->calculateItemSize();
            model.setNodeData(node, size, DataRole::Size);
        }

        item->setVisible(true);

        addItem(item);
        mNodeItems.append(item);
    }
}

// ----------------------------------------------------------------------------

void NodeScene::nodeDeleted(NodeModel &model, const NodeID &node)
{
    Q_UNUSED(model);

    nodeDisconnected(model, node);

    for (auto it=mNodeItems.begin(); it!=mNodeItems.end(); ++it)
    {
        if ((*it)->node() == node)
        {
            removeItem(*it);
            delete *it;
            mNodeItems.erase(it);
            break;
        }
    }
}

// ----------------------------------------------------------------------------

void NodeScene::sceneRectChanged(const QRectF &rect)
{
    mGrid.setSceneRect(rect);
}
// ----------------------------------------------------------------------------

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);

    if (mCreateConnection)
    {
        updateCreateConnection(event->scenePos());
        invalidate(sceneRect(), QGraphicsScene::ForegroundLayer);
    }
}


// ----------------------------------------------------------------------------

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    PortID port;
    auto item = itemAt(event->scenePos(), port);
    if (item && port.isValid())
    {
        qDebug() << "NodeScene: port press" << port.value;        

        if (!beginCreateConnection(event->scenePos(), item->node(), port))
            return;

        mItemMoveEnabled = false;
    }
}

// ----------------------------------------------------------------------------

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);

    if (mCreateConnection)
    {
        PortID port;
        auto item = itemAt(event->scenePos(), port);
        if (item && port.isValid())
        {
            Connection conn = { mCreateNode, mCreatePort, item->node(), port };
            endCreateConnection(event->scenePos(), conn);
        } else
            endCreateConnection(event->scenePos(), Connection::invalid());

        mCreateConnection = false;
    }

    mItemMoveEnabled = true;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
