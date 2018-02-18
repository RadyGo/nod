
// ----------------------------------------------------------------------------

#ifndef NOD_NODESCENE_H
#define NOD_NODESCENE_H

// ----------------------------------------------------------------------------

#include <QGraphicsScene>
#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/connectionshape.h"
#include "nod/nodegrid.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class ConnectionItem;

// ----------------------------------------------------------------------------

class NodeScene : public QGraphicsScene
{
    Q_OBJECT
public:

    NodeScene(NodeItemFactory &factory, QObject *parent=nullptr);

    void                        setDebug(bool debug) { mDebug = debug; invalidate(); }

    bool                        isDebug() const { return mDebug; }

    void                        setDrawGrid(bool draw) { mDrawGrid = draw; invalidate(); }

    bool                        drawGrid() const { return mDrawGrid; }

    NodeGrid                    &grid() { return mGrid; }

    const NodeGrid              &grid() const { return mGrid; }

    bool                        isItemMoveEnabled() const { return mItemMoveEnabled; }

    NodeModel                   *model() { return mModel; }

    const NodeModel             *model() const { return mModel; }

    virtual void                setModel(NodeModel *model);

    NodeItem                    *nodeItem(const NodeID &node);

    NodeItem                    *itemAt(const QPointF &pt, PortID &port_id);

    virtual bool                beginCreateConnection(const QPointF &pt, const NodeID &node, const PortID &port);

    virtual void                updateCreateConnection(const QPointF &pt);

    virtual bool                endCreateConnection(const QPointF &pt, const Connection &connection);

    /* QGraphicsScene */

    void                        drawBackground(QPainter *painter, const QRectF &rect) override;

    void                        drawForeground(QPainter *painter, const QRectF &rect) override;

    void                        mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    void                        mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void                        mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected slots:

    virtual void                modelDestroyed();

    virtual void                nodeConnected(NodeModel &model, const NodeID &node, const PortID &port);

    virtual void                nodeDisconnected(NodeModel &model, const NodeID &node);

    virtual void                portDisconnected(NodeModel &model, const NodeID &node, const PortID &port);

    virtual void                nodeCreated(NodeModel &model, const NodeID &node);

    virtual void                nodeDeleted(NodeModel &model, const NodeID &node);

    virtual void                sceneRectChanged(const QRectF &rect);

private:

    NodeItemFactory             &mFactory;
    NodeGrid                    mGrid;
    NodeModel                   *mModel = nullptr;
    QVector<NodeItem *>         mNodeItems;
    bool                        mItemMoveEnabled = true;
    QVector<ConnectionItem *>   mConnectionItems;
    bool                        mDebug = false;
    bool                        mDrawGrid = true;

    bool                        mCreateConnection = false;
    QPointF                     mCreatePoint;
    NodeID                      mCreateNode;
    PortID                      mCreatePort;
    QScopedPointer<ConnectionShape> mCreateShape;
    QPointF                     mCreateOffset;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODESCENE_H

// ----------------------------------------------------------------------------
