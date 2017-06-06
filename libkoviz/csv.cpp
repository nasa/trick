#include "csv.h"
#include <QTextDecoder>

CSV::CSV(QIODevice * device)
{
	m_device = device;
	m_codec = QTextCodec::codecForLocale();
    m_pos = 0;
    m_rx = QRegExp("((?:(?:[^,\\n]*,?)|(?:\"[^\"]*\",?))*)\\n");
}
CSV::CSV(QString &string){	
	m_device = NULL;
	m_codec = QTextCodec::codecForLocale();
	m_string = string;	
	m_pos = 0;
    m_rx = QRegExp("((?:(?:[^,\\n]*,?)|(?:\"[^\"]*\",?))*)\\n");
}

CSV::~CSV()
{
	//delete m_codec;
}


void CSV::setCodec(const char* codecName){
	//delete m_codec;
    m_codec = QTextCodec::codecForName(codecName);
}

QString CSV::readLine(){
	QString line;

	if(m_string.isNull()){
		//READ DATA FROM DEVICE
		if(m_device && m_device->isReadable()){
			QTextDecoder dec(m_codec);
			m_string = dec.toUnicode(m_device->readAll());
		}else{
			return QString();
		}
	}

	//PARSE
	if((m_pos = m_rx.indexIn(m_string,m_pos)) != -1) {
		line = m_rx.cap(1);		
		m_pos += m_rx.matchedLength();
	}
	return line;
	
}

QStringList CSV::parseLine(){
    QStringList list;
    QString line(m_device->readLine(1024*1024));
    if ( line.isEmpty() ) {
        return list; // empty list - file end, hopefully!!!
    }
    line.chop(1); // chops off \n
    list = line.split(',');
    return list;
}

#if 0
QStringList CSV::parseLine(){
    while ( 1 ) {
        QStringList list = parseLine(readLine());
        if(m_string.isNull()){
            if ( m_device->atEnd() ) {

            }
        }
    }
}
#endif

QStringList CSV::parseLine(QString line){
	QStringList list;
	int pos2 = 0;
    QRegExp rx2("(?:\"([^\"]*)\",?)|(?:([^,]*),?)");
	if(line.size()<1){
		list << "";		
	}else while (line.size()>pos2 && (pos2 = rx2.indexIn(line, pos2)) != -1) {
		QString col;
		if(rx2.cap(1).size()>0)
			col = rx2.cap(1);
		else if(rx2.cap(2).size()>0)
			col = rx2.cap(2);
		
		list << col;			

		if(col.size())
			pos2 += rx2.matchedLength();
		else
			pos2++;
	}
	return list;
}
