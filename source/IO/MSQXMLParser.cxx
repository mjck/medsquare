// Own includes
#include "MSQXMLParser.h"

// Qt includes
#include <QDomNode>
#include <QDomDocument>
#include <QDateTime>
#include <QDebug>

QVariantMap MSQXMLParser::parseXML(QIODevice *device) {
	QVariantMap result;
	QDomDocument doc;
	QString errorMessage;
	int errorLine;
	int errorColumn;
	bool success = doc.setContent(device, false, &errorMessage, &errorLine, &errorColumn);
	if (!success) {
		qDebug() << "MSQXMLParser Warning: Could not parse XML file!";
		qDebug() << "Error message: " << errorMessage;
		qDebug() << "Error line: " << errorLine;
		qDebug() << "Error column: " << errorColumn;
		return result;
	}
	QDomElement root = doc.documentElement();
	//if (root.attribute(QStringLiteral("version"), QStringLiteral("1.0")) != QLatin1String("1.0")) {
	//	qDebug() << "MSQXMLParser Warning: plist is using an unknown format version, parsing might fail unexpectedly";
	//}
	if (root.attribute(QLatin1String("version"), QLatin1String("1.0")) != QLatin1String("1.0")) {
		qDebug() << "MSQXMLParser Warning: unknown format version, parsing might fail unexpectedly";
	}
	return parseWorkbookElement(root);
}

QVariantMap MSQXMLParser::parseWorkbookElement(const QDomElement& element) {
	int count = 0;
	QVariantMap result;
	QDomNodeList children = element.childNodes();
	//std::cout << "tag top: " << element.tagName().toStdString() << "\n"; 
	for (int i = 0; i < children.count(); i++) {
		QDomNode child = children.at(i);
		QDomElement e = child.toElement();
		if (!e.isNull()) {
			QString tagName = e.tagName();
			//std::cout << "tag: " << tagName.toStdString() << "\n"; 
			if (tagName == QLatin1String("Worksheet")) {
				count++;
				//std::cout << "Worksheet " << count << std::endl;
				result[QString("Sheet %1").arg(count)] = parseWorksheetElement(e);
			}
		}
	}
	return result;
}

// Parse a worksheet. Each single worksheet may contain several tables
QVariant MSQXMLParser::parseWorksheetElement(const QDomElement& element) {
	QVariant result;
	QDomNodeList children = element.childNodes();
	if (children.count() > 0) {
		QDomNode child = children.at(0);
		QDomElement e = child.toElement();
		if (!e.isNull()) {
			QString tagName = e.tagName();
			//std::cout << "tag: " << tagName.toStdString() << "\n"; 
			if (tagName == QLatin1String("Table")) {
				return parseTableElement(e);
			}
		}
	}
	return result;
}

// Parse a table. Each table is made of a collection of rows
QVariantMap MSQXMLParser::parseTableElement(const QDomElement& element) {
	int count = 0;
	QVariantMap result;
	QDomNodeList children = element.childNodes();
	for (int i = 0; i < children.count(); i++) {
		QDomNode child = children.at(i);
		QDomElement e = child.toElement();
		if (!e.isNull()) {
			QString tagName = e.tagName();
			if (tagName == QLatin1String("Row")) {
				count++;
				//std::cout << "Row " << i << std::endl;
				result[QString("Row %1").arg(count)] = parseRowElement(e);
				//result.append(parseRowElement(e));
			}
		}
	}
	return result;
}

// Parse a row. Each row is a collection of cells
QVariantMap MSQXMLParser::parseRowElement(const QDomElement& element) {
	int index = 0;
	QVariantMap result;
	QDomNodeList children = element.childNodes();
	for (int i = 0; i < children.count(); i++) {
		QDomNode child = children.at(i);
		QDomElement e = child.toElement();
		if (!e.isNull()) {
			QString tagName = e.tagName();
			if (tagName == QLatin1String("Cell")) {
				QString attrib = e.attribute("ss:Index","None");
				if (attrib == "None") 
					index++;
				else
					index = attrib.toInt();
				//std::cout << "Cell at: " <<  attrib.toStdString() << std::endl;
				result[QString("%1").arg(index)] = parseCellElement(e);
			}
		}
	}
	return result;
}

QVariant MSQXMLParser::parseCellElement(const QDomElement& element) {
	QVariant result;
	QDomNodeList children = element.childNodes();
	if (children.count() > 0) {
		QDomNode child = children.at(0);
		QDomElement e = child.toElement();
		if (!e.isNull()) {
			QString tagName = e.tagName();
			if (tagName == QLatin1String("Data")) {
				//std::cout << "[" << e.text().toStdString() << "]" << std::endl;
				return e.text();
			}
		}
	}
	return result;
}
