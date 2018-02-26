
// ----------------------------------------------------------------------------

#include <QDebug>
#include <QPainter>

// ----------------------------------------------------------------------------

#include "nod/defaultnodeitem.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

DefaultNodeItem::Style DefaultNodeItem::defaultStyle()
{
    return {
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
        // node_header_text_font
        QFont(),
        // node_header_text_pen,
        QColor::fromRgb(0x2c, 0x2c, 0x2c),

        // port_radius
        7,
        // port_default_color
        QColor(Qt::blue).lighter(),
        // port_label_spacing
        3,
        // port_label_font
        QFont(),
        // port_label_pen
        QColor::fromRgb(0x2c, 0x2c, 0x2c)
    };
}

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

void DefaultNodeItem::drawContent(QPainter &, const QRectF &)
{

}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawPort(QPainter &painter, const QRectF &rect, const PortID &port)
{
    auto color_data = model()->portData(node(), port, DataRole::Color);
    if (!color_data.isValid())
        color_data = mStyle.port_default_color;

    auto color = qvariant_cast<QColor>(color_data);
    painter.setBrush(color);
    painter.setPen(color.darker());
    painter.drawEllipse(rect);
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawPortLabel(QPainter &painter, const QRectF &rect, const PortID &port)
{
    auto caption = model()->portData(node(), port, DataRole::Display);
    if (caption.isNull())
        caption = model()->portData(node(), port, DataRole::Name);

    painter.setPen(mStyle.port_label_pen);
    painter.drawText(rect, Qt::TextSingleLine | Qt::AlignVCenter, caption.toString());
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawForeground(QPainter &, const QRectF &)
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
    auto index = portIndex(port, direction);
    Q_ASSERT(index >= 0);
    if (index < 0)
        return QRectF();

    int size = mStyle.port_radius * 2;
    int gs = scene().grid().gridSize();
    int gsh = gs >> 1;
    int y = mStyle.header_height + gs * index + gs / 2 - size / 2;

    if (direction == Direction::Input)
        return QRectF(gsh - size / 2.0f, y, size, size);

    return QRectF(rc.right() - gsh - size / 2.0f, y, size, size);
}

// ----------------------------------------------------------------------------

QRectF DefaultNodeItem::portLabelRect(const QRectF &rc, const PortID &port) const
{
    auto direction = model()->portDirection(node(), port);
    auto port_rc = portRect(rc, port);

    auto name = model()->portData(node(), port, DataRole::Name);
    if (name.isNull())
        name = model()->portData(node(), port, DataRole::Display);

    QFontMetricsF m(mStyle.port_label_font);
    auto text_rc = m.boundingRect(QRectF(0, 0, 0xfffff, port_rc.height()), Qt::TextSingleLine | Qt::AlignVCenter, name.toString());

    auto x = 0;
    auto y = port_rc.top() + (port_rc.height() / 2 - text_rc.height() / 2);

    if (direction == Direction::Output)
    {
        x = port_rc.left() - mStyle.port_label_spacing - text_rc.width();
    } else
        x = port_rc.right() + mStyle.port_label_spacing;

    return QRectF(QPointF(x, y), text_rc.size());
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::updateGrid()
{
    auto rc = boundingRect();
    auto scene_rc = QRectF(mapToScene(rc.topLeft()), rc.size());

    scene().grid().setUsage(scene_rc.adjusted(0, 0, -1, -1), CellUsage::Node);

    for (auto it=model()->firstPort(node()); !it.atEnd(); it.next())
    {
        auto prc = portRect(rc, it.port());
        auto pt = scene_rc.topLeft() + prc.topLeft();
        //auto pt = mapToScene(prc.topLeft());
        //auto pt = prc.topLeft();
        scene().grid().setUsage(pt, CellUsage::Empty);
    }
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
                auto port_rect = portRect(rc, it.port());
                drawPort(painter, port_rect, it.port());

                auto label_rect = portLabelRect(rc, it.port());
                drawPortLabel(painter, label_rect, it.port());
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

    auto measurePorts = [this] (Direction direction) -> QRectF {

        QRectF rc;
        forAllPorts(direction, [this, &rc] (const PortID &id, int) -> bool {

            auto bounds = QRectF();
            auto port_rect = portRect(bounds, id);
            auto label_rect = portLabelRect(bounds, id);

            rc = rc.united(port_rect);
            rc = rc.united(label_rect);

            return true;
        });

        return rc;
    };

    auto in_rect = measurePorts(Direction::Input);
    auto out_rect = measurePorts(Direction::Output);

    if (size.height() < in_rect.height()) size.setHeight(in_rect.height());
    if (size.height() < out_rect.height()) size.setHeight(out_rect.height());

    auto port_w = in_rect.width() + scene().grid().gridSize() + out_rect.width();
    if (size.width() < port_w) size.setWidth(port_w);

    auto header = headerRect(QRectF(0, 0, size.width(), size.height()));
    size.setHeight(size.height() + header.height());

    auto offset = scene().grid().gridSize();
    auto p0 = scene().grid().snapAt(pos());
    auto p1 = scene().grid().snapAt(pos() + QPointF(offset, offset) + QPointF(size.width(), size.height()));

    auto pt = p1 - p0;
    return QSizeF(pt.x(), pt.y());
}

// ----------------------------------------------------------------------------

void DefaultNodeItem::drawHeader(QPainter &painter, const QRectF &rect, const QString &text)
{
    painter.fillRect(rect, isSelected() ? mStyle.node_header_brush_select : mStyle.node_header_brush);
    painter.setFont(mStyle.node_header_text_font);

    QTextOption o;
    o.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    o.setFlags(QTextOption::IncludeTrailingSpaces);
    o.setWrapMode(QTextOption::NoWrap);
    painter.setPen(mStyle.node_header_text_pen);
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
