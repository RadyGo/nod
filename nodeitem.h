
// ----------------------------------------------------------------------------

#ifndef NOD_NODEITEM_H
#define NOD_NODEITEM_H

// ----------------------------------------------------------------------------

#include <QGraphicsItem>

// ----------------------------------------------------------------------------

#include "nod/common.h"
#include "nod/nodescene.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class NodeItem : public QGraphicsItem
{
public:

    enum { Type = UserType + 1 };

    NodeItem(NodeScene &scene, const NodeID &node);

    NodeScene                   &scene() { return mScene; }

    const NodeScene             &scene() const { return mScene; }

    NodeModel                   *model() { return scene().model(); }

    const NodeModel             *model() const { return scene().model(); }

    const NodeID                node() const { return mNode; }

    virtual QSizeF              calculateItemSize() const=0;

    virtual void                draw(QPainter &painter)=0;

    virtual int                 portCount(Direction direction) const=0;

    virtual int                 portIndex(const PortID &port) const=0;

    virtual PortID              portAt(const QPointF &pos) const=0;

    virtual QRectF              portRect(const QRectF &rc, Direction direction, int port_index) const=0;

    virtual QRectF              portRect(const QRectF &rc, const PortID &port) const=0;

    /* QGraphicsItem */

    int                         type() const override { return Type; }

    QRectF                      boundingRect() const override;

    void                        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QVariant                    itemChange(GraphicsItemChange change, const QVariant &value) override;

private:

    NodeScene                   &mScene;
    NodeID                      mNode;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEITEM_H

// ----------------------------------------------------------------------------
