#ifndef DOC_WINDOW
#define DOC_WINDOW

#include <QMainWindow>
#include "VarTableWidget.h"
#include "TRK_DataLog.hh"

class DocWindow : public QMainWindow {

  Q_OBJECT

  public:
    DocWindow(TRK_DataLog* data_log );
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
    int foundItemIndex;
    QString searchPattern;
    QLineEdit* searchLineEdit;
    VarTableWidget* varTable;
    TRK_DataLog* datalog;
};

#endif
