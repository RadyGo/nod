
// ----------------------------------------------------------------------------

#ifndef NOD_ALIGNDIALOG_H
#define NOD_ALIGNDIALOG_H

// ----------------------------------------------------------------------------

#include <QDialog>

// ----------------------------------------------------------------------------

namespace nod { namespace qgs {

// ----------------------------------------------------------------------------

class AlignDialog : public QDialog
{
    Q_OBJECT
public:

    AlignDialog(QWidget *parent);

    virtual Qt::Alignment   alignment() const=0;

    virtual void            setAlignment(Qt::Alignment alignment)=0;
};

// ----------------------------------------------------------------------------

class DefaultAlignDialog : public AlignDialog
{
    Q_OBJECT
public:

    using AlignDialog::AlignDialog;

    /* AlignDialog */

    void                    accept() override;

    Qt::Alignment           alignment() const override { return mAlignment; }

    void                    setAlignment(Qt::Alignment alignment) override;

private:

    Qt::Alignment           mAlignment = Qt::AlignTop | Qt::AlignLeft;
};

// ----------------------------------------------------------------------------

} } // namespaces

// ----------------------------------------------------------------------------

#endif // NOD_ALIGNDIALOG_H

// ----------------------------------------------------------------------------
