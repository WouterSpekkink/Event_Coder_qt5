/*
 Copyright 2017 Wouter Spekkink 
 Authors : Wouter Spekkink <wouterspekkink@gmail.com> 
 Website : http://www.wouterspekkink.org

 This file is part of the Event Coder Tool.

 DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.

 Copyright 2017 Wouter Spekkink. All rights reserved.

 The Event Coder Tool is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 The Event Coder Tool is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with the Event Coder Tool.  If not, see <http://www.gnu.org/licenses/>.

*/ 

/*
  ===NOTES===
  See MemoDialog.cpp for more details on what this class and its methods do.

*/

#ifndef MEMODIALOG_H
#define MEMODIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QPointer>

class MemoDialog : public QDialog {
  Q_OBJECT

public:
  MemoDialog(QWidget *parent = 0, std::vector <std::vector <std::string> >::size_type index = 0, const std::string currentMemo = std::string());
  ~MemoDialog() {};
  QString getMemoText();
		       		  
private slots:
  void saveAndClose();
  bool eventFilter(QObject *target, QEvent *event);

  //bool eventFilter(QObject *target, QEvent *event);
  
private:
  QPointer<QLabel> memoLabel;
  QPointer<QTextEdit> memoField;
  QPointer<QPushButton> saveCloseButton;

  std::vector <std::vector <std::string> >::size_type eventIndex;
  QString memo;
    
};

#endif
