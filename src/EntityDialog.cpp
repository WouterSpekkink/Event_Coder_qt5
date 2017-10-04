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
  This dialog can be used to add new or edit existing entities (for relationships).

*/

#include "../include/EntityDialog.h"

// Define some constants
const QString EntityDialog::EMPTY = "";
const QString EntityDialog::ENTITY = "Entity";

EntityDialog::EntityDialog(QWidget *parent, DataInterface *interface, const QString &label, Logger *subLogger) : QDialog(parent) {
  dataInterface = interface;
  logger = subLogger;
  submittedLabel = label;
  currentAttribute = "";
  currentAttributeFilter = "";
  currentValue = "";
  if (submittedLabel == EMPTY) {
    name = "";
    permanentName = "";
    description = "";
  } else {
    name = submittedLabel.toStdString();
    permanentName = name;
    std::vector <std::vector <std::string> >::iterator sIt;
    for (sIt = dataInterface->entities.begin(); sIt != dataInterface->entities.end(); sIt++) {
      std::vector<std::string> currentEntity = *sIt;
      if (currentEntity[0] == name) {
	description = currentEntity[1];
	break;
      }
    }
  }
  
  dataInterface = interface;

  // First we declare the entities of this dialog.
  nameLabel = new QLabel(tr("Name:"));
  nameField = new QLineEdit();

  descriptionLabel = new QLabel(tr("Description:"));
  descriptionField = new QPlainTextEdit();

  valueLabel = new QLabel(tr("Value:"));
  valueField = new QLineEdit();
  valueField->setEnabled(false);
    
  nameField->setText(QString::fromStdString(name));
  descriptionField->setPlainText(QString::fromStdString(description));
  nameField->installEventFilter(this);
  descriptionField->installEventFilter(this);
  valueField->installEventFilter(this);

  entityAttributesLabel = new QLabel(tr("Entity Attributes:"));
  attributesPoolLabel = new QLabel(tr("Attribute pool:"));
  entityAttributesListWidget = new QListWidget();
  entityAttributesListWidget->setSortingEnabled(true);
  attributesPoolListWidget = new QListWidget();
  attributesPoolListWidget->setSortingEnabled(true);
  assignAttributeButton = new QPushButton("<<");
  unassignAttributeButton = new QPushButton(">>");
  attributesFilterLabel = new QLabel(tr("Filter:"));
  attributesFilterField = new QLineEdit();
  addAttributeButton = new QPushButton(tr("Add new attribute"));
  editAttributeButton = new QPushButton(tr("Edit highlighted attribute"));
  removeAttributesButton = new QPushButton(tr("Remove unused attributes"));

  cancelCloseButton = new QPushButton(tr("Cancel"));
  saveCloseButton = new QPushButton(tr("Save entity"));
  
  // Then we wire the signals of the dialog.
  connect(nameField, SIGNAL(textChanged(const QString &)), this, SLOT(setName(const QString &)));
  connect(valueField, SIGNAL(textChanged(const QString &)), this, SLOT(setValue(const QString &)));
  connect(attributesFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterAttributes(const QString &)));
  connect(entityAttributesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
	  this, SLOT(setCurrentAttributeAssigned(QListWidgetItem*)));
  connect(entityAttributesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	  this, SLOT(editAttribute()));
  connect(attributesPoolListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	  this, SLOT(editAttribute()));
  connect(attributesPoolListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
	  this, SLOT(setCurrentAttributeUnassigned(QListWidgetItem*)));
  connect(assignAttributeButton, SIGNAL(clicked()), this, SLOT(assignAttribute()));
  connect(unassignAttributeButton, SIGNAL(clicked()), this, SLOT(unassignAttribute()));
  connect(addAttributeButton, SIGNAL(clicked()), this, SLOT(addAttribute()));
  connect(editAttributeButton, SIGNAL(clicked()), this, SLOT(editAttribute()));
  connect(removeAttributesButton, SIGNAL(clicked()), this, SLOT(removeAttributes()));

  connect(cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelAndClose()));
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));


  // Then we create the layout for the dialog.
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> nameLayout = new QHBoxLayout;
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(nameField);
  mainLayout->addLayout(nameLayout);
  QPointer<QHBoxLayout> descriptionLayout = new QHBoxLayout;
  descriptionLayout->addWidget(descriptionLabel);
  descriptionLayout->addWidget(descriptionField);
  mainLayout->addLayout(descriptionLayout);

  QPointer<QFrame> topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(topLine);

  QPointer<QHBoxLayout> attributeLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> assignedLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> topAssignedLayout = new QHBoxLayout;
  topAssignedLayout->addWidget(entityAttributesLabel);
  QPointer<QFrame> vertLine = new QFrame();
  vertLine->setFrameShape(QFrame::VLine);
  topAssignedLayout->addWidget(vertLine);
  topAssignedLayout->addWidget(valueLabel);
  topAssignedLayout->addWidget(valueField);
  assignedLayout->addLayout(topAssignedLayout);
  assignedLayout->addWidget(entityAttributesListWidget);
  attributeLayout->addLayout(assignedLayout);
  QPointer<QVBoxLayout> leftButtonLayout = new QVBoxLayout;
  leftButtonLayout->addWidget(assignAttributeButton);
  leftButtonLayout->addWidget(unassignAttributeButton);
  leftButtonLayout->setAlignment(Qt::AlignVCenter);
  attributeLayout->addLayout(leftButtonLayout);
  QPointer<QVBoxLayout> unassignedLayout = new QVBoxLayout;
  unassignedLayout->addWidget(attributesPoolLabel);
  unassignedLayout->addWidget(attributesPoolListWidget);
  attributeLayout->addLayout(unassignedLayout);
  QPointer<QVBoxLayout> rightButtonLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> filterLayout = new QHBoxLayout;
  filterLayout->addWidget(attributesFilterLabel);
  filterLayout->addWidget(attributesFilterField);
  rightButtonLayout->addLayout(filterLayout);
  rightButtonLayout->addWidget(addAttributeButton);
  rightButtonLayout->addWidget(editAttributeButton);
  rightButtonLayout->addWidget(removeAttributesButton);
  rightButtonLayout->setAlignment(Qt::AlignVCenter);
  attributeLayout->addLayout(rightButtonLayout);
  mainLayout->addLayout(attributeLayout);
  
  QPointer<QFrame> bottomLine = new QFrame();
  bottomLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(bottomLine);

  QPointer<QHBoxLayout> optionLayout = new QHBoxLayout;
  optionLayout->addWidget(cancelCloseButton);
  optionLayout->addWidget(saveCloseButton);
  optionLayout->setSpacing(600);
  mainLayout->addLayout(optionLayout);

  setLayout(mainLayout);
  setWindowTitle("Add /Edit Entity");
  setFixedHeight(sizeHint().height());

  updateTexts();
  // And that finishes this constructor.
}
    
void EntityDialog::setName(const QString &newName) {
  name = newName.toStdString();
  if (submittedLabel == EMPTY) {
    std::string oldName = permanentName;
    permanentName = name;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationships.size(); i++) {
      if (dataInterface->relationships[i][1] == oldName) {
	dataInterface->relationships[i][1] = name;
      }
      if (dataInterface->relationships[i][3] == oldName) {
	dataInterface->relationships[i][3] = name;
      }
    }
  }
}

void EntityDialog::setValue(const QString &newValue) {
  if (currentAttribute != "") {
    std::string temp = newValue.toStdString();
    temp.erase(std::remove(temp.begin(), temp.end(), ';'), temp.end());
    temp.erase(std::remove(temp.begin(), temp.end(), '|'), temp.end());
    currentValue = QString::fromStdString(temp);
  }
  std::vector <std::vector <std::string> >::iterator it;
  if (currentValue != "") {
    std::vector<std::string> tempValue;
    tempValue.push_back(currentAttribute.toStdString());
    tempValue.push_back(permanentName);
    tempValue.push_back(currentValue.toStdString());
    bool found = false;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityValues.size(); i++) {
      std::vector<std::string> currentValue = dataInterface->entityValues[i];
      if (currentValue[0] == tempValue[0] && currentValue[1] == tempValue[1]) {
	dataInterface->entityValues[i][2] = tempValue[2];
	found = true;
      }
    }
    if (!found) {
      dataInterface->entityValues.push_back(tempValue);
    }
    
  } else {
    for (it = dataInterface->entityValues.begin(); it != dataInterface->entityValues.end();) {
      std::vector<std::string> currentValue = *it;
      if (currentValue[0] == currentAttribute.toStdString() && currentValue[1] == permanentName) {
	dataInterface->entityValues.erase(it);
      } else {
	it++;
      } 
    }
  }
}

void EntityDialog::filterAttributes(const QString &text) {
  disableAttributeSelection();
  currentAttributeFilter = text;
  updateTexts();
}

void EntityDialog::setCurrentAttributeUnassigned(QListWidgetItem *item) {
  currentAttribute = item->text();
  valueField->blockSignals(true);
  valueField->setText("");
  valueField->blockSignals(false);
  valueField->setEnabled(false);
}

void EntityDialog::setCurrentAttributeAssigned(QListWidgetItem *item) {
  currentAttribute = item->text();
  std::vector <std::vector <std::string> >::iterator it;
  valueField->setEnabled(true);
  currentValue = "";
  valueField->blockSignals(true);
  valueField->setText(currentValue);
  valueField->blockSignals(false);
  for (it = dataInterface->entityValues.begin(); it != dataInterface->entityValues.end(); it++) {
    std::vector<std::string> currentGroup = *it;
    if (currentGroup[0] == currentAttribute.toStdString() && currentGroup[1] == permanentName) {
      currentValue = QString::fromStdString(currentGroup[2]);
      valueField->blockSignals(true);
      valueField->setText(currentValue);
      valueField->blockSignals(false);
      return;
    }
  }
}

void EntityDialog::assignAttribute() {
  std::vector <std::vector <std::string> >::iterator sIt;
  bool foundAttribute = false;
  bool foundEntity = false;
  permanentName.erase(std::remove(permanentName.begin(), permanentName.end(), ';'), permanentName.end());
  permanentName.erase(std::remove(permanentName.begin(), permanentName.end(), '|'), permanentName.end());
  permanentName.erase(permanentName.find_last_not_of(" \n\r\t")+1);
  std::vector <std::vector <std::string> >::size_type attributeIndex;
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedEntityAttributes.size(); i++) {
    std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributes[i];
    if (currentGroup[0] == currentAttribute.toStdString()) {
      foundAttribute = true;
      attributeIndex = i;
      std::vector<std::string>::iterator pIt;
      for (pIt = currentGroup.begin() + 1; pIt != currentGroup.end(); pIt++) {
	if (*pIt == permanentName) {
	  foundEntity = true;
	}
      }
    }
  }
  if (!foundAttribute) {
    std::vector <std::string> tempPair;
    tempPair.push_back(currentAttribute.toStdString());
    tempPair.push_back(permanentName);
    dataInterface->assignedEntityAttributes.push_back(tempPair);
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Assigned attribute " + currentAttribute + " to entity " + QString::fromStdString(permanentName);
    logger->addToLog(newLog);
  } else if (foundAttribute && !foundEntity) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Assigned attribute " + currentAttribute + " to entity " + QString::fromStdString(permanentName);
    logger->addToLog(newLog);
    dataInterface->assignedEntityAttributes[attributeIndex].push_back(permanentName);
  }
  disableAttributeSelection();
  updateTexts();
}

void EntityDialog::unassignAttribute() {
  std::vector <std::vector <std::string> >::iterator sIt;
  std::vector<std::string>::size_type eraseIndex = 0;
  bool foundAttribute = false;
  bool foundEntity = false;
  std::vector <std::vector <std::string> >::size_type attributeIndex;
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedEntityAttributes.size(); i++) {
    std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributes[i];
    if (currentGroup[0] == currentAttribute.toStdString()) {
      foundAttribute = true;
      attributeIndex = i;
      for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	if (currentGroup[j] == permanentName) {
	  foundEntity = true;
	  eraseIndex = j;
	}
      }
    }
  }
  if (foundAttribute && foundEntity) {
    dataInterface->assignedEntityAttributes[attributeIndex].erase(dataInterface->
							  assignedEntityAttributes[attributeIndex].begin() + eraseIndex);
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Unassigned attribute " + currentAttribute + " from entity " +
      QString::fromStdString(permanentName);
    logger->addToLog(newLog);
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->assignedEntityAttributes.begin(); it != dataInterface->assignedEntityAttributes.end();) {
      std::vector<std::string> currentAssigned = *it;
      if (currentAssigned.size() < 2) {
	dataInterface->assignedEntityAttributes.erase(it);
      } else {
	it++;
      }
    }
    for (it = dataInterface->entityValues.begin(); it != dataInterface->entityValues.end();) {
      std::vector<std::string> currentValue = *it;
      if (currentValue[0] == currentAttribute.toStdString() && currentValue[1] == permanentName) {
	dataInterface->entityValues.erase(it);
      } else {
	it++;
      }
    }
  }
  disableAttributeSelection();
  updateTexts();
}

void EntityDialog::addAttribute() {
  disableAttributeSelection();
  AttributesDialog *attributesDialog = new AttributesDialog(this, dataInterface, EMPTY, ENTITY, logger);
  attributesDialog->exec();
  delete attributesDialog;
  updateTexts();
}

void EntityDialog::editAttribute() {
  if (currentAttribute != "") {
    AttributesDialog *attributesDialog = new AttributesDialog(this, dataInterface, currentAttribute, ENTITY, logger);
    disableAttributeSelection();
    attributesDialog->exec();
    delete attributesDialog;
    updateTexts();
  }
}

void EntityDialog::removeAttributes() {
  disableAttributeSelection();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->entityAttributes.begin(); it != dataInterface->entityAttributes.end();) {
    std::vector<std::string> currentAttribute = *it;
    bool found = false;
    for (it2 = dataInterface->assignedEntityAttributes.begin(); it2 != dataInterface->assignedEntityAttributes.end(); it2++) {
      std::vector<std::string> currentAssigned = *it2;
      if (currentAssigned[0] == currentAttribute[0]) {
	found = true;
      }
    }
    if (!found) {
      if (removed.size() == 0) {
	removed.append(QString::fromStdString(*it->begin()));
      } else {
	removed.append(", ");
	removed.append(QString::fromStdString(*it->begin()));
      }
      std::vector <std::vector <std::string> >::iterator it3;
      for (it3 = dataInterface->assignedEntityAttributeCategories.begin();
	   it3 != dataInterface->assignedEntityAttributeCategories.end();) {
	std::vector<std::string>::iterator it4;
	for (it4 = it3->begin() + 1; it4 != it3->end();) {
	  if (*it4 == currentAttribute[0]) {
	    it3->erase(it4);
	  } else {
	    it4++;
	  }
	}
	if (it3->size() < 2) {
	  dataInterface->assignedEntityAttributeCategories.erase(it3);
	} else {
	  it3++;
	}
      }
      dataInterface->entityAttributes.erase(it);
    } else {
      it++;
    }
  }
  if (removed.size() > 0) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Removed unused categories " + removed;
    logger->addToLog(newLog);
  }
  updateTexts();
}

void EntityDialog::disableAttributeSelection() {
  currentAttribute = "";
  currentValue = "";
  valueField->setEnabled(false);
  valueField->blockSignals(true);
  valueField->setText(currentValue);
  valueField->blockSignals(false);
}

void EntityDialog::updateTexts() {
  entityAttributesListWidget->clear();
  attributesPoolListWidget->clear();
  std::vector <std::vector <std::string> >::iterator sIt;
  std::vector <std::vector <std::string> >::iterator sIt2;
  for (sIt = dataInterface->entityAttributes.begin(); sIt != dataInterface->entityAttributes.end(); sIt++) {
    bool found = false;
    std::vector<std::string> currentAttr= *sIt;
    for (sIt2 = dataInterface->assignedEntityAttributes.begin(); sIt2 != dataInterface->assignedEntityAttributes.end(); sIt2++) {
      std::vector<std::string> currentAssigned = *sIt2;
      if (currentAssigned[0] == currentAttr[0] && !found) {
	for (std::vector<std::string>::size_type i = 1; i != currentAssigned.size(); i++) {
	  std::string currentEnt = currentAssigned[i];
	  if (currentEnt == permanentName) {
	    found = true;
	  }
	}
      }
    }
    if (!found) {
      if (currentAttributeFilter != "") {
	std::size_t found = currentAttr[0].find(currentAttributeFilter.toStdString());
	if (found != std::string::npos) {
	  attributesPoolListWidget->addItem(QString::fromStdString(currentAttr[0]));
	}
      } else {
	attributesPoolListWidget->addItem(QString::fromStdString(currentAttr[0]));
      }
    } else {
      if (currentAttributeFilter != "") {
	std::size_t found = currentAttr[0].find(currentAttributeFilter.toStdString());
	if (found != std::string::npos) {
	  entityAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));	
      	}
      } else {
	entityAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));
      }
    }
  }
  for (int i = 0; i != entityAttributesListWidget->count(); i++) {
    std::string currentItem = entityAttributesListWidget->item(i)->text().toStdString();
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->entityAttributes.begin(); it != dataInterface->entityAttributes.end(); it++) {
      std::vector<std::string> currentAttrib = *it;
      if (currentAttrib[0] == currentItem) {
	entityAttributesListWidget->item(i)->setToolTip(QString::fromStdString(currentAttrib[1]));
      }
    }
  }
  for (int i = 0; i != attributesPoolListWidget->count(); i++) {
    std::string currentItem = attributesPoolListWidget->item(i)->text().toStdString();
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->entityAttributes.begin(); it != dataInterface->entityAttributes.end(); it++) {
      std::vector<std::string> currentAttrib = *it;
      if (currentAttrib[0] == currentItem) {
	attributesPoolListWidget->item(i)->setToolTip(QString::fromStdString(currentAttrib[1]));
      }
    }
  }
}

std::string EntityDialog::getOldName() {
  return permanentName;
}

std::string EntityDialog::getNewName() {
  return name;
}

void EntityDialog::cancelAndClose() {
  disableAttributeSelection();
  if (submittedLabel == EMPTY) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new entity";
    logger->addToLog(newLog);
  } else {
    name = permanentName;
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing entity " + submittedLabel;
    logger->addToLog(newLog);
  }
  this->close();
}
  
void EntityDialog::saveAndClose() {
  disableAttributeSelection();
  description = (descriptionField->toPlainText()).toStdString();
  bool createNew = true;
  name.erase(std::remove(name.begin(), name.end(), ';'), name.end());
  name.erase(std::remove(name.begin(), name.end(), '|'), name.end());
  name.erase(name.find_last_not_of(" \n\r\t")+1);
  description.erase(std::remove(description.begin(), description.end(), ';'), description.end());
  description.erase(std::remove(description.begin(), description.end(), '|'), description.end());
  description.erase(description.find_last_not_of(" \n\r\t")+1);
  if (name != "" && description != "") {
    std::vector<std::string> tempEntity;
    tempEntity.push_back(name);
    tempEntity.push_back(description);
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entities.size(); i++) {
      std::vector<std::string> currentEntity = dataInterface->entities[i];
      if (currentEntity[0] == name && currentEntity[0] != permanentName) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Entity already exists.");
	warningBox->setInformativeText("You cannot create two entities with identical names.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entities.size(); i++) {
      std::vector<std::string> currentEntity = dataInterface->entities[i];
      if (currentEntity[0] == permanentName && submittedLabel == EMPTY) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Entity already exists.");
	warningBox->setInformativeText("You cannot create two entities with identical names.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entities.size(); i++) {
      std::vector<std::string> currentEntity = dataInterface->entities[i];
      if (currentEntity[0] == permanentName && submittedLabel != EMPTY) {
	dataInterface->entities[i][0] = name;
	dataInterface->entities[i][1] = description;
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Edited entity " + submittedLabel +
	  ", which is now saved as " + QString::fromStdString(name);
	logger->addToLog(newLog);

	/* 
	   We should also check:
	   (1) if this entity has any attributes assigned to it already
	   (2) if there are any relationships to which this entity was assigned.
	*/
	for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->assignedEntityAttributes.size(); j++) {
	  std::vector<std::string> currentAttribute = dataInterface->assignedEntityAttributes[j];
	  for (std::vector<std::string>::size_type k = 1; k != currentAttribute.size(); k++) {
	    if (dataInterface->assignedEntityAttributes[j][k] == permanentName) {
	      dataInterface->assignedEntityAttributes[j][k] = name;
	    }
	  }
	}
	for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->relationships.size(); j++) {
	  if (dataInterface->relationships[j][1] == permanentName) {
	    std::string currentRelType = dataInterface->relationships[j][2];
	    std::string directedness = "";
	    for (std::vector <std::vector <std::string> >::size_type k = 0; k != dataInterface->relationshipTypes.size(); k++) {
	      if (currentRelType == dataInterface->relationshipTypes[k][0]) {
		directedness = dataInterface->relationshipTypes[k][2];
	      }
	    }
	    std::string tail = "";
	    std::string head = "]-->";
	    if (directedness == "Directed") {
	      tail = "---[";
	    } else if (directedness == "Undirected") {
	      tail = "<--[";
	    }
	    std::string oldLabel = permanentName + tail + currentRelType + head + dataInterface->relationships[j][3];
	    std::string newLabel = name + tail + currentRelType + head + dataInterface->relationships[j][3];
	    dataInterface->relationships[j][0] = newLabel;
	    dataInterface->relationships[j][1] = name;
	    for (std::vector <std::vector <std::string> >::size_type l = 0; l != dataInterface->assignedRelationships.size(); l++) {
	      if (dataInterface->assignedRelationships[l][0] == oldLabel) {
		dataInterface->assignedRelationships[l][0] = newLabel;
	      }
	    }
	    for (std::vector <std::vector <std::string> >::size_type l = 0; l != dataInterface->relationMemos.size(); l++) {
	      if (dataInterface->relationMemos[l][0] == oldLabel) {
		dataInterface->relationMemos[l][0] = newLabel;
	      }
	    }
	  } else if (dataInterface->relationships[j][3] == permanentName) {
	    std::string currentRelType = dataInterface->relationships[j][2];
	    std::string directedness = "";
	    for (std::vector <std::vector <std::string> >::size_type k = 0; k != dataInterface->relationshipTypes.size(); k++) {
	      if (currentRelType == dataInterface->relationshipTypes[k][0]) {
		directedness = dataInterface->relationshipTypes[k][2];
	      }
	    }
	    std::string tail = "";
	    std::string head = "]-->";
	    if (directedness == "Directed") {
	      tail = "---[";
	    } else if (directedness == "Undirected") {
	      tail = "<--[";
	    }
	    std::string oldLabel = dataInterface->relationships[j][1] + tail + currentRelType + head + permanentName;
	    std::string newLabel = dataInterface->relationships[j][1] + tail + currentRelType + head + name;
	    dataInterface->relationships[j][0] = newLabel;
	    dataInterface->relationships[j][3] = name;
	    for (std::vector <std::vector <std::string> >::size_type l = 0; l != dataInterface->assignedRelationships.size(); l++) {
	      if (dataInterface->assignedRelationships[l][0] == oldLabel) {
		dataInterface->assignedRelationships[l][0] = newLabel;
	      }
	    }
	    for (std::vector <std::vector <std::string> >::size_type l = 0; l != dataInterface->relationMemos.size(); l++) {
	      if (dataInterface->relationMemos[l][0] == oldLabel) {
		dataInterface->relationMemos[l][0] = newLabel;
	      }
	    }
	  }
	}
	createNew = false;
	this->close();
      }
    }
    if (createNew) {
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Created new entity " + QString::fromStdString(name);
      logger->addToLog(newLog);
      dataInterface->entities.push_back(tempEntity);
      this->close();
    }
  } else if (name == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No Label provided.");
    warningBox->setInformativeText("You need to enter a label to save the entity.");
    warningBox->exec();
    return;
  } else if (description == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No description provided.");
    warningBox->setInformativeText("You need to enter a description to save the entity.");
    warningBox->exec();
    return;
  }
}

bool EntityDialog::eventFilter(QObject *target, QEvent *event) {
  if (target == valueField || target == nameField || target == descriptionField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Semicolon || keyEvent->key() == Qt::Key_Bar || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
	return true;
      }
    }
  }
  if (target == nameField || target == valueField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Comma) {
	return true;
      }
    }
  }  
  return QObject::eventFilter(target, event);
}
