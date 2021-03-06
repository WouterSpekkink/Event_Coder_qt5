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
  See CategoryDialog.cpp for more details on what this class and its methods do.

*/

#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QDateTime>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include "../include/DataInterface.h"
#include "../include/Logger.h"

class CategoryDialog : public QDialog {
  Q_OBJECT

public:
  CategoryDialog(QWidget *parent = 0, DataInterface *interface = new DataInterface,
		 const QString &label = QString(), const QString type = EMPTY, Logger *subLogger = new Logger);
  ~CategoryDialog() {};

private slots:
  void cancelAndClose();
  void saveAndClose();

  void setName(const QString &newName);
  
  bool eventFilter(QObject *target, QEvent *event);

private:
  QPointer<DataInterface> dataInterface;
  QPointer<Logger> logger;
  
  QPointer<QLabel> nameLabel;
  QPointer<QLabel> descriptionLabel;

  QPointer<QPushButton> cancelCloseButton;
  QPointer<QPushButton> saveCloseButton;

  QPointer<QLineEdit> nameField;

  QPointer<QPlainTextEdit> descriptionField;

  QString currentType;
  QString submittedLabel;
  std::string name;
  std::string permanentName;
  std::string description;

  static const QString EMPTY;
  static const QString INCIDENT;
  static const QString ENTITY;
};

#endif
