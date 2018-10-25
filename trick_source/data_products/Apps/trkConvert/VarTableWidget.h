#ifndef VARIABLE_TABLE_WIDGET_H
#define VARIABLE_TABLE_WIDGET_H

#include <string>
#include <QTableWidget> // widgets

class VarTableWidget : public QTableWidget {
    Q_OBJECT

    public:
    // Constructors
    VarTableWidget(QWidget* parent = 0);

    void addRecord( Qt::CheckState checkState, std::string name, std::string type, std::string units );  
    bool isChecked(int index);
    int recordCount();
    void scrollToRecord(int index);
    void highLightRecord(int index);
    void unHighLightRecord(int index);

    public slots:
    void checkAll();
    void unCheckAll();
};
#endif
