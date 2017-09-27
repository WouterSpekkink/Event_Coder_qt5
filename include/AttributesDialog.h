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
  See AttributesDialog.cpp for more details on what this class and its methods do.

*/

#ifndef ATTRIBUTESDIALOG_H
#define ATTRIBUTESDIALOG_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QPlainTextEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QDateTime>
#include "../include/CategoryDialog.h"
#include "../include/DataInterface.h"
#include "../include/Logger.h"

class AttributesDialog : public QDialog {
  Q_OBJECT

public:
  AttributesDialog(QWidget *parent = 0, DataInterface *interface =  new DataInterface,
		   const QString &label = QString(), const QString &type = QString(), Logger *subLogger = new Logger);
  ~AttributesDialog() {};

private slots:
  
  void setName(const QString &newName);
  
  void setCurrentCategory(QListWidgetItem *item);
  void assignCategory();
  void unassignCategory();
  void filterCategories(const QString &text);

  void addIncidentCategory();
  void editIncidentCategory();
  void removeIncidentCategories();
  void addEntityCategory();
  void editEntityCategory();
  void removeEntityCategories();

  void disableCategorySelection();
  void updateTexts();
  
  void cancelAndClose();
  void saveAndClose();
  bool eventFilter(QObject *target, QEvent *event);
  
private:
  DataInterface *dataInterface;
  Logger *logger;
  
  QPointer<QLabel> informativeLabel;
  QPointer<QLabel> nameLabel;
  QPointer<QLabel> descriptionLabel;
  QPointer<QLabel> attributeCategoriesLabel;
  QPointer<QLabel> categoriesPoolLabel;
  QPointer<QLabel> categoriesFilterLabel;

  QPointer<QPushButton> assignCategoryButton;
  QPointer<QPushButton> unassignCategoryButton;
  QPointer<QPushButton> addCategoryButton;
  QPointer<QPushButton> editCategoryButton;
  QPointer<QPushButton> removeCategoriesButton;
  QPointer<QPushButton> cancelCloseButton;
  QPointer<QPushButton> saveCloseButton;
  
  QPointer<QLineEdit> nameField;
  QPointer<QLineEdit> categoriesFilterField;

  QPointer<QPlainTextEdit> descriptionField;

  QPointer<QListWidget> assignedCategoriesListWidget;
  QPointer<QListWidget> unassignedCategoriesListWidget;

  QString currentCategory;
  QString currentCategoryFilter;
  QString submittedLabel;
  QString submittedType;
  
  std::string name;
  std::string permanentName;
  std::string description;
  
  static const QString INCIDENT;
  static const QString ENTITY;
  static const QString EMPTY;
};

#endif
