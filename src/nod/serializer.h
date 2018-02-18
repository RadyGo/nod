
// ----------------------------------------------------------------------------

#ifndef NOD_SERIALIZER_H
#define NOD_SERIALIZER_H

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

class Reader;
class Writer;

// ----------------------------------------------------------------------------

class Serializer
{
public:

    Serializer(QIODevice &device, Reader &reader, Writer &writer);

    void                        setMaxSerializedRole(int max) { mMaxSerializedRole = max; }

    int                         maxSerializedRole() const { return mMaxSerializedRole; }

    bool                        isReading() const { return mReading; }

    void                        setReading(bool reading) { mReading = reading; }

    bool                        isWriting() const { return !mReading; }

    void                        setWriting(bool writing) { mReading = !writing; }

    bool                        read(const NodeModel &model);

    bool                        write(const NodeModel &model);

    static QJsonValue           toJson(const QVariant &var);

    static QRectF               toRectF(const QJsonValue &var);

    static QSizeF               toSizeF(const QJsonValue &var);

    static QPointF              toPointF(const QJsonValue &var);

private:

    int                         mMaxSerializedRole = int(DataRole::User) - 1;
    bool                        mReading = true;
    QIODevice                   &mDevice;
    Reader                      &mReader;
    Writer                      &mWriter;
};

// ----------------------------------------------------------------------------

class Reader
{
public:

    virtual ~Reader() {}

    virtual bool                read(QIODevice &device, Serialized &data)=0;
};

// ----------------------------------------------------------------------------

class Writer
{
public:

    virtual ~Writer() {}

    virtual bool                write(QIODevice &device, const Serialized &data)=0;
};

// ----------------------------------------------------------------------------

class JsonReader : public Reader
{
public:

    bool                        read(QIODevice &device, Serialized &data) override;
};

// ----------------------------------------------------------------------------

class JsonWriter : public Writer
{
public:

    bool                        write(QIODevice &device, const Serialized &data) override;
};

// ----------------------------------------------------------------------------

} // namespace nod

// ----------------------------------------------------------------------------

#endif // SERIALIZER_H

// ----------------------------------------------------------------------------
