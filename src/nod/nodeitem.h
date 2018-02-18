
// ----------------------------------------------------------------------------

#ifndef NOD_NODEITEM_H
#define NOD_NODEITEM_H

// ----------------------------------------------------------------------------

#include <QGraphicsItem>

// ----------------------------------------------------------------------------

#include "nod/common.h"
#include "nod/nodemodel.h"
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

    virtual PortID              portAt(const QPointF &pos) const=0;

    virtual QRectF              portRect(const QRectF &rc, const PortID &port) const=0;

    /* QGraphicsItem */

    int                         type() const override { return Type; }

    QRectF                      boundingRect() const override;

    void                        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QVariant                    itemChange(GraphicsItemChange change, const QVariant &value) override;

protected:

    template <typename F>
    int                         forAllPorts(Direction direction, F f) const;

    int                         portCount(Direction direction) const;

private:

    NodeScene                   &mScene;
    NodeID                      mNode;
};

// ----------------------------------------------------------------------------

template<typename F>
inline int NodeItem::forAllPorts(Direction direction, F f) const
{
    int index = 0;
    for (auto it=model()->firstPort(node()), end=model()->endPort(node()); it!=end; it.next())
    {
        if (model()->portDirection(it.node(), it.port()) == direction)
        {
            if (!f(it.port(), index))
                return index;

            index++;
        }
    }
    return -1;
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEITEM_H

// ----------------------------------------------------------------------------
