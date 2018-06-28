#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

#include <QTextStream>

#include "DocWindow.h"
#include "VarTableWidget.h"
#include "CSV_Formatter.hh"

DocWindow::DocWindow(const QString &name)
    : QMainWindow( 0, 0) {

  trkFileName = name;

  // Build the Menus
  QAction * fileLoadAction = new QAction( "&Open File...", this );
  fileLoadAction->setShortcut(tr("CTRL+O"));
  connect( fileLoadAction, &QAction::triggered , this, &DocWindow::load );

  QAction * fileSaveAction = new QAction( "&Export as CSV...", this );
  connect( fileSaveAction, &QAction::triggered, this, &DocWindow::saveAsCSV );

  QMenu *fileMenu = menuBar()->addMenu("&File");

  fileMenu->addAction(fileLoadAction);
  fileMenu->addSeparator();
  fileMenu->addAction(fileSaveAction);

  QAction * editSelectAction = new QAction( "&Select All", this );
  editSelectAction->setShortcut(tr("CTRL+A"));
  connect( editSelectAction, &QAction::triggered, this, &DocWindow::checkAll );

  QAction * editClearAction = new QAction( "&Clear All", this );
  connect( editClearAction, &QAction::triggered, this, &DocWindow::unCheckAll );

  QMenu *editMenu = menuBar()->addMenu("&Edit");

  editMenu->addAction(editSelectAction);
  editMenu->addAction(editClearAction);


  // Build the Table Widget that displays the variable names, types, and units.
  varTable = new VarTableWidget(this);

  datalog = new TRK_DataLog( trkFileName.toStdString().c_str() );
  int recordCount = datalog->parameterCount();
  for (int ii = 0; ii < recordCount; ii++) {
      varTable->addRecord( Qt::Checked, 
                           datalog->parameterName(ii),
                           datalog->parameterType(ii),
                           datalog->parameterUnits(ii));
  }

  setCentralWidget(varTable);
}

void DocWindow::load() {

    QString newFileName; 

    newFileName = QFileDialog::getOpenFileName(this,
    tr("Open Data File"), ".", tr("Data Files (*.trk)"));

    if (!newFileName.isEmpty()) {
        DocWindow* w = new DocWindow(newFileName);
        w->setWindowTitle(newFileName);
        w->resize(800, 500);
        w->show();
    }
}

void DocWindow::saveAsCSV() {
     
    QTextStream out(stdout);

    CSV_Formatter csv_formatter;

    QFileInfo trkFileInfo( trkFileName);

    QString csvFileName = trkFileInfo.canonicalPath();
    csvFileName += "/";
    csvFileName += trkFileInfo.completeBaseName();
    csvFileName += csv_formatter.extension();
    out << "csvFileName = \"" << csvFileName << "\"" << endl;

    int count = varTable->recordCount();
    for (int index=0 ; index<count ; index++) {
        if (varTable->isChecked(index)) {
            datalog->selectParameter(index);
        }
    }

    FILE *fp;
    if (( fp = fopen(csvFileName.toStdString().c_str(), "w") ) != NULL) {
        datalog->formattedWrite(fp, &csv_formatter);
    }

}

void DocWindow::checkAll() {
    varTable->checkAll();
}

void DocWindow::unCheckAll() {
    varTable->unCheckAll();
}
