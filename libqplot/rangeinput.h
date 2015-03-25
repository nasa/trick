#ifndef RANGEINPUT_H
#define RANGEINPUT_H

#include <QFrame>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleValidator>

class RangeInput : public QFrame
{
    Q_OBJECT
public:
    explicit RangeInput(QWidget *parent = 0);
    virtual QSize sizeHint() const;
    
signals:
    void minChanged(double min);
    void maxChanged(double max);

private slots:
    void _slotMinChanged();
    void _slotMaxChanged();

private:
    QHBoxLayout* _layout;
    QLineEdit* _minInput;
    QLineEdit* _maxInput;
    QDoubleValidator _validator;

};

#endif // RANGEINPUT_H
