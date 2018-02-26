
// ----------------------------------------------------------------------------

#ifndef NOD_NODEGRID_H
#define NOD_NODEGRID_H

// ----------------------------------------------------------------------------

#include <deque>
#include <functional>

// ----------------------------------------------------------------------------

#include <QVector>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

class QPainter;

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

enum class CellUsage
{
    Empty,
    Solid,
    Connection,
    Node
};

// ----------------------------------------------------------------------------

struct GridCell
{   
    CellUsage               usage;
    int                     i, j;
    int                     from;
    float                   cost;
    float                   f, g;
    int                     visited;
};

// ----------------------------------------------------------------------------

/**
 * Costs:
 *  * cell occupied by a connection
 *  * cell occupied by a node
 *  * number of bends in the path
 *  * distance from start
 */
class PathPlanner
{
public:

    enum class Result
    {
        NoPath, // no points
        Blocked, // last path point is blocker
        Found // last point is p2
    };

    PathPlanner(NodeGrid &grid);

    Result                      plan(QVector<QPointF> &path, const QPointF &p1, const QPointF &p2, std::function<int (const GridCell &)> fn);

private:

    NodeGrid                    &mGrid;
    int                         mSearchNo = 0;
    std::deque<int>             mFrontier;

    bool                        isBlocked(const GridCell &cell) const;
};

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


    CellUsage                   cellUsage(const QPoint &pt) const;

    void                        setCellUsage(const QPoint &pt, CellUsage usage);

    void                        setCellUsage(const QRect &rc, CellUsage usage);

    QSize                       cells() const { return mCells; }

    QPoint                      cellAt(const QPointF &pt) const;

    int                         cellIndex(const QPoint &cell);

    QLine                       clipCellLine(const QLine &line);

    GridCell                    *cell(const QPoint &cell);

    GridCell                    *cell(int index);

    void                        setUsage(const QRectF &rc, CellUsage usage);

    CellUsage                   usage(const QPointF &pt) const { return cellUsage(cellAt(pt)); }

    void                        setUsage(const QPointF &pt, CellUsage usage) { setCellUsage(cellAt(pt), usage); }

    QPointF                     positionAt(const QPoint &cell, bool center=true) const;

    QPointF                     snapAt(const QPointF &pt, bool center=true) const { return positionAt(cellAt(pt), center); }

    QRectF                      snap(const QRectF &rc, bool center=true);

    PathPlanner                 &planner() { return mPlanner; }

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

    QVector<GridCell>           mGrid;
    PathPlanner                 mPlanner;

    int                         cellWeight(int index);
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_NODEGRID_H

// ----------------------------------------------------------------------------
