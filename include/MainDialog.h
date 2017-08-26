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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with the Event Coder Tool.  If not, see <http://www.gnu.org/licenses/>.  
*/

/*
  ===NOTES===
  See MainDialog.cpp for more details on what this class and its methods do.  

 */

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QComboBox>
#include <QPointer>
#include "DataInterface.h"
#include "Logger.h"
#include "IndexDialog.h"
#include "AttributesDialog.h"
#include "RelationsDialog.h"
#include "MemoDialog.h"
#include "ExportDialog.h"
#include "ImportCodesDialog.h"
#include <vector>
#include <string>

class QPushButton;
class QLabel;
class QListWidget;
class QComboBox;
class QLineEdit;
class QTextEdit;

class MainDialog : public QDialog {
  Q_OBJECT

public:
  MainDialog(QWidget *parent=0);

signals:
  void writeLog(const QString &entry); // Still needs to be implemented.
				     
private slots: // Still have to be defined.
  void initializeSeps();
  void getFile();
  void setSep(const QString &selection);
  void readNewData();
  void importCodes();

  void saveSession();
  void loadSession();
  void processLoad(const QString &index, const QString &separator);

  void exportData();
  
  void previousEvent();
  void nextEvent();

  void toggleFlag();
  void previousFlagged();
  void nextFlagged();
  void jumpToIndex();
  void editMemo();
  
  void previousLeftEventField();
  void nextLeftEventField();
  void previousRightEventField();
  void nextRightEventField();
  void setLeftEventField(const QString &selection);
  void setRightEventField(const QString &selection);

  void assignAttribute();
  void unassignAttribute();
  void setValue(const QString &newValue);
  void setRelMemo(const QString &newRelMemo);
  void filterAttributes(const QString &text);

  void assignRelationship();
  void unassignRelationship();
  void filterRelationships(const QString &text);

  void addAttribute();
  void editAttribute();
  void removeAttributes();
  void addRelationship();
  void editRelationship();
  void removeRelationships();

  void setCurrentAttributeUnassigned(QListWidgetItem *item);
  void setCurrentAttributeAssigned(QListWidgetItem *item);
  void setCurrentRelationship(QListWidgetItem *item);

  void updateIndexIndicator();
  void disableAttributeSelection();
  void updateTexts();
  void setWorkButtons(const bool status);
  void finalBusiness();
  bool eventFilter(QObject *target, QEvent *event);
  
private:
  QPointer<DataInterface> dataInterface;
  QPointer<Logger> logger;
  // Still need to define some others.
  
  QPointer<QLabel> importFileLabel;
  QPointer<QLabel> saveLoadLabel;
  QPointer<QLabel> exportDataLabel;
  QPointer<QLabel> indexIndicatorLabel;
  QPointer<QLabel> eventFlagLabel;
  QPointer<QLabel> eventAttributesLabel;
  QPointer<QLabel> valueLabel;
  QPointer<QLabel> attributesPoolLabel;
  QPointer<QLabel> attributesFilterLabel;
  QPointer<QLabel> eventRelationshipsLabel;
  QPointer<QLabel> relationshipsPoolLabel;
  QPointer<QLabel> relationshipsFilterLabel;
  QPointer<QLabel> relMemoLabel;
  
  QPointer<QPushButton> selectFileButton;
  QPointer<QPushButton> importFileButton;
  QPointer<QPushButton> importCodesButton;
  QPointer<QPushButton> saveSessionButton;
  QPointer<QPushButton> loadSessionButton;
  QPointer<QPushButton> exitProgramButton;
  QPointer<QPushButton> exportDataButton;
  QPointer<QPushButton> previousEventButton;
  QPointer<QPushButton> nextEventButton;
  QPointer<QPushButton> previousFlaggedButton;
  QPointer<QPushButton> nextFlaggedButton;
  QPointer<QPushButton> jumpToButton;
  QPointer<QPushButton> flagButton;
  QPointer<QPushButton> memoButton;
  QPointer<QPushButton> previousLeftEventFieldButton;
  QPointer<QPushButton> nextLeftEventFieldButton;
  QPointer<QPushButton> previousRightEventFieldButton;
  QPointer<QPushButton> nextRightEventFieldButton;
  QPointer<QPushButton> assignAttributeButton;
  QPointer<QPushButton> unassignAttributeButton;
  QPointer<QPushButton> newAttributeButton;
  QPointer<QPushButton> editAttributeButton;
  QPointer<QPushButton> removeAttributesButton;
  QPointer<QPushButton> assignRelationshipButton;
  QPointer<QPushButton> unassignRelationshipButton;
  QPointer<QPushButton> newRelationshipButton;
  QPointer<QPushButton> editRelationshipButton;
  QPointer<QPushButton> removeRelationshipsButton;
  
  QPointer<QTextEdit> leftEventField;
  QPointer<QTextEdit> rightEventField;

  QPointer<QLineEdit> valueField;
  QPointer<QLineEdit> attributesFilterField;
  QPointer<QLineEdit> relationshipsFilterField;
  QPointer<QLineEdit> relMemoField;

  QPointer<QComboBox> selectSeparatorComboBox;
  QPointer<QComboBox> selectLeftEventComboBox;
  QPointer<QComboBox> selectRightEventComboBox;

  QPointer<QListWidget> assignedAttributesListWidget;
  QPointer<QListWidget> unassignedAttributesListWidget;
  QPointer<QListWidget> assignedRelationshipsListWidget;
  QPointer<QListWidget> unassignedRelationshipsListWidget;

  QString fileName;
  QString sep;
  QString currentAttribute;
  QString currentAttributeFilter;
  QString currentValue;
  QString currentRelMemo;
  QString currentRelationship;
  QString currentRelationshipFilter;
  
  std::vector<std::vector <std::string> >::size_type eventIndex;
  std::vector<std::string>::size_type leftFieldIndex;
  std::vector<std::string>::size_type rightFieldIndex;
  
  static const QString SEPDEFAULT;
  static const QString EMPTY;
  static const QString INCIDENT;
};

#endif
