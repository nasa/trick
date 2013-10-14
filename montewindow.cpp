#ifdef SNAPGUI

#include "montewindow.h"

#include <QDockWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QSortFilterProxyModel>

MonteWindow::MonteWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr("Snap!"));
    createMenu();

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    //
    // Monte Carlo Plot Page Widget
    //
    QString dpfile("/home/vetter/dev/");
    dpfile += "SET_Series30xx/DP_Product/";
    dpfile += "DP_cat6.xml";
    DPProduct dp(dpfile);
    QString montedir("/home/vetter/dev/SET_Series30xx/");
    montedir += "MONTE_RUN_M_3027_i15T_i350T_IDSS-N1_iLIDS-DTS4C1.1000r";
    Monte* monte = new Monte(montedir);
    DPPage* page = dp.pages().at(0);
    _plot_monte = new PlotPage(page);
    _plot_monte->setData(monte);
    s->addWidget(_plot_monte);

    // Resize main window
    resize(1600,800);
}

MonteWindow::~MonteWindow()
{
}

void MonteWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    setMenuWidget(_menuBar);
}

#endif // SNAPGUI2
