
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QPainter>

// ----------------------------------------------------------------------------

#include "nod/defaultnodeitem.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

DefaultNodeItem::Style DefaultNodeItem::DefaultStyle =
{
    // node_padding
    //10,
    // node_pen
    QColor::fromRgb(0x30, 0x30, 0x30),
    // node_pen_select
    QColor::fromRgb(0xff, 0x50, 0x50),
    // node_brush
    QColor::fromRgb(0x7d, 0x7d, 0x7d),
    // node_brush_select
    QColor::fromRgb(0x94, 0x94, 0x94),
    // node_brush_highlight
    Qt::lightGray,
    // header_height (multiple of grid size)
    24,
    // node_header_brush
    QColor::fromRgb(0xbf, 0xbf, 0xbf),
    // node_header_brush_select
    QColor::fromRgb(0xd5, 0xd5, 0xd5),
    // node_header_brush_highlight
    Qt::lightGray,
    // node_header_font
    QFont(),
    // node_header_text_color,
    QColor::fromRgb(0x2c, 0x2c, 0x2c),

    // port_radius
    7,

    QColor(Qt::blue).lighter()
};

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawBackground(QPainter &painter, const QRectF &rect)
{
    painter.fillRect(rect, isSelected() ? mStyle.node_brush_select : mStyle.node_brush);
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawHeader(QPainter &painter, const QRectF &rect)
{
    auto caption = model()->nodeData(node(), DataRole::Display);
    if (caption.isNull())
        caption = model()->nodeData(node(), DataRole::Name);

    drawHeader(painter, rect, caption.toString());
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawContent(QPainter &painter, const QRectF &rect)
{

}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawPort(QPainter &painter, const QRectF &rect, const PortID &port)
{
    qDebug() << "DefaultItem: draw port" << port.value << rect;

    auto color_data = model()->portData(node(), port, DataRole::Color);
    if (!color_data.isValid())
        color_data = mStyle.port_default_color;

    auto color = qvariant_cast<QColor>(color_data);
    painter.setBrush(color);
    painter.setPen(color.darker());
    painter.drawEllipse(rect);
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawForeground(QPainter &painter, const QRectF &rect)
{

}

// ----------------------------------------------------------------------------

QRectF DefaultNodeItem::headerRect(const QRectF &rc) const
{
    return QRectF(0, 0, rc.width(), mStyle.header_height);
}

// ----------------------------------------------------------------------------

QRectF DefaultNodeItem::contentRect(const QRectF &rc) const
{
    return QRectF(0, mStyle.header_height, rc.width(), rc.height() - mStyle.header_height);
}

// ----------------------------------------------------------------------------

QRectF DefaultNodeItem::portRect(const QRectF &rc, const PortID &port) const
{
    auto direction = model()->portDirection(node(), port);
    auto index = forAllPorts(direction, [&port] (const PortID &id, int) -> bool {
        return (id == port) ? false : true;
    });

    Q_ASSERT(index >= 0);
    if (index < 0)
        return QRectF();

    QRectF prc;
    int size = mStyle.port_radius * 2;
    int gs = scene().grid().gridSize();
    int gsh = gs >> 1;
    //int y = mStyle.node_padding + mStyle.header_height + index * (mStyle.port_spacing + size);
    int y = mStyle.header_height + gs * index + gs / 2 - size / 2;
    //+ gs * index + gsh - size / 2.0f;

    if (direction == Direction::Input)
        return QRectF(gsh - size / 2.0f, y, size, size);

    return QRectF(rc.right() - gsh - size / 2.0f, y, size, size);
}

// ----------------------------------------------------------------------------

QSizeF DefaultNodeItem::calculateContentSize() const
{
    return QSizeF(0, 0);
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::draw(QPainter &painter)
{
    QRectF rc = boundingRect();

    // TODO: cache clip path and sizes

    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath clip;
    int gs = scene().grid().gridSize();
    int gsh = gs >> 1;

    int curved = gs / 3;
    int straight = gs - curved;

    int r = rc.right() - straight - curved;
    int b = rc.bottom() - straight - curved;

    clip.moveTo(0, gs);
    clip.lineTo(0, gs - straight);
    clip.quadTo(0, 0, curved, 0);
    clip.lineTo(r + straight, 0);
    clip.quadTo(r + straight + curved, 0, r + straight + curved, curved);
    clip.lineTo(r + straight + curved, b + straight);
    clip.quadTo(r + straight + curved, b + straight + curved, r + straight, b + straight + curved);
    clip.lineTo(curved, b + straight + curved);
    clip.quadTo(0, b + straight + curved, 0, b + straight);
    clip.lineTo(0, gs);

    painter.setClipPath(clip);

    drawBackground(painter, rc);

    auto header = headerRect(rc);
    drawHeader(painter, header);

    auto content = contentRect(rc);
    drawContent(painter, content);

    auto drawPorts = [this, &painter, &rc] (Direction direction)
    {
        for (auto it=model()->firstPort(node()), end=model()->endPort(node()); it!=end; it.next())
        {
            if (model()->portDirection(it.node(), it.port()) == direction)
            {
                auto rect = portRect(rc, it.port());
                drawPort(painter, rect, it.port());
            }
        }
    };

    drawPorts(Direction::Input);
    drawPorts(Direction::Output);

    drawForeground(painter, rc);

    // TODO: add to style
    QPen pen(isSelected() ? mStyle.node_pen_select : mStyle.node_pen);
    pen.setWidthF(2.6f);
    painter.strokePath(clip, pen);
}

// ----------------------------------------------------------------------------

QSizeF DefaultNodeItem::calculateItemSize() const
{
    auto size = calculateContentSize();
    auto iheight = portCount(Direction::Input) * scene().grid().gridSize();
    auto oheight = portCount(Direction::Output) * scene().grid().gridSize();

    if (size.height() < iheight) size.setHeight(iheight);
    if (size.height() < oheight) size.setHeight(oheight);

    auto header = headerRect(QRectF(0, 0, size.width(), size.height()));
    size.setHeight(size.height() + header.height());

    auto p0 = scene().grid().snapAt(pos());
    auto p1 = scene().grid().snapAt(pos() + QPointF(size.width(), size.height()));
    auto pt = p1 - p0;
    return QSizeF(pt.x(), pt.y());
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawHeader(QPainter &painter, const QRectF &rect, const QString &text)
{
    painter.fillRect(rect, isSelected() ? mStyle.node_header_brush_select : mStyle.node_header_brush);
    painter.setFont(mStyle.node_header_font);

    QTextOption o;
    o.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    o.setFlags(QTextOption::IncludeTrailingSpaces);
    o.setWrapMode(QTextOption::NoWrap);
    painter.setPen(mStyle.node_header_text_color);
    painter.drawText(rect, text, o);
}

// ----------------------------------------------------------------------------

PortID DefaultNodeItem::portAt(const QPointF &pos) const
{
    QRectF rc = boundingRect();

    for (auto it=model()->firstPort(node()), end=model()->endPort(node()); it!=end; it.next())
    {
        auto rect = portRect(rc, it.port());
        if (rect.contains(pos))
            return it.port();
    }

    return PortID::invalid();
}

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------
