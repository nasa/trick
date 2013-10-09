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

    QFrame* frame = new QFrame;
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    s->addWidget(frame);

    _left_lay = new QGridLayout(frame);
    _left_lay->setSpacing(0);
    _left_lay->setContentsMargins(12, 12, 12, 12);
    _left_lay->setObjectName(QString::fromUtf8("verticalLayout"));

    //
    // Left side panel
    //
    QTabWidget* tab = new QTabWidget(frame);
    _left_lay->addWidget(tab,0,0,1,1);

    //
    // Right side panel
    //
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    QFrame* f2 = new QFrame;
    f2->setSizePolicy(sizePolicy);
    s->addWidget(f2);

    QGridLayout* lay2 = new QGridLayout(f2);
    lay2->setSpacing(0);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setObjectName(QString::fromUtf8("layout2"));

    QString dpf("/home/vetter/dev/");
    dpf += "SET_Series30xx/DP_Product/";
    dpf += "DP_cat.xml";
    DPProduct dp(dpf);
    DPPage* page = dp.pages().at(0);
    _plot_monte = new PlotPage(page,f2);
    lay2->addWidget(_plot_monte,1,0,1,1);

    //QString dir("/home/vetter/dev/SET_Series30xx/");
    //dir += "MONTE_RUN_M_3027_i15T_i350T_IDSS-N1_iLIDS-DTS4C1.1000r";
    QString dir("/home/vetter/dev/SIM_tim");

    Monte monte(dir);
    _plot_monte->setData(&monte);

    //
    // Resize main window
    //
    resize(1600,800);
    frame->setMaximumWidth(800);
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
