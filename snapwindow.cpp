#ifdef SNAPGUI

#include "snapwindow.h"

#include <QDockWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>

SnapWindow::SnapWindow(Snap *snap, QWidget *parent) :
    QMainWindow(parent),_snap(snap)
{
    setWindowTitle(tr("Snap!"));

    createMenu();

    // Central Widget
    QSplitter* s = new QSplitter;
    setCentralWidget(s);

    QFrame* frame = new QFrame;
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    //frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    s->addWidget(frame);

    QGridLayout* lay = new QGridLayout(frame);
    lay->setSpacing(0);
    lay->setContentsMargins(12, 12, 12, 12);
    lay->setObjectName(QString::fromUtf8("verticalLayout"));

    QTabWidget* tab = new QTabWidget(frame);
    lay->addWidget(tab,0,0,1,1);

    for ( int ii = 0; ii < _snap->tables.size(); ++ii) {
        QTableView* tv = _create_table_view(_snap->tables.at(ii),
                                            _snap->tables.at(ii)->orientation());
        _tvs.append(tv);
        QString title = _snap->tables.at(ii)->tableName();
        tab->addTab(tv,title);

        if ( title == "Top Jobs" ) {
            tv->hideColumn(5);
            connect(tv->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                    this,
                    SLOT(_update_job_plot(QModelIndex)));
        }
    }

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::MinimumExpanding);
    QFrame* f2 = new QFrame;
    f2->setSizePolicy(sizePolicy);
    s->addWidget(f2);

    QGridLayout* lay2 = new QGridLayout(f2);
    lay2->setSpacing(0);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay2->setObjectName(QString::fromUtf8("layout2"));

    _frames = new TrickDataModel ;
    _frames->load_binary_trk("log_frame", snap->rundir());
    SnapPlot* plot = new SnapPlot(f2);
    lay2->addWidget(plot,0,0,1,1);
    plot->addCurve(_frames,0,1);
    plot->xAxis->setLabel("Time (s)");
    plot->yAxis->setLabel("Frame Scheduled Time (s)");
    plot->zoomToFit();

    _userjobs = new TrickDataModel ;
    _userjobs->load_binary_trk("log_userjobs",snap->rundir());
    _plot_jobs = new SnapPlot(f2);
    lay2->addWidget(_plot_jobs,1,0,1,1);
    _plot_jobs->addCurve(_userjobs,0,2);
    _plot_jobs->xAxis->setLabel("Time (s)");
    _plot_jobs->yAxis->setLabel("Job Time (s)");
    _plot_jobs->zoomToFit();

    _trickjobs = new TrickDataModel ;
    _trickjobs->load_binary_trk("log_trickjobs",snap->rundir());

    //
    // Resize main window
    //
    resize(1200,700);
    this->show();

    //
    // Hack to resize notebook of tables to correct size
    //
    int w = 0;
    int ncols = _snap->tables.at(3)->columnCount();
    for ( int ii = 0; ii < ncols; ++ii) {
        w += _tvs.at(3)->columnWidth(ii);  // Job Culprits
    }
    w += frame->contentsMargins().left();
    w += frame->contentsMargins().right();
    int margins = frame->frameRect().width() - frame->childrenRect().width();
    w += margins;
    frame->setMaximumWidth(w+8);
}

SnapWindow::~SnapWindow()
{
    delete _frames;
    delete _userjobs;
    delete _trickjobs;
}

void SnapWindow::createMenu()
{
    _menuBar = new QMenuBar;
    _fileMenu = new QMenu(tr("&File"), this);
    _exitAction = _fileMenu->addAction(tr("E&xit"));
    _menuBar->addMenu(_fileMenu);
    setMenuWidget(_menuBar);
    //connect(_exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

void SnapWindow::_update_job_plot(const QModelIndex &idx)
{

    QModelIndex jidx = idx.model()->index(idx.row(),5);
    QString jobname = idx.model()->data(jidx).toString();
    for ( int ii = 0; ii < _userjobs->columnCount(); ++ii) {
        QString name = _userjobs->headerData
                        (ii,Qt::Horizontal,TrickDataModel::ParamName).toString();
        if ( name == jobname ) {
            _plot_jobs->removeCurve(0);
            _plot_jobs->addCurve(_userjobs,0,ii);
        }
    }
    for ( int ii = 0; ii < _trickjobs->columnCount(); ++ii) {
        QString name = _trickjobs->headerData
                        (ii,Qt::Horizontal,TrickDataModel::ParamName).toString();
        if ( name == jobname ) {
            _plot_jobs->removeCurve(0);
            _plot_jobs->addCurve(_trickjobs,0,ii);
        }
    }
    _plot_jobs->zoomToFit();
}

QTableView* SnapWindow::_create_table_view(QAbstractItemModel *model,
                                           Qt::Orientation orientation)
{
    QTableView* tv = new QTableView();
    tv->setModel(model);
    tv->setSortingEnabled(false);
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setShowGrid(true);
    tv->setCurrentIndex(QModelIndex());
    tv->horizontalHeader()->setStretchLastSection(false);
    tv->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);

    if ( orientation == Qt::Horizontal ) {
        tv->verticalHeader()->hide();
    }

    return tv;
}


#endif // SNAPGUI
