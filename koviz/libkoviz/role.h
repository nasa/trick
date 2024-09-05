#ifndef ROLE_H
#define ROLE_H


class Role
{
  public:
    Role() :
        format(QString("")),
        alignment(Qt::AlignRight | Qt::AlignVCenter)
    {}

    virtual ~Role() {}

    QString format;
    int alignment;

    enum Roles
    {
        Format     = Qt::UserRole+0,
        EditNoEmitDataChange = Qt::UserRole+1   // no signals emitted w/ change
    };

    virtual QVariant value(int role)
    {
        QVariant ret;

        switch (role)
        {
        case Qt::TextAlignmentRole: ret = QVariant(alignment); break;
        default: break;
        };

        return ret;
    }

    virtual bool setValue(int role, QVariant val)
    {
        bool ret = false;

        switch (role)
        {
        case Format: format = val.toString(); ret = true; break;
        case Qt::TextAlignmentRole: alignment = val.toInt(); ret = true; break;
        default: break;
        };

        return ret;
    }
};

#endif // ROLE_H
