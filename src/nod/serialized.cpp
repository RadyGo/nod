
// ----------------------------------------------------------------------------

#include "nod/serialized.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

Serialized::Serialized(bool reading)
    : mReading(reading)
{
}

// ----------------------------------------------------------------------------

QJsonValue Serialized::toJson(const QVariant &var)
{
    switch (var.type())
    {
    case QVariant::Rect:
    case QVariant::RectF:
        {
            QJsonArray ret;
            auto rc = var.toRectF();
            ret.append(rc.x());
            ret.append(rc.y());
            ret.append(rc.width());
            ret.append(rc.height());
            return ret;
        }
    case QVariant::Size:
    case QVariant::SizeF:
        {
            QJsonArray ret;
            auto sz = var.toSizeF();
            ret.append(sz.width());
            ret.append(sz.height());
            return ret;
        }
    break;
    case QVariant::Point:
    case QVariant::PointF:
        {
            QJsonArray ret;
            auto pt = var.toPointF();
            ret.append(pt.x());
            ret.append(pt.y());
            return ret;
        }
        break;
    default:
        break;
    }

    return var.toString();
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
