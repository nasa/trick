#ifndef DOC_WINDOW
#define DOC_WINDOW

#include <QMainWindow>
#include "VarTableWidget.h"
#include "TRK_DataLog.hh"

class DocWindow : public QMainWindow {

  Q_OBJECT

  public:
    DocWindow(const QString &name);
    ~DocWindow(){};

  private slots:
      void load();
      void saveAsCSV();
      void checkAll();
      void unCheckAll();

  private:
    QString trkFileName;
    VarTableWidget* varTable;
    TRK_DataLog* datalog;
};

#endif
