
// ----------------------------------------------------------------------------

#include "nod/serializer.h"

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

Serializer::Serializer(QIODevice &device, Reader &reader, Writer &writer)
    : mDevice(device),
      mReader(reader),
      mWriter(writer)
{

}

// ----------------------------------------------------------------------------

QJsonValue Serializer::toJson(const QVariant &var)
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

bool JsonReader::read(QIODevice &device, Serialized &data)
{
    return true;
}

// ----------------------------------------------------------------------------

bool JsonWriter::write(QIODevice &device, const Serialized &data)
{
    return true;
}

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------
