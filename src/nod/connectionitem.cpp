
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVector2D>

// ----------------------------------------------------------------------------

#include "nod/connectionitem.h"
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

    auto rc1 = item1->portRect(item1->boundingRect(), mConnection.port1);
    auto rc2 = item2->portRect(item2->boundingRect(), mConnection.port2);

    qDebug() << "ConnectionItem: update path rect" << rc1 << rc2;

    qDebug() << "ConnectionItem: update path rect center" << rc1.center() << rc2.center();

    qDebug() << "ConnectionItem: update path scene center" << item1->mapToScene(rc1.center()) << item2->mapToScene(rc2.center());

    auto c1 = mScene.grid().snapAt(item1->mapToScene(item1->portRect(item1->boundingRect(), mConnection.port1).center()));
    auto c2 = mScene.grid().snapAt(item2->mapToScene(item2->portRect(item2->boundingRect(), mConnection.port2).center()));

    qDebug() << "ConnectionItem: update path" << c1 << c2;

    mShape->updatePath(c1, c2);

    auto cx = c1.x() < c2.x() ? c1.x() : c2.x();
    auto cy = c1.y() < c2.y() ? c1.y() : c2.y();

    qDebug() << "ConnectionItem: update pos" << cx << cy;

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

#if 0
void ConnectionItem::makeCurve(const QVector<QPointF> &path)
{
    mPath.clear();
    mCurve = QPainterPath();

    if (path.size() < 2)
    {
        mBounds = QRectF();
        return;
    }

    QPointF minp(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    for (int i=0; i<path.size()-1; ++i)
    {
        auto &pt = path[i];
        if (pt.x() < minp.x()) minp.setX(pt.x());
        if (pt.y() < minp.y()) minp.setY(pt.y());
    }

    int gs = mScene.grid().gridSize();
    int gsh = gs >> 1;
    minp -= QPointF(gsh, gsh);

    auto p0 = path[0] - minp;
    mPath.append(p0);

    QPointF maxp(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (int i=0; i<path.size()-1; ++i)
    {
        auto p1 = path[i + 1] - minp;
        mPath.append(p1);
        //auto p1 = path[i + 1];
#if 1
        if (i < path.size() - 2)
        {
            auto p2 = path[i + 2] - minp;
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
#endif

        if (p0.x() > maxp.x()) maxp.setX(p0.x());
        if (p0.y() > maxp.y()) maxp.setY(p0.y());
        if (p1.x() > maxp.x()) maxp.setX(p1.x());
        if (p1.y() > maxp.y()) maxp.setY(p1.y());

        mCurve.moveTo(p0);
        mCurve.lineTo(p1);

        p0 = p1;
    }

    maxp += QPointF(gs + gsh, gs + gsh);

    mBounds = QRectF(QPointF(0, 0), maxp - minp);
}

// ----------------------------------------------------------------------------

void ConnectionItem::updatePath()
{
    auto &grid = mScene.grid();

    auto item1 = mScene.nodeItem(mConnection.node1);
    auto c1 = item1->mapToScene(item1->portRect(item1->boundingRect(), mConnection.port1).center());

    auto item2 = mScene.nodeItem(mConnection.node2);
    auto c2 = item1->mapToScene(item2->portRect(item2->boundingRect(), mConnection.port2).center());

    mPath.clear();
    grid.path(mPath, grid.snapAt(c1), grid.snapAt(c2));

    makeCurve(mPath);
}

// ----------------------------------------------------------------------------

void ConnectionItem::updateGrid(NodeGrid &grid)
{
    if (mPath.size() < 2)
        return;

    for (int i=0; i<mPath.size() - 1; ++i)
    {
        auto p0 = mPath[i];
        auto p1 = mPath[i + 1];

        auto g0 = grid.cellAt(p0);
        auto g1 = grid.cellAt(p1);
        auto gd = g1 - g0;

        auto ax = std::abs(gd.x());
        auto ay = std::abs(gd.y());
        if (ay < ax)
        {
            for (int i=0; i<=ax; ++i)
                grid.setCellBlocked(g0 + QPoint(gd.x() < 0 ? (-i) : i, 0), true);
        } else
        {
            for (int i=0; i<=ay; ++i)
                grid.setCellBlocked(g0 + QPoint(0, gd.y() < 0 ? (-i) : i), true);
        }
    }
}
// ----------------------------------------------------------------------------

QRectF ConnectionItem::boundingRect() const
{
    //return QRectF(0, 0, 300, 160);
    return mBounds;
}

// ----------------------------------------------------------------------------

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);

    painter->drawRect(option->rect);

    QPen pen;
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

#if 0

    QVector<QPointF> path;
//    path.append({ 30, 30 });
    path.append({ 30, 90 });
    path.append({ 30, 30 });
    path.append({ 90, 30 });
    path.append({ 90, 90 });
    path.append({ 190, 90 });
    path.append({ 190, 110 });


    mOrigin = path[0];

    int gs = mScene.grid().gridSize();

    QPainterPath curve;

    auto p0 = path[0];

    QPointF maxp(-std::numeric_limits<float>::lowest(), -std::numeric_limits<float>::lowest());
    for (int i=0; i<path.size()-1; ++i)
    {
        auto p1 = path[i+1];

        if (p1.x() < mOrigin.x()) mOrigin.setX(p1.x());
        if (p1.y() < mOrigin.y()) mOrigin.setY(p1.y());
        if (p1.x() > maxp.x()) maxp.setX(p1.x());
        if (p1.y() > maxp.y()) maxp.setY(p1.y());

        if (i < path.size() - 2)
        {
            auto &p2 = path[i + 2];

            auto d1 = QVector2D(p1 - p0).normalized();
            auto d2 = QVector2D(p2 - p1).normalized();

            if (QVector2D::dotProduct(d1, d2) < 1.0f)
            {
                curve.moveTo(p0);

                QVector2D cs = QVector2D(p1) - d1 * gs / 2.0f;
                QVector2D cp = (cs + d1 * gs / 2.0f);
                QVector2D ce = QVector2D(p1) + d2 * gs / 2.0f;

                curve.lineTo(cs.toPointF());
                curve.quadTo(cp.toPointF(), ce.toPointF());

                p0 = ce.toPointF();
                continue;
            }
        }

        curve.moveTo(p0);
        curve.lineTo(p1);

        p0 = p1;
    }

    painter->strokePath(curve, pen);
#endif

    painter->strokePath(mCurve, pen);
}
#endif

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
    //int gsh = gs >> 1;
    //minp -= QPointF(gsh, gsh);

    auto p0 = path()[0] - minp;


    for (int i=0; i<path().size()-1; ++i)
    {
        auto p1 = path()[i + 1] - minp;

#if 1
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
#endif

        mCurve.moveTo(p0);
        mCurve.lineTo(p1);

        p0 = p1;
    }

    //maxp += QPointF(gs + gsh, gs + gsh);

    mBounds = QRectF(QPointF(0, 0), maxp - minp);

    qDebug() << "DefaultConnectionShape: bounds" << mBounds << maxp << minp;
}

// ----------------------------------------------------------------------------

ConnectionShape::ConnectionShape(NodeGrid &grid)
    : mGrid(grid)
{

}

// ----------------------------------------------------------------------------

ConnectionShape::~ConnectionShape()
{

}

// ----------------------------------------------------------------------------

void ConnectionShape::updatePath(const QPointF &start, const QPointF &end)
{
    qDebug() << "ConnectionShape: update path" << start << end;

#if 0
    mPath.clear();
    mGrid.path(mPath, start, end);
#else
    mPath.clear();
    mPath.push_back(start);
    mPath.push_back(end);
#endif

    updateShape();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
