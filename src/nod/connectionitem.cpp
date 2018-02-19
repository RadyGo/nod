
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVector2D>

// ----------------------------------------------------------------------------

#include "nod/connectionitem.h"
#include "nod/connectionshape.h"
#include "nod/nodeitem.h"
#include "nod/nodescene.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

ConnectionItem::ConnectionItem(NodeScene &scene, const Connection &connection, ConnectionShape *shape)
    : mScene(scene),
      mConnection(connection),
      mShape(shape)
{
    mScene.addItem(this);

    setFlags(ItemIsSelectable);
    setZValue(ItemZ::Connection);
}

// ----------------------------------------------------------------------------

ConnectionItem::~ConnectionItem()
{
    delete mShape;
}

// ----------------------------------------------------------------------------

void ConnectionItem::updatePath()
{
    if (!mShape)
        return;

    auto item1 = mScene.nodeItem(mConnection.node1);
    auto item2 = mScene.nodeItem(mConnection.node2);
    if (!item1 || !item2)
        return;

    auto c1 = mScene.grid().snapAt(item1->mapToScene(item1->portRect(item1->boundingRect(), mConnection.port1).center()));
    auto c2 = mScene.grid().snapAt(item2->mapToScene(item2->portRect(item2->boundingRect(), mConnection.port2).center()));

    mShape->updatePath(c1, c2);

    auto cx = c1.x() < c2.x() ? c1.x() : c2.x();
    auto cy = c1.y() < c2.y() ? c1.y() : c2.y();

    setPos(cx, cy);
}

// ----------------------------------------------------------------------------

void ConnectionItem::updateGrid()
{   
    if (mShape)
        mShape->updateGrid();
}

// ----------------------------------------------------------------------------

QRectF ConnectionItem::boundingRect() const
{
    return mShape->boundingRect();
}

// ----------------------------------------------------------------------------

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (mShape)
        mShape->draw(*painter);
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
