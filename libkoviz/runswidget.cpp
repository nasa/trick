#include "runswidget.h"

#ifdef __linux
#include "timeit_linux.h"
#endif

RunsWidget::RunsWidget(Runs *runs,
                       PlotBookModel *bookModel,
                       MonteInputsView *monteInputsView,
                       QWidget *parent) :
    QWidget(parent),
    _runs(runs),
    _bookModel(bookModel),
    _monteInputsView(monteInputsView)
{
    // Search box
    _gridLayout = new QGridLayout(parent);
    _searchBox = new QLineEdit(parent);
    connect(_searchBox,SIGNAL(returnPressed()),
            this,SLOT(_runsSearchBoxReturnPressed()));
    _gridLayout->addWidget(_searchBox,0,0);

    // Dir/File model
    _fileModel = new QFileSystemModel;
    _fileModel->setRootPath(QDir::currentPath());

    // Filter
    _filterModel = new RunsWidgetFilterProxyModel;
    _filterModel->setSourceModel(_fileModel);
    _filterModel->setFilterRole(Qt::DisplayRole);
    _filterModel->setFilterKeyColumn(0);
    _filterModel->setFilterRegularExpression("");

    // File browser view
    _fileTreeView = new QTreeView(this);
    _fileTreeView->setModel(_filterModel);
    _fileTreeView->setDragEnabled(true);
    _gridLayout->addWidget(_fileTreeView,1,0);

    // Set browser root path
    QModelIndex sourceIndex = _fileModel->index(QDir::currentPath());
    QModelIndex proxyIndex = _filterModel->mapFromSource(sourceIndex);
    _fileTreeView->setRootIndex(proxyIndex);
}

RunsWidget::~RunsWidget()
{
    delete _fileModel;
}

void RunsWidget::_runsSearchBoxReturnPressed()
{
    QString rx = _searchBox->text();
    _filterModel->setFilterRegularExpression(rx);
}

bool RunsWidgetFilterProxyModel::filterAcceptsRow(
                                        int row,const QModelIndex &srcIdx) const
{
    if (!sourceModel()) {
        return false;
    }

    QModelIndex index = sourceModel()->index(row, 0, srcIdx);
    QString filePath = sourceModel()->data(index,
                                     QFileSystemModel::FilePathRole).toString();

    if (filePath == QDir::currentPath()) {
        // This is needed, else the model is empty
        return true;
    }

    QFileInfo fi(filePath);
    if ( fi.isDir() && filePath.contains(QDir::currentPath()) ) {
        if ( _isDirContains(filePath,filterRegularExpression(),0) ) {
            return true;
        }
    }

    return filePath.contains(filterRegularExpression());
}

bool RunsWidgetFilterProxyModel::_isDirContains(const QString &path,
                                                const QRegularExpression &rx,
                                                int depth) const
{
    if (depth > 10 ) {
        return false;
    }

    QDir dir(path);

    // Check files
    QStringList files = dir.entryList(QDir::Files|QDir::NoDotAndDotDot,
                                      QDir::Name);
    foreach (const QString &file, files) {
        if (file.contains(rx)) {
            return true;
        }
    }

    // Check directories
    QStringList dirs = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot,
                                     QDir::Name);
    foreach (const QString &subDir, dirs) {
        QString subDirPath = path + QDir::separator() + subDir;
        if (subDir.contains(rx)) {
            return true;
        } else if ( _isDirContains(subDirPath, rx, depth+1) ) {
            return true;
        }
    }

    return false;
}
