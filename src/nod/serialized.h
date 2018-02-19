
// ----------------------------------------------------------------------------

#ifndef NOD_SERIALIZED_H
#define NOD_SERIALIZED_H

// ----------------------------------------------------------------------------

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// ----------------------------------------------------------------------------

#include "nod/common.h"

// ----------------------------------------------------------------------------

class QIODevice;

// ----------------------------------------------------------------------------

namespace nod {

// ----------------------------------------------------------------------------

class Serialized
{
public:

    Serialized(bool reading);

    QJsonDocument               &doc() { return mDoc; }

    bool                        isReading() const { return mReading; }

    bool                        isWriting() const { return !mReading; }

    void                        setMaxSerializedRole(int max) { mMaxSerializedRole = max; }

    int                         maxSerializedRole() const { return mMaxSerializedRole; }

    static QJsonValue           toJson(const QVariant &var);

    static QRectF               toRectF(const QJsonValue &var);

    static QSizeF               toSizeF(const QJsonValue &var);

    static QPointF              toPointF(const QJsonValue &var);

private:

    QJsonDocument               mDoc;

    bool                        mReading = true;

    int                         mMaxSerializedRole = int(DataRole::MaxSerializedRole);

};

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

#endif // SERIALIZED_H

// ----------------------------------------------------------------------------
