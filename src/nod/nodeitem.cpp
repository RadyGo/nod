
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QStyleOptionGraphicsItem>

// ----------------------------------------------------------------------------

#include "nod/nodeitem.h"
#include "nod/nodemodel.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

NodeItem::NodeItem(NodeScene &scene, const NodeID &node)
    : mScene(scene),
      mNode(node)
{
    setZValue(ItemZ::Node);

    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsFocusable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsScenePositionChanges |
             QGraphicsItem::ItemContainsChildrenInShape);

    setAcceptHoverEvents(true);
}

// ----------------------------------------------------------------------------

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (!mScene.isItemMoveEnabled())
            return pos();

        qDebug() << "NodeItem: item pos change" << value.toPointF() << mScene.grid().snapAt(value.toPointF());
        //if (!mScene.grid().is)
        return mScene.grid().snapAt(value.toPointF(), false);
    default:
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}

// ----------------------------------------------------------------------------

QRectF NodeItem::boundingRect() const
{
    // TODO: cache this or make pos / size direct members?
    auto size = model()->nodeData(node(), DataRole::Size).toSizeF();
    return QRectF(QPointF(0, 0), size);
}

// ----------------------------------------------------------------------------

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (scene().model())
        draw(*painter);
}

// ----------------------------------------------------------------------------

int NodeItem::portCount(Direction direction) const
{
    int count = forAllPorts(direction, [] (const PortID &, int) {
        return true;
    });

    return count < 0 ? 0 : count;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
