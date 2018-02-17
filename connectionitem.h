
// ----------------------------------------------------------------------------

#ifndef NOD_CONNECTIONITEM_H
#define NOD_CONNECTIONITEM_H

// ----------------------------------------------------------------------------

#include <QGraphicsItem>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class ConnectionItem : public QGraphicsItem
{
public:

    enum { Type = UserType + 2 };

    ConnectionItem(NodeScene &scene, const Connection &connection);

    const Connection            &connection() const { return mConnection; }

    void                        updatePath();

    void                        updateGrid(NodeGrid &grid);

    /* QGraphicsItem */

    int                         type() const override { return Type; }

    QRectF                      boundingRect() const override;

    void                        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:

    void                        makeCurve(const QVector<QPointF> &path);

private:

    NodeScene                   &mScene;
    Connection                  mConnection;
    QVector<QPointF>            mPath;
    // path along grid node centers
    QPainterPath                mCurve;
    QRectF                      mBounds;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_CONNECTIONITEM_H

// ----------------------------------------------------------------------------
