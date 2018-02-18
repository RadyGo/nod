
// ----------------------------------------------------------------------------

#ifndef NOD_CONNECTIONSHAPE_H
#define NOD_CONNECTIONSHAPE_H

// ----------------------------------------------------------------------------

#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

class QPainter;

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

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_CONNECTIONSHAPE_H

// ----------------------------------------------------------------------------
