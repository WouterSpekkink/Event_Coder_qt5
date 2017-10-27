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
  See EntityDialog.cpp for more details on what this class and its methods do.

*/

#ifndef ENTITYDIALOG_H
#define ENTITYDIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QPlainTextEdit>
#include "../include/AttributesDialog.h"
#include "../include/DataInterface.h"
#include "../include/Logger.h"

class EntityDialog : public QDialog {
  Q_OBJECT

public:
  EntityDialog(QWidget *parent = 0, DataInterface *interface = new DataInterface,
	       const QString &label = QString(), Logger *subLogger = new Logger);
  ~EntityDialog() {};

  std::string getOldName();
  std::string getNewName();
		    
private slots:

  void setName(const QString &newName);
  void setValue(const QString &newValue);
  
  void filterAttributes(const QString &text);
  void setCurrentAttributeAssigned(QListWidgetItem *item);
  void setCurrentAttributeUnassigned(QListWidgetItem *item);
  void assignAttribute();
  void unassignAttribute();
  void addAttribute();
  void editAttribute();
  void removeAttributes();

  void disableAttributeSelection();
  void updateTexts();
  void cancelAndClose();
  void saveAndClose();
  bool eventFilter(QObject *target, QEvent *event);
  void reject();
  
private:
  DataInterface *dataInterface;
  Logger *logger;
  
  QPointer<QLabel> nameLabel;
  QPointer<QLabel> descriptionLabel;
  QPointer<QLabel> entityAttributesLabel;
  QPointer<QLabel> valueLabel;
  QPointer<QLabel> attributesPoolLabel;
  QPointer<QLabel> attributesFilterLabel;

  QPointer<QPushButton> assignAttributeButton;
  QPointer<QPushButton> unassignAttributeButton;
  QPointer<QPushButton> addAttributeButton;
  QPointer<QPushButton> editAttributeButton;
  QPointer<QPushButton> removeAttributesButton;
  QPointer<QPushButton> cancelCloseButton;
  QPointer<QPushButton> saveCloseButton;

  QPointer<QLineEdit> nameField;
  QPointer<QLineEdit> valueField;
  QPointer<QLineEdit> attributesFilterField;

  QPointer<QPlainTextEdit> descriptionField;

  QPointer<QListWidget> entityAttributesListWidget;
  QPointer<QListWidget> attributesPoolListWidget;

  QString submittedLabel;
  QString currentValue;
  QString currentAttribute;
  QString currentAttributeFilter;
  
  std::string name;
  std::string permanentName;
  std::string description;
  
  static const QString EMPTY;
  static const QString ENTITY;
};

#endif
