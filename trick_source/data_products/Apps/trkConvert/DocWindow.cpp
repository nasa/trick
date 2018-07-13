#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include <iostream>

#include <QTextStream>

#include "DocWindow.h"
#include "VarTableWidget.h"
#include "CSV_Formatter.hh"
#include "Varlist_Formatter.hh"

// Notes:
// Need to be able to search for a variable by pattern.
// Use a QLineEdit widget for the text box.

DocWindow::DocWindow(const QString &name)
    : QMainWindow( 0, 0) {

    QTextStream out(stdout);

    foundItemIndex = 0;

    trkFileName = name;

    // Build the Menus
    QAction * fileLoadAction = new QAction( "&Open File...", this );
    fileLoadAction->setShortcut(tr("CTRL+O"));
    connect( fileLoadAction, &QAction::triggered , this, &DocWindow::load );

    QAction * csvSaveAction = new QAction( "&Export as CSV...", this );
    connect( csvSaveAction, &QAction::triggered, this, &DocWindow::saveAsCSV );

    QAction * varListSaveAction = new QAction( "&Export as Variable List...", this );
    connect( varListSaveAction, &QAction::triggered, this, &DocWindow::saveAsVarList );

    QMenu *fileMenu = menuBar()->addMenu("&File");

    fileMenu->addAction(fileLoadAction);
    fileMenu->addSeparator();
    fileMenu->addAction(csvSaveAction);
    fileMenu->addAction(varListSaveAction);


    QAction * editSelectAction = new QAction( "&Select All", this );
    editSelectAction->setShortcut(tr("CTRL+A"));
    connect( editSelectAction, &QAction::triggered, this, &DocWindow::checkAll );

    QAction * editClearAction = new QAction( "&Clear All", this );
    connect( editClearAction, &QAction::triggered, this, &DocWindow::unCheckAll );

    QMenu *editMenu = menuBar()->addMenu("&Edit");

    editMenu->addAction(editSelectAction);
    editMenu->addAction(editClearAction);

    QHBoxLayout *hbox = new QHBoxLayout();

    QPushButton *backward = new QPushButton(QChar(0x25C0), this);
    connect( backward, &QPushButton::released, this, &DocWindow::findAgainBackward);

    QPushButton *forward  = new QPushButton(QChar(0x25B6), this);
    connect( forward, &QPushButton::released, this, &DocWindow::findAgainForward);

    searchLineEdit = new QLineEdit;
    searchLineEdit->setPlaceholderText("Search Pattern");
    connect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(find()));

    hbox->addWidget(backward);
    hbox->addWidget(forward);
    hbox->addWidget(searchLineEdit);

    QVBoxLayout *vbox = new QVBoxLayout();

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

    vbox->addWidget(varTable);
    vbox->addLayout(hbox);

    QWidget *window = new QWidget;
    window->setLayout(vbox);

    setCentralWidget(window);
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

void DocWindow::formattedSave(LogFormatter &formatter) {
     
    QFileInfo trkFileInfo( trkFileName);

    QString outFileName = trkFileInfo.canonicalPath();
    outFileName += "/";
    outFileName += trkFileInfo.completeBaseName();
    outFileName += formatter.extension();

    QTextStream out(stdout);
    out << "outFileName = \"" << outFileName << "\"" << endl;

    int count = varTable->recordCount();
    for (int index=0 ; index<count ; index++) {
        if (varTable->isChecked(index)) {
            datalog->selectParameter(index);
        } else {
            datalog->deselectParameter(index);
        }         
    }

    FILE *fp;
    if (( fp = fopen(outFileName.toStdString().c_str(), "w") ) != NULL) {
        datalog->formattedWrite(fp, &formatter);
    }
}

void DocWindow::saveAsCSV() {
    CSV_Formatter csv_formatter;
    formattedSave(csv_formatter);
}

void DocWindow::saveAsVarList() {
    Varlist_Formatter varlist_formatter;
    formattedSave(varlist_formatter);
}

void DocWindow::checkAll() {
    varTable->checkAll();
}

void DocWindow::unCheckAll() {
    varTable->unCheckAll();
}

void DocWindow::findAgain(int direction) {
    if (foundItemIndex >= 0) {
        varTable->unHighLightRecord(foundItemIndex);      
        int count = varTable->recordCount();
        int startIndex = (count + foundItemIndex + direction) % count;
        if ((foundItemIndex = textSearch(searchPattern, startIndex, direction)) >= 0) {
            varTable->scrollToRecord(foundItemIndex);      
            varTable->highLightRecord(foundItemIndex);      
        }
    }
}

void DocWindow::findAgainForward() {
    findAgain(1);
}

void DocWindow::findAgainBackward() {
    findAgain(-1);
}

void DocWindow::find() {
    searchPattern = searchLineEdit->text();

    if (foundItemIndex >= 0) {
        varTable->unHighLightRecord(foundItemIndex);      
    }
    if ((foundItemIndex = textSearch(searchPattern, 0, 1)) >= 0) {
        varTable->scrollToRecord(foundItemIndex);      
        varTable->highLightRecord(foundItemIndex);      
    }
}

int DocWindow::textSearch(QString pattern, int startIndex, int direction) {

    QRegExp rx(pattern);
    int count = varTable->recordCount();
    int foundIx = -1;
    for (int i=0 ; i<count ; i++) {
        int index = (count + startIndex + direction * i) % count;
        if ((rx.indexIn(datalog->parameterName(index),0)) >= 0) {
            foundIx = index;
            break;
        }
    }
    return foundIx;
}
