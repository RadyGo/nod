
// ----------------------------------------------------------------------------

#ifndef NOD_NODEGRID_H
#define NOD_NODEGRID_H

// ----------------------------------------------------------------------------

#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

class QPainter;

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class NodeGrid
{
public:

    enum
    {
        DefaultGridSize         = 24
    };

    NodeGrid(NodeScene &scene);

    int                         gridSize() const { return mGridSize; }

    void                        setSceneRect(const QRectF &rc);


    bool                        isCellBlocked(const QPoint &pt) const;

    void                        setCellBlocked(const QPoint &pt, bool blocked);

    void                        setCellsBlocked(const QRect &rc, bool blocked);

    QSize                       cells() const { return mCells; }

    QPoint                      cellAt(const QPointF &pt) const;


    void                        setBlocked(const QRectF &rc, bool blocked);

    bool                        isBlocked(const QPointF &pt) const { return isCellBlocked(cellAt(pt)); }

    void                        setBlocked(const QPointF &pt, bool blocked) { setCellBlocked(cellAt(pt), blocked); }

    QPointF                     positionAt(const QPoint &cell, bool center=true) const;

    QPointF                     snapAt(const QPointF &pt, bool center=true) const { return positionAt(cellAt(pt), center); }

    enum class PathResult
    {
        NoPath, // no points
        Blocked, // last path point is blocker
        Finished // last point is p2
    };

    PathResult                  path(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2);

    void                        updateGrid();

    void                        updateGrid(const QRectF &area);

    void                        draw(QPainter &painter);

    void                        debugDraw(QPainter &painter);

private:

    NodeScene                   &mScene;

    int                         mGridSize = DefaultGridSize;

    QPointF                     mOrigin;
    QSizeF                      mSize;

    QSize                       mCells;
    //QVector<bool>               mGrid;

    struct Cell
    {
        int                     i, j;
        int                     cost;
        int                     visited;
    };

    QVector<Cell>               mGrid;

    int                         mSearchNo = 0;

    int                         cellWeight(int index);
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEGRID_H

// ----------------------------------------------------------------------------
