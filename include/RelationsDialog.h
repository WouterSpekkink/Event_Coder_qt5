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
  See RelationsDialog.cpp for more details on what this class and its methods do.

*/

#ifndef RELATIONSDIALOG_H
#define RELATIONSDIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include "../include/EntityDialog.h"
#include "../include/RelTypeDialog.h"
#include "../include/DataInterface.h"
#include "../include/Logger.h"

class RelationsDialog : public QDialog {
  Q_OBJECT

public:
  RelationsDialog(QWidget *parent = 0, DataInterface *interface = new DataInterface,
		  const QString &label = QString(), Logger *subLogger = new Logger);
  ~RelationsDialog() {};

private slots:
  void setCurrentEntityLeft(QListWidgetItem *item);
  void setCurrentRelType(QListWidgetItem *item);
  void setCurrentEntityRight(QListWidgetItem *item);

  void filterLeftEntity(const QString &text);
  void filterTypes(const QString &text);
  void filterRightEntity(const QString &text);
  
  void assignLeftEntity();
  void assignRelType();
  void assignRightEntity();
  void addEntity();
  void editEntity();
  void editLeftAssignedEntity();
  void editRightAssignedEntity();
  void removeEntities();
  void addRelType();
  void editRelType();
  void editAssignedRelType();
  void removeRelTypes();
  
  void updateTexts();
  void updateLeftList();
  void updateMiddleList();
  void updateRightList();
  void disableAssign();
  void reset();
  void cancelAndClose();
  void saveAndClose();

private:
  QPointer<DataInterface> dataInterface;
  QPointer<Logger> logger;
  
  QPointer<QLabel> informativeLabel;
  QPointer<QLabel> sourceLabel;
  QPointer<QLabel> typeLabel;
  QPointer<QLabel> targetLabel;
  QPointer<QLabel> selectedSourceLabel;
  QPointer<QLabel> tailLabel;
  QPointer<QLabel> selectedTypeLabel;
  QPointer<QLabel> selectedTargetLabel;
  QPointer<QLabel> headLabel;
  QPointer<QLabel> durationLabel;

  QPointer<QPushButton> resetButton;
  QPointer<QPushButton> newEntityButton;
  QPointer<QPushButton> editEntityButton;
  QPointer<QPushButton> editLeftAssignedEntityButton;
  QPointer<QPushButton> editRightAssignedEntityButton;
  QPointer<QPushButton> removeEntitiesButton;
  QPointer<QPushButton> newRelationshipButton;
  QPointer<QPushButton> editRelationshipButton;
  QPointer<QPushButton> editAssignedRelationshipButton;
  QPointer<QPushButton> removeRelationshipsButton;
  QPointer<QPushButton> assignLeftEntityButton;
  QPointer<QPushButton> assignRightEntityButton;
  QPointer<QPushButton> assignTypeButton;
  QPointer<QPushButton> cancelCloseButton;
  QPointer<QPushButton> saveCloseButton;
  
  QPointer<QListWidget> sourceListWidget;
  QPointer<QListWidget> typeListWidget;
  QPointer<QListWidget> targetListWidget;

  QPointer<QLineEdit> durationField;
  QPointer<QLineEdit> leftEntityFilterField;
  QPointer<QLineEdit> rightEntityFilterField;
  QPointer<QLineEdit> typesFilterField;

  QString submittedLabel;
  QString permanentLabel;

  QString lastSelectedEntity;
  QString currentLeftEntity;
  QString currentRightEntity;
  QString currentLeftEntitySelected;
  QString currentRightEntitySelected;
  QString currentLeftEntityFilter;
  QString currentRelTypeFilter;
  QString currentRightEntityFilter;
  QString currentRelType;
  QString currentRelTypeSelected;
  QString currentDirectedness;
    
  static const QString DEFAULT;
  static const QString UNDIRECTEDTAIL;
  static const QString DIRECTEDTAIL;
  static const QString DIRECTEDHEAD;
  static const QString DIRECTED;
  static const QString UNDIRECTED;
  static const QString EMPTY;
};

#endif
