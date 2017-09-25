/*
 Copyright 2017 Wouter Spekkink 
 Authors : Wouter Spekkink <wouter.spekkink@gmail.com> 
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
  This dialog can be used to create new relationships.

*/

#include "../include/RelationsDialog.h"

// Initialize some constants
const QString RelationsDialog::DEFAULT = "-Unselected-";
const QString RelationsDialog::UNDIRECTEDTAIL = " <--- ";
const QString RelationsDialog::DIRECTEDTAIL = "----";
const QString RelationsDialog::DIRECTEDHEAD = " ---> ";
const QString RelationsDialog::DIRECTED = "Directed";
const QString RelationsDialog::UNDIRECTED = "Undirected";
const QString RelationsDialog::EMPTY = "";

RelationsDialog::RelationsDialog(QWidget *parent, DataInterface *interface, const QString &label, Logger *subLogger) : QDialog(parent) {
  dataInterface = interface;
  logger = subLogger;
  // First we declare the entities of this dialog.
  headLabel = new QLabel(DIRECTEDHEAD);
  submittedLabel = label;
  currentLeftEntity = "";
  currentRightEntity = "";
  currentLeftEntityFilter = "";
  currentRightEntityFilter = "";
  currentRelType = "";
    
  if (submittedLabel == EMPTY) {
    selectedSourceLabel = new QLabel(DEFAULT);
    tailLabel = new QLabel(UNDIRECTEDTAIL);
    selectedTypeLabel = new QLabel(DEFAULT);
    selectedTargetLabel = new QLabel(DEFAULT);
    permanentLabel = "";
    currentLeftEntitySelected = "";
    currentRightEntitySelected = "";
    currentRelTypeSelected = "";
    currentDirectedness = DIRECTED;

  } else {
    permanentLabel = submittedLabel;
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->relationships.begin(); it != dataInterface->relationships.end(); it++) {
      std::vector<std::string> currentRelationship = *it;
      if (currentRelationship[0] == submittedLabel.toStdString()) {
	selectedSourceLabel = new QLabel(QString::fromStdString(currentRelationship[1]));
	currentLeftEntitySelected = QString::fromStdString(currentRelationship[1]);
	selectedTypeLabel = new QLabel(QString::fromStdString(currentRelationship[2]));
	currentRelTypeSelected = QString::fromStdString(currentRelationship[2]);
	selectedTargetLabel = new QLabel(QString::fromStdString(currentRelationship[3]));
	currentRightEntitySelected = QString::fromStdString(currentRelationship[3]);
	std::vector <std::vector <std::string> >::iterator it2;
	for (it2 = dataInterface->relationshipTypes.begin(); it2 != dataInterface->relationshipTypes.end(); it2++) {
	  std::vector<std::string> currentRelationshipType = *it2;
	  if (currentRelationshipType[0] == currentRelTypeSelected.toStdString()) {
	    if (currentRelationshipType[2] == DIRECTED.toStdString()) {
	      tailLabel = new QLabel(DIRECTEDTAIL);
	      currentDirectedness = DIRECTED;
	    } else if (currentRelationshipType[2] == UNDIRECTED.toStdString()) {
	      tailLabel = new QLabel(UNDIRECTEDTAIL);
	      currentDirectedness = UNDIRECTED;
	    }
	  }
	}
      }
    }    
  }
  
  informativeLabel = new QLabel(tr("<h3>Define the relationship</h3>"));
  sourceLabel = new QLabel(tr("Source filter:"));
  typeLabel = new QLabel(tr("Type filter:"));
  targetLabel = new QLabel(tr("Target filter:"));
  sourceListWidget = new QListWidget();
  sourceListWidget->setSortingEnabled(true);
  typeListWidget = new QListWidget();
  typeListWidget->setSortingEnabled(true);
  targetListWidget = new QListWidget();
  targetListWidget->setSortingEnabled(true);
  assignLeftEntityButton = new QPushButton("use selected");
  assignLeftEntityButton->setEnabled(false);
  assignTypeButton = new QPushButton("use selected");
  assignTypeButton->setEnabled(false);
  assignRightEntityButton = new QPushButton("use selected");
  assignRightEntityButton->setEnabled(false);
  
  leftEntityFilterField = new QLineEdit();
  rightEntityFilterField = new QLineEdit();
  newEntityButton = new QPushButton(tr("Define new entity"));
  editEntityButton = new QPushButton(tr("Edit highlighted entity"));
  editLeftAssignedEntityButton = new QPushButton(tr("Edit left assigned entity"));
  editRightAssignedEntityButton = new QPushButton(tr("Edit right assigned entity"));
  removeEntitiesButton = new QPushButton(tr("Remove unused entities"));

  typesFilterField = new QLineEdit();
  newRelationshipButton = new QPushButton(tr("Define new relationship type"));
  editRelationshipButton = new QPushButton(tr("Edit highlighted relationship type"));
  editAssignedRelationshipButton = new QPushButton(tr("Edit assigned relationship type"));
  removeRelationshipsButton = new QPushButton(tr("Remove unused relationship types"));

  cancelCloseButton = new QPushButton(tr("Cancel"));
  resetButton = new QPushButton(tr("Reset"));
  saveCloseButton = new QPushButton(tr("Save relationship"));
  
  // Then we wire the signals.

  connect(sourceListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setCurrentEntityLeft(QListWidgetItem*)));
  connect(typeListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setCurrentRelType(QListWidgetItem*)));
  connect(targetListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setCurrentEntityRight(QListWidgetItem*)));
  connect(sourceListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editEntity()));
  connect(typeListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editRelType()));
  connect(targetListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editEntity()));
  connect(leftEntityFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterLeftEntity(const QString &)));
  connect(typesFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterTypes(const QString &)));
  connect(rightEntityFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterRightEntity(const QString &)));
  connect(assignLeftEntityButton, SIGNAL(clicked()), this, SLOT(assignLeftEntity()));
  connect(assignTypeButton, SIGNAL(clicked()), this, SLOT(assignRelType()));
  connect(assignRightEntityButton, SIGNAL(clicked()), this, SLOT(assignRightEntity()));
  connect(newEntityButton, SIGNAL(clicked()), this, SLOT(addEntity()));
  connect(editEntityButton, SIGNAL(clicked()), this, SLOT(editEntity()));
  connect(editLeftAssignedEntityButton, SIGNAL(clicked()), this, SLOT(editLeftAssignedEntity()));
  connect(editRightAssignedEntityButton, SIGNAL(clicked()), this, SLOT(editRightAssignedEntity()));
  connect(removeEntitiesButton, SIGNAL(clicked()), this, SLOT(removeEntities()));
  connect(newRelationshipButton, SIGNAL(clicked()), this, SLOT(addRelType()));
  connect(editRelationshipButton, SIGNAL(clicked()), this, SLOT(editRelType()));
  connect(editAssignedRelationshipButton, SIGNAL(clicked()), this, SLOT(editAssignedRelType()));
  connect(removeRelationshipsButton, SIGNAL(clicked()), this, SLOT(removeRelTypes()));
  connect(cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelAndClose()));
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
  
  // Then we create the layout.
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  mainLayout->addWidget(informativeLabel);
  informativeLabel->setAlignment(Qt::AlignHCenter);

  QPointer<QHBoxLayout> labelLayout = new QHBoxLayout;
  labelLayout->addWidget(sourceLabel);
  labelLayout->addWidget(leftEntityFilterField);
  labelLayout->addWidget(typeLabel);
  labelLayout->addWidget(typesFilterField);
  labelLayout->addWidget(targetLabel);
  labelLayout->addWidget(rightEntityFilterField);
  mainLayout->addLayout(labelLayout);
  QPointer<QHBoxLayout> listsLayout = new QHBoxLayout;
  listsLayout->addWidget(sourceListWidget);
  listsLayout->addWidget(typeListWidget);
  listsLayout->addWidget(targetListWidget);
  mainLayout->addLayout(listsLayout);
  QPointer<QHBoxLayout> assignLayout = new QHBoxLayout;
  assignLayout->addWidget(assignLeftEntityButton);
  assignLayout->addWidget(assignTypeButton);
  assignLayout->addWidget(assignRightEntityButton);
  mainLayout->addLayout(assignLayout);
  QPointer<QHBoxLayout> arrowLayout = new QHBoxLayout;
  arrowLayout->addWidget(selectedSourceLabel);
  selectedSourceLabel->setAlignment(Qt::AlignHCenter);
  arrowLayout->addWidget(tailLabel);
  tailLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  arrowLayout->addWidget(selectedTypeLabel);
  selectedTypeLabel->setAlignment(Qt::AlignHCenter);
  arrowLayout->addWidget(headLabel);
  headLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  arrowLayout->addWidget(selectedTargetLabel);
  selectedTargetLabel->setAlignment(Qt::AlignHCenter);
  mainLayout->addLayout(arrowLayout);

  QPointer<QFrame> topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(topLine);

  QPointer<QHBoxLayout> buttonsLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> buttonsLeftLayout = new QVBoxLayout;
  buttonsLeftLayout->addWidget(newEntityButton);
  buttonsLeftLayout->addWidget(editEntityButton);
  buttonsLeftLayout->addWidget(editLeftAssignedEntityButton);
  buttonsLeftLayout->addWidget(editRightAssignedEntityButton);
  buttonsLeftLayout->addWidget(removeEntitiesButton);
  buttonsLeftLayout->setContentsMargins(40,10,20,10);
  buttonsLayout->addLayout(buttonsLeftLayout);
  QPointer<QVBoxLayout> buttonsRightLayout = new QVBoxLayout;
  buttonsRightLayout->addWidget(newRelationshipButton);
  buttonsRightLayout->addWidget(editRelationshipButton);
  buttonsRightLayout->addWidget(editAssignedRelationshipButton);
  buttonsRightLayout->addWidget(removeRelationshipsButton);
  buttonsRightLayout->setContentsMargins(20,10,40,10);
  buttonsRightLayout->setAlignment(Qt::AlignVCenter);
  buttonsLayout->addLayout(buttonsRightLayout);
  mainLayout->addLayout(buttonsLayout);
  
  QPointer<QFrame> bottomLine = new QFrame();
  bottomLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(bottomLine);

  QPointer<QHBoxLayout> optionsLayout = new QHBoxLayout;
  optionsLayout->addWidget(cancelCloseButton);
  optionsLayout->addWidget(resetButton);
  optionsLayout->addWidget(saveCloseButton);
  optionsLayout->setSpacing(300);
  mainLayout->addLayout(optionsLayout);

  setLayout(mainLayout);
  setWindowTitle("Add / Edit Relationship");
  resize(600, 100);

  updateTexts();
  // And that finishes this constructor.
}

void RelationsDialog::setCurrentEntityLeft(QListWidgetItem *item) {
  currentLeftEntity = item->text();
  lastSelectedEntity = item->text();
  assignLeftEntityButton->setEnabled(true);
  assignTypeButton->setEnabled(false);
  assignRightEntityButton->setEnabled(false);
}

void RelationsDialog::setCurrentRelType(QListWidgetItem *item) {
  currentRelType = item->text();
  assignLeftEntityButton->setEnabled(false);
  assignTypeButton->setEnabled(true);
  assignRightEntityButton->setEnabled(false);
}

void RelationsDialog::setCurrentEntityRight(QListWidgetItem *item) {
  currentRightEntity = item->text();
  lastSelectedEntity = item->text();
  assignLeftEntityButton->setEnabled(false);
  assignTypeButton->setEnabled(false);
  assignRightEntityButton->setEnabled(true);
}

void RelationsDialog::assignLeftEntity() {
  if (currentLeftEntity != "") {
    currentLeftEntitySelected = currentLeftEntity;
    selectedSourceLabel->setText(currentLeftEntitySelected);
    updateTexts();
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Assigned entity " + currentLeftEntity + " as source.";
    logger->addToLog(newLog);
  }
  disableAssign();
}

void RelationsDialog::assignRelType() {
  if (currentRelType != "") {
    currentRelTypeSelected = currentRelType;
    selectedTypeLabel->setText(currentRelTypeSelected);
    updateTexts();
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Assigned relationship type " + currentRelType + " as relationship type.";
    logger->addToLog(newLog);
  }
  disableAssign();
}

void RelationsDialog::assignRightEntity() {
  if (currentRightEntity != "") {
    currentRightEntitySelected = currentRightEntity;
    selectedTargetLabel->setText(currentRightEntitySelected);
    updateTexts();
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Assigned entity " + currentLeftEntity + " as target.";
    logger->addToLog(newLog);
  }
  disableAssign();
}

void RelationsDialog::filterLeftEntity(const QString &text) {
  disableAssign();
  currentLeftEntityFilter = text;
  updateTexts();
}

void RelationsDialog::filterTypes(const QString &text) {
  disableAssign();
  currentRelTypeFilter = text;
  updateTexts();
}

void RelationsDialog::filterRightEntity(const QString &text) {
  disableAssign();
  currentRightEntityFilter = text;
  updateTexts();
}

void RelationsDialog::addEntity() {
  disableAssign();
  EntityDialog *entityDialog = new EntityDialog(this, dataInterface, EMPTY, logger);
  entityDialog->exec();
  delete entityDialog;
  updateTexts();
}

void RelationsDialog::editEntity() {
  if (lastSelectedEntity != "") {
    EntityDialog *entityDialog = new EntityDialog(this, dataInterface, lastSelectedEntity, logger);
    disableAssign();
    entityDialog->exec();
    delete entityDialog;
    updateTexts();
  }
}

void RelationsDialog::editLeftAssignedEntity() {
  if (currentLeftEntitySelected != DEFAULT && currentLeftEntitySelected != EMPTY) {
    EntityDialog *entityDialog = new EntityDialog(this, dataInterface, currentLeftEntitySelected, logger);
    disableAssign();
    entityDialog->exec();
    delete entityDialog;
    updateTexts();
  }
}

void RelationsDialog::editRightAssignedEntity() {
  if (currentRightEntitySelected != DEFAULT && currentRightEntitySelected != EMPTY) {
    EntityDialog *entityDialog = new EntityDialog(this, dataInterface, currentRightEntitySelected, logger);
    disableAssign();
    entityDialog->exec();
    delete entityDialog;
    updateTexts();
  }
}

void RelationsDialog::removeEntities() {
  disableAssign();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->entities.begin(); it != dataInterface->entities.end();) {
    std::vector<std::string> currentEntity = *it;
    bool found = false;
    for (it2 = dataInterface->relationships.begin(); it2 != dataInterface->relationships.end(); it2++) {
      std::vector<std::string> currentRelationship = *it2;
      if (currentEntity[0] == currentRelationship[1] || currentEntity[0] == currentRelationship[3]) {
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
      for (it3 = dataInterface->assignedEntityAttributes.begin();
	   it3 != dataInterface->assignedEntityAttributes.end();) {
	std::vector<std::string>::iterator it4;
	for (it4 = it3->begin() + 1; it4 != it3->end();) {
	  if (*it4 == currentEntity[0]) {
	    it3->erase(it4);
	  } else {
	    it4++;
	  } 
	}
	if (it3->size() < 2) {
	  std::vector <std::vector <std::string> >::iterator it5;
	  for (it5 = dataInterface->assignedEntityAttributeCategories.begin();
	       it5 != dataInterface->assignedEntityAttributeCategories.end();) {
	    std::vector<std::string>::iterator it6;
	    for (it6 = it5->begin() + 1; it6 != it5->end();) {
	      if (*it6 == *it3->begin()) {
		it5->erase(it6);
	      } else {
		it6++;
	      }
	    }
	    if (it5->size() < 2) {
	      dataInterface->assignedEntityAttributeCategories.erase(it5);
	    } else {
	      it5++;
	    }
	  }
	  dataInterface->assignedEntityAttributes.erase(it3);	    
	} else {
	  it3++;
	}
      }
      dataInterface->entities.erase(it);
    } else {
      it++;
    }
  }
  if (removed.size() > 0) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Removed unused entities " + removed;
    logger->addToLog(newLog);
  }
  updateTexts();
}

void RelationsDialog::addRelType() {
  disableAssign();
  RelTypeDialog *relTypeDialog = new RelTypeDialog(this, dataInterface, EMPTY, logger);
  disableAssign();
  relTypeDialog->exec();
  delete relTypeDialog;
  updateTexts();
}

void RelationsDialog::editRelType() {
  if (currentRelType != "") {
    RelTypeDialog *relTypeDialog = new RelTypeDialog(this, dataInterface, currentRelType, logger);
    disableAssign();
    relTypeDialog->exec();
    delete relTypeDialog;
    updateTexts();
  }
}

void RelationsDialog::editAssignedRelType() {
  if (currentRelTypeSelected != DEFAULT && currentRelTypeSelected != EMPTY) {
    RelTypeDialog *relTypeDialog = new RelTypeDialog(this, dataInterface, currentRelTypeSelected, logger);
    disableAssign();
    relTypeDialog->exec();
    delete relTypeDialog;
    updateTexts();
  }
}

void RelationsDialog::removeRelTypes() {
  disableAssign();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->relationshipTypes.begin(); it != dataInterface->relationshipTypes.end();) {
    std::vector<std::string> currentType = *it;
    bool found = false;
    for (it2 = dataInterface->relationships.begin(); it2 != dataInterface->relationships.end(); it2++) {
      std::vector<std::string> currentRelationship = *it2;
      if (currentType[0] == currentRelationship[2]) {
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
      dataInterface->relationshipTypes.erase(it);
    } else {
      it++;
    }
  }
  if (removed.size() > 0) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Removed unused relationship types " + removed;
    logger->addToLog(newLog);
  }
  updateTexts();
}

void RelationsDialog::disableAssign() {
  lastSelectedEntity = "";
  currentRelType = "";
  assignLeftEntityButton->setEnabled(false);
  assignRightEntityButton->setEnabled(false);
  assignTypeButton->setEnabled(false);
}

void RelationsDialog::updateTexts() {
  sourceListWidget->clear();
  typeListWidget->clear();
  targetListWidget->clear();
  std::vector <std::vector <std::string> >::iterator sIt;
  for (sIt = dataInterface->entities.begin(); sIt != dataInterface->entities.end(); sIt++) {
    std::vector<std::string> entity= *sIt;
    if (entity[0] != currentLeftEntitySelected.toStdString() && entity[0] != currentRightEntitySelected.toStdString()) {
      if (currentLeftEntityFilter != "") {
	std::size_t found = entity[0].find(currentLeftEntityFilter.toStdString());
	if (found != std::string::npos) {
	  sourceListWidget->addItem(QString::fromStdString(entity[0]));
	}
      } else {
	sourceListWidget->addItem(QString::fromStdString(entity[0]));
      }
      if (currentRightEntityFilter != "") {
	std::size_t found = entity[0].find(currentRightEntityFilter.toStdString());
	if (found != std::string::npos) {
	  targetListWidget->addItem(QString::fromStdString(entity[0]));
	}
      } else {
	targetListWidget->addItem(QString::fromStdString(entity[0]));
      }
    }
    for (int i = 0; i != sourceListWidget->count(); i++) {
      std::string currentItem = sourceListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->entities.begin(); it != dataInterface->entities.end(); it++) {
	std::vector<std::string> currentEntity = *it;
	if (currentEntity[0] == currentItem) {
	  sourceListWidget->item(i)->setToolTip(QString::fromStdString(currentEntity[1]));
	}
      }
    }
    for (int i = 0; i != targetListWidget->count(); i++) {
      std::string currentItem = targetListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->entities.begin(); it != dataInterface->entities.end(); it++) {
	std::vector<std::string> currentEntity = *it;
	if (currentEntity[0] == currentItem) {
	  targetListWidget->item(i)->setToolTip(QString::fromStdString(currentEntity[1]));
	}
      }
    }
  }
  std::vector <std::vector <std::string> >::iterator sIt2;
  for (sIt2 = dataInterface->relationshipTypes.begin(); sIt2 != dataInterface->relationshipTypes.end(); sIt2++) {
    std::vector<std::string> relationshipType = *sIt2;
    if (relationshipType[0] != currentRelTypeSelected.toStdString()) {
      if (currentRelTypeFilter != "") {
	std::size_t found = relationshipType[0].find(currentRelTypeFilter.toStdString());
	if (found != std::string::npos) {
	  typeListWidget->addItem(QString::fromStdString(relationshipType[0]));
	}
      } else {
	typeListWidget->addItem(QString::fromStdString(relationshipType[0]));
      }
    }
    for (int i = 0; i != typeListWidget->count(); i++) {
      std::string currentItem = typeListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->relationshipTypes.begin(); it != dataInterface->relationshipTypes.end(); it++) {
	std::vector<std::string> currentRelationshipType = *it;
	if (currentRelationshipType[0] == currentItem) {
	  typeListWidget->item(i)->setToolTip(QString::fromStdString(currentRelationshipType[1]));
	}
      }
    }
  }
  std::vector <std::vector <std::string> >::iterator sIt3;
  for (sIt3 = dataInterface->relationshipTypes.begin(); sIt3 != dataInterface->relationshipTypes.end(); sIt3++) {
    std::vector<std::string> currentRelationshipType = *sIt3;
    if (currentRelationshipType[0] == currentRelTypeSelected.toStdString()) {
      if (currentRelationshipType[2] == DIRECTED.toStdString()) {
	tailLabel->setText(DIRECTEDTAIL);
	currentDirectedness = DIRECTED;
      } else if (currentRelationshipType[2] == UNDIRECTED.toStdString()) {
	tailLabel->setText(UNDIRECTEDTAIL);
	currentDirectedness = UNDIRECTED;
      }
    }
  }
}

void RelationsDialog::reset() {
  currentLeftEntity = "";
  currentRightEntity = "";
  currentLeftEntitySelected = "";
  currentRightEntitySelected = "";
  currentRelType = "";
  selectedSourceLabel->setText(DEFAULT);
  selectedTargetLabel->setText(DEFAULT);
  selectedTypeLabel->setText(DEFAULT);
  headLabel->setText(DIRECTED);
  disableAssign();
  updateTexts();
}

void RelationsDialog::cancelAndClose() {
  disableAssign();
  if (submittedLabel == EMPTY) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new relationship";
    logger->addToLog(newLog);
  } else {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing relationship " + submittedLabel;
    logger->addToLog(newLog);
  }
  this->close();
}

void RelationsDialog::saveAndClose() {
  disableAssign();
  if (selectedSourceLabel->text() != DEFAULT && selectedTypeLabel->text() != DEFAULT && selectedTargetLabel->text() != DEFAULT) {
    std::vector<std::string> tempRelationship;
    bool createNew = true;
    std::string tail = "";
    std::string head = "]-->";
    if (currentDirectedness == DIRECTED) {
      tail = "---[";
    } else if (currentDirectedness == UNDIRECTED) {
      tail = "<--[";
    }
    std::string label = selectedSourceLabel->text().toStdString() + tail +
      selectedTypeLabel->text().toStdString() + head + selectedTargetLabel->text().toStdString();
    std::string altLabel = selectedTargetLabel->text().toStdString() + tail +
      selectedTypeLabel->text().toStdString() + head + selectedSourceLabel->text().toStdString();

    tempRelationship.push_back(label);
    tempRelationship.push_back(selectedSourceLabel->text().toStdString());
    tempRelationship.push_back(selectedTypeLabel->text().toStdString());
    tempRelationship.push_back(selectedTargetLabel->text().toStdString());
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationships.size(); i++) {
      if (dataInterface->relationships[i][0] == label && dataInterface->relationships[i][0] != permanentLabel.toStdString()) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship already exists.");
	warningBox->setInformativeText("You cannot create two identical relationships.");
	warningBox->exec();
	return;
      }
      if (dataInterface->relationships[i][0] == altLabel && currentDirectedness == UNDIRECTED) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship already exists.");
	warningBox->setInformativeText("You cannot create two identical relationships.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationships.size(); i++) {
      if (dataInterface->relationships[i][0] == label && dataInterface->relationships[i][0] != permanentLabel.toStdString()) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship already exists.");
	warningBox->setInformativeText("You cannot create two identical relationships.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationships.size(); i++) {
      if (dataInterface->relationships[i][0] == permanentLabel.toStdString() && submittedLabel == EMPTY) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship already exists.");
	warningBox->setInformativeText("You cannot create two identical relationships.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationships.size(); i++) {
      if (dataInterface->relationships[i][0] == permanentLabel.toStdString() && submittedLabel != EMPTY) {
	dataInterface->relationships[i][0] = label;
	dataInterface->relationships[i][1] = selectedSourceLabel->text().toStdString();
	dataInterface->relationships[i][2] = selectedTypeLabel->text().toStdString();
	dataInterface->relationships[i][3] = selectedTargetLabel->text().toStdString();
 	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Edited relationship " + submittedLabel +
	  ", which is now saved as " + QString::fromStdString(label);
	logger->addToLog(newLog);
	createNew = false;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedRelationships.size(); i++) {
      if (dataInterface->assignedRelationships[i][0] == permanentLabel.toStdString() && submittedLabel != EMPTY) {
	dataInterface->assignedRelationships[i][0] = label;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationMemos.size(); i++) {
      if (dataInterface->relationMemos[i][0] == permanentLabel.toStdString() && submittedLabel  != EMPTY) {
	dataInterface->relationMemos[i][0] = label;
      }
    }
    if (createNew) {
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Created new relationship " + QString::fromStdString(label);
      logger->addToLog(newLog);
      dataInterface->relationships.push_back(tempRelationship);
      this->close();
    } else {
      this->close();
    }
  } else if (selectedSourceLabel->text() == DEFAULT || selectedTypeLabel->text() == DEFAULT || selectedTargetLabel->text() == DEFAULT) {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("Relationship incomplete.");
    warningBox->setInformativeText("You need to select a source, type and target for the relationship");
    warningBox->exec();
    return;
  }
}
