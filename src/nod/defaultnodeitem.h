
// ----------------------------------------------------------------------------

#ifndef NOD_DEFAULTNODEITEM_H
#define NOD_DEFAULTNODEITEM_H

// ----------------------------------------------------------------------------

#include "nod/nodeitem.h"

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class DefaultNodeItem : public NodeItem
{
public:

    struct Style
    {
        QPen                    node_pen;
        QPen                    node_pen_select;
        QBrush                  node_brush;
        QBrush                  node_brush_select;
        QBrush                  node_brush_highlight;

        int                     header_height;
        QBrush                  node_header_brush;
        QBrush                  node_header_brush_select;
        QBrush                  node_header_brush_highlight;
        QFont                   node_header_text_font;
        QPen                    node_header_text_pen;

        int                     port_radius;
        QColor                  port_default_color;
        int                     port_label_spacing;
        QFont                   port_label_font;
        QPen                    port_label_pen;
    };

    using NodeItem::NodeItem;

    static Style                defaultStyle();

    virtual void                drawBackground(QPainter &painter, const QRectF &rect);

    virtual void                drawHeader(QPainter &painter, const QRectF &rect);

    virtual void                drawContent(QPainter &painter, const QRectF &rect);

    virtual void                drawPort(QPainter &painter, const QRectF &rect, const PortID &port);

    virtual void                drawPortLabel(QPainter &painter, const QRectF &rect, const PortID &port);

    virtual void                drawForeground(QPainter &painter, const QRectF &rect);

    virtual QRectF              headerRect(const QRectF &rc) const;

    virtual QRectF              contentRect(const QRectF &rc) const;

    virtual QSizeF              calculateContentSize() const;

    /* NodeItem */

    void                        draw(QPainter &painter) override;

    virtual QSizeF              calculateItemSize() const override;

    PortID                      portAt(const QPointF &pos) const override;

    QRectF                      portRect(const QRectF &rc, const PortID &port) const override;

    QRectF                      portLabelRect(const QRectF &rc, const PortID &port) const override;

    void                        updateGrid() override;

protected:

    virtual void                drawHeader(QPainter &painter, const QRectF &rect, const QString &text);

private:

    Style                       mStyle = defaultStyle();
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_DEFAULTNODEITEM_H

// ----------------------------------------------------------------------------
