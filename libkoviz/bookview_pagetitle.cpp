#include "bookview_pagetitle.h"

PageTitleView::PageTitleView(QWidget *parent) :
    BookIdxView(parent)
{
    setFrameShape(QFrame::NoFrame);
}

void PageTitleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    QFont origFont = painter.font();
    QPaintDevice* paintDevice = painter.device();
    QRect R(0,0,paintDevice->width(),paintDevice->height());

    //
    // Get title strings from model
    //
    QModelIndex title1Idx;
    if ( _bookModel()->isIndex(rootIndex(),"Page")) {
        title1Idx = _bookModel()->getDataIndex(rootIndex(),"PageTitle","Page");
    } else {
        title1Idx = _bookModel()->getDataIndex(rootIndex(),
                                               "TableTitle","Table");
    }
    QModelIndex defTitlesIdx = _bookModel()->getIndex(QModelIndex(),
                                                      "DefaultPageTitles");
    QModelIndex title2Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title2",
                                                       "DefaultPageTitles");
    QModelIndex title3Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title3",
                                                       "DefaultPageTitles");
    QModelIndex title4Idx = _bookModel()->getDataIndex(defTitlesIdx,
                                                       "Title4",
                                                       "DefaultPageTitles");
    QString title1 = model()->data(title1Idx).toString();
    QString t1 = _bookModel()->getDataString(defTitlesIdx,
                                             "Title1","DefaultPageTitles");
    if ( title1.isEmpty() && t1.startsWith("koviz") ) {
        // Since subtitle has RUNs, don't use t1 (it has RUNs too)
        title1 = "Koviz Plots";
    } else if ( !t1.startsWith("koviz") ) {
        // DP page title overwritten by -t1 optional title
        title1 = t1;
    }
    QString title2 = model()->data(title2Idx).toString();
    QString title3 = model()->data(title3Idx).toString();
    QString title4 = model()->data(title4Idx).toString();

    // For estimating width of title3/4 so that main title is left of title3/4
    QFont font(origFont);
    font.setPointSize(11);
    QFontMetrics fm11(font,paintDevice);
    int w34 = fm11.boundingRect(title4).width();

    // Draw main title
    //
    font.setPointSize(14);
    painter.setFont(font);
    QFontMetrics fm1(font,paintDevice);
    int margin = 0;
    int w = fm1.boundingRect(title1).width();
    int x = (R.width()-w34-w)/2;
    int y = margin + fm1.ascent();
    if ( !title1.isEmpty() ) {
        painter.drawText(x,y,title1);
    }

    //
    // Draw subtitle with RUNs
    //
    if ( !title2.isEmpty() ) {
        font.setPointSize(11);
        painter.setFont(font);
        QFontMetrics fm2(font,paintDevice);
        QStringList lines = title2.split('\n', QString::SkipEmptyParts);
        if ( lines.size() == 1 ) {
            // single RUN
            x = (R.width()-w34-fm2.boundingRect(title2).width())/2;
            y += fm1.lineSpacing();
            painter.drawText(x,y,title2);
        } else if ( lines.size() > 1 ) {
            // multiple RUNs (show two RUNs and elide rest with elipsis)
            QString s1 = lines.at(0);
            QString s2 = lines.at(1);
            if ( lines.size() > 2 ) {
                if ( !s2.endsWith(',') ) {
                    s2 += ",";
                }
                s2 += "...)";
            }
            QString s = s1 + " " + s2;
            w = fm2.boundingRect(s).width();
            y += fm1.lineSpacing();
            if ( w < 0.6*R.width() ) {
                // print on single line
                x = (R.width()-w34-w)/2;
                painter.drawText(x,y,s);
            } else {
                // print on two lines
                QString s;
                if ( s1.size() > s2.size() ) {
                    x = (R.width()-w34-fm2.boundingRect(s1).width())/2;
                } else {
                    x = (R.width()-w34-fm2.boundingRect(s2).width())/2;
                }
                painter.drawText(x,y,s1);
                if ( s1.startsWith('(') ) {
                    x += fm2.width('(');
                }
                y += fm2.lineSpacing();
                painter.drawText(x,y,s2);
            }
        }
        y += fm2.lineSpacing();
    }

    //
    // Draw title3 and title4 (align on colon if possible)
    //
    font.setPointSize(11);
    painter.setFont(font);
    QFontMetrics fm3(font,paintDevice);
    if ( !title3.isEmpty() && title3.contains(':') &&
         !title4.isEmpty() && title4.contains(':') ) {

        // Normal case, most likely user,date with colons
        // e.g. user: vetter
        //      date: July 8, 2016
        int i = title3.indexOf(':');
        QString s31 = title3.left(i);
        QString s32 = title3.mid(i+1);
        i = title4.indexOf(':');
        QString s41 = title4.left(i);
        QString s42 = title4.mid(i+1);
        if ( s32.size() > s42.size() ) {
            w = fm3.boundingRect(s32).width();
        } else {
            w = fm3.boundingRect(s42).width();
        }
        x = R.width()-w-margin;
        y = (R.height() - fm3.lineSpacing())/2;
        painter.drawText(x,y,s32);
        y += fm3.lineSpacing();
        painter.drawText(x,y,s42);
        x -= fm3.boundingRect(" : ").width();
        y -= fm3.lineSpacing();
        painter.drawText(x,y," : ");
        y += fm3.lineSpacing();
        painter.drawText(x,y," : ");
        x -= fm3.boundingRect(s31).width();
        y -= fm3.lineSpacing();
        painter.drawText(x,y,s31);
        x += fm3.boundingRect(s31).width();
        x -= fm3.boundingRect(s41).width();
        y += fm3.lineSpacing();
        painter.drawText(x,y,s41);

    } else {
        // title3,title4 are custom (i.e. not colon delimited user/date)
        x = R.width() - fm3.boundingRect(title3).width() - margin;
        y = (R.height() - fm3.lineSpacing())/2;
        painter.drawText(x,y,title3);
        x = R.width() - fm3.boundingRect(title4).width() - margin;
        y += fm3.lineSpacing();
        painter.drawText(x,y,title4);
    }

    painter.setFont(origFont);
    painter.restore();
}
