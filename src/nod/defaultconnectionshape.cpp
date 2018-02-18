
// ----------------------------------------------------------------------------

#include <QPainter>
#include <QPen>
#include <QVector2D>

// ----------------------------------------------------------------------------

#include "nod/defaultconnectionshape.h"
#include "nod/nodegrid.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

DefaultConnectionShape::DefaultConnectionShape(NodeGrid &grid)
    : ConnectionShape(grid)
{
}

// ----------------------------------------------------------------------------

void DefaultConnectionShape::draw(QPainter &painter)
{
    // TODO: add to style

    QPen pen;
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.strokePath(mCurve, pen);
}

// ----------------------------------------------------------------------------

void DefaultConnectionShape::updateGrid()
{
    if (path().size() < 2)
        return;

    for (int i=0; i<path().size() - 1; ++i)
    {
        auto p0 = path()[i];
        auto p1 = path()[i + 1];

        auto g0 = grid().cellAt(p0);
        auto g1 = grid().cellAt(p1);
        auto gd = g1 - g0;

        auto ax = std::abs(gd.x());
        auto ay = std::abs(gd.y());
        if (ay < ax)
        {
            for (int i=0; i<=ax; ++i)
                grid().setCellBlocked(g0 + QPoint(gd.x() < 0 ? (-i) : i, 0), true);
        } else
        {
            for (int i=0; i<=ay; ++i)
                grid().setCellBlocked(g0 + QPoint(0, gd.y() < 0 ? (-i) : i), true);
        }
    }
}

// ----------------------------------------------------------------------------

void DefaultConnectionShape::updateShape()
{
    mCurve = QPainterPath();

    if (path().size() < 2)
    {
        mBounds = QRectF();
        return;
    }

    QPointF minp(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    QPointF maxp(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (int i=0; i<path().size(); ++i)
    {
        auto &pt = path()[i];
        if (pt.x() < minp.x()) minp.setX(pt.x());
        if (pt.y() < minp.y()) minp.setY(pt.y());
        if (pt.x() > maxp.x()) maxp.setX(pt.x());
        if (pt.y() > maxp.y()) maxp.setY(pt.y());
    }

    int gs = grid().gridSize();
    auto p0 = path()[0] - minp;


    for (int i=0; i<path().size()-1; ++i)
    {
        auto p1 = path()[i + 1] - minp;

        if (i < path().size() - 2)
        {
            auto p2 = path()[i + 2] - minp;
            //auto p2 = path[i + 2];

            auto d1 = QVector2D(p1 - p0).normalized();
            auto d2 = QVector2D(p2 - p1).normalized();

            if (QVector2D::dotProduct(d1, d2) < 1.0f)
            {
                mCurve.moveTo(p0);

                QVector2D cs = QVector2D(p1) - d1 * gs / 2.0f;
                QVector2D cp = (cs + d1 * gs / 2.0f);
                QVector2D ce = QVector2D(p1) + d2 * gs / 2.0f;

                mCurve.lineTo(cs.toPointF());
                mCurve.quadTo(cp.toPointF(), ce.toPointF());

                p0 = ce.toPointF();
                continue;
            }
        }

        mCurve.moveTo(p0);
        mCurve.lineTo(p1);

        p0 = p1;
    }

    mBounds = QRectF(QPointF(0, 0), maxp - minp);
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
