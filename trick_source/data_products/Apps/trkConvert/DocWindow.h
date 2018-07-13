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

   void formattedSave(LogFormatter &formatter);
   void textSearch();
   int textSearch(QString searchText, int startIndex, int direction);
   void findAgain(int direction);

  private slots:
      void load();
      void saveAsCSV();
      void saveAsVarList();
      void checkAll();
      void unCheckAll();
      void find();

      void findAgainForward();
      void findAgainBackward();

  private:
    QString searchPattern;
    int foundItemIndex;
    QString trkFileName;
    QLineEdit* searchLineEdit;
    VarTableWidget* varTable;
    TRK_DataLog* datalog;
};

#endif
