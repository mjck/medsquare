/*=========================================================================

 Program:   MedSquare
 Module:    MSQBTable.h

 Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
 All rights reserved.
 See Copyright.txt or http://www.medsquare.org/copyright for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef MSQ_BTABLE_H
#define MSQ_BTABLE_H

#include <QObject>
#include <QTextStream>
#include <QString>
#include <QDir>

#include <iostream>
#include <vector>

class MSQBTable : public QObject {

Q_OBJECT

public:
  struct bvalbvec
  {
    double bval;
    std::vector<double> bvec;
  };

  std::vector<bvalbvec> table;
  std::vector<bvalbvec> table_unique;

  bool empty()
  {
  	return table.size() == 0 && table_unique.size() == 0;
  }

  bool unique(double bvalue, double gx, double gy, double gz) {
  	for(int i = 0; i < table_unique.size(); i++) {
  		if (table_unique[i].bval == bvalue &&
  			table_unique[i].bvec[0] == gx &&
  			table_unique[i].bvec[1] == gy &&
  			table_unique[i].bvec[2] == gz)
  			return false;
  	}
    return true;
  }

  void add(double bvalue, double gx, double gy, double gz) {
  		//std::cout << "adding " << bvalue << ", " << gx << "," << gy << "," << gz << std::endl;
    bvalbvec item;
    item.bval = bvalue;
    item.bvec.push_back(gx);
		item.bvec.push_back(gy);
		item.bvec.push_back(gz);
    
		table.push_back(item);

		if (this->unique(bvalue, gx, gy, gz)) {
			table_unique.push_back(item);
		}

  }

  void print() {
  	for(int i = 0; i < table.size(); i++) {
  		std::cout << "BVal: " << table[i].bval << ", (" <<
  			table[i].bvec[0] << ", " <<
  			table[i].bvec[1] << ", " <<
  			table[i].bvec[2] << ")" << std::endl;
  	}
  }

  void savedat(QString filePath) {

  	QString fname = QFileInfo(filePath).baseName() + ".dat";
  	QString path = QFileInfo(filePath).path();
  	QString fileName = QDir(path).filePath(fname);
  	QFile file(fileName);
    	file.open(QIODevice::WriteOnly | QIODevice::Text);
  	QTextStream out(&file);
  	out << table_unique.size() - 1 << endl;
  	for(int i = 0; i < table_unique.size(); i++) {
  		if (table_unique[i].bval > 0)
  			out << table_unique[i].bvec[0] << " " << table_unique[i].bvec[1] << " " << table_unique[i].bvec[2] << endl;
  	}
  	file.close();
  }

  void savebvals(QString path) {

  	QString fileName = QDir(path).filePath("bvals");
  	QFile file(fileName);
  	file.open(QIODevice::WriteOnly | QIODevice::Text);
  	QTextStream out(&file);
  	for(int i = 0; i < table.size(); i++) {
  		out << table[i].bval << " ";
  	}
  	out << endl;
  	file.close();

  }

  void savebvecs(QString path) {

  	QString fileName = QDir(path).filePath("bvecs");
  	QFile file(fileName);
  	file.open(QIODevice::WriteOnly | QIODevice::Text);
  	QTextStream out(&file);
  	for(int i = 0; i < table.size(); i++) {
  		out << table[i].bvec[0] << " ";
  	}
  	out << endl;
  	for(int i = 0; i < table.size(); i++) {
  		out << table[i].bvec[1] << " ";
  	}
  	out << endl;
  	for(int i = 0; i < table.size(); i++) {
  		out << table[i].bvec[2] << " ";
  	}
  	out << endl;
  	file.close();

  }

};


#endif