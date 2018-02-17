
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

/// Visual representation of a connection, used for drawing, collision, making connections
class ConnectionShape
{
public:

    ConnectionShape(NodeGrid &grid);
    virtual ~ConnectionShape();
    NodeGrid                    &grid() { return mGrid; }

    const QVector<QPointF>      &path() const { return mPath; }

    /** Plans a path on the grid.
     *
     * Calls updateShape() after the new path was determined.
     *
     */
    virtual void                updatePath(const QPointF &start, const QPointF &end);


    virtual QRectF              boundingRect() const=0;

    /** Draws the shape.
     *
     * @param painter The painter to draw with.
     *
     */
    virtual void                draw(QPainter &painter)=0;

    /** Updates grid cells depending on shape.
     *
     */
    virtual void                updateGrid()=0;

protected:

    virtual void                updateShape()=0;

private:

    NodeGrid                    &mGrid;
    QVector<QPointF>            mPath;
};

// ----------------------------------------------------------------------------

class DefaultConnectionShape : public ConnectionShape
{
public:

    DefaultConnectionShape(NodeGrid &grid);

    /* ConnectionShape */

    QRectF                      boundingRect() const override { return mBounds; }

    void                        draw(QPainter &painter) override;

    void                        updateGrid() override;

protected:

    void                        updateShape() override;

private:

    QRectF                      mBounds;
    QPainterPath                mCurve;
};

// ----------------------------------------------------------------------------

class ConnectionItem : public QGraphicsItem
{
public:

    enum { Type = UserType + 2 };

    ConnectionItem(NodeScene &scene, const Connection &connection, ConnectionShape *shape);
    ~ConnectionItem();

    const Connection            &connection() const { return mConnection; }

    void                        updatePath();

    void                        updateGrid();

    /* QGraphicsItem */

    int                         type() const override { return Type; }

    QRectF                      boundingRect() const override;

    void                        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /*
protected:

    void                        makeCurve(const QVector<QPointF> &path);
    */

private:

    NodeScene                   &mScene;
    Connection                  mConnection;
    ConnectionShape             *mShape = nullptr;
/*    QVector<QPointF>            mPath;
    // path along grid node centers
    QPainterPath                mCurve;
    QRectF                      mBounds;*/
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_CONNECTIONITEM_H

// ----------------------------------------------------------------------------
