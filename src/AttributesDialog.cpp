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
  This dialog can be used to create new event attributes.

*/

#include "../include/AttributesDialog.h"

// Define some constants
const QString AttributesDialog::INCIDENT = "Incident";
const QString AttributesDialog::ENTITY = "Entity";
const QString AttributesDialog::EMPTY = "";

// The constructor for this class
AttributesDialog::AttributesDialog(QWidget *parent, DataInterface *interface,
				   const QString &label, const QString &type, Logger *subLogger) : QDialog(parent) {
  submittedType = type;
  currentCategory = "";
  currentCategoryFilter = "";
  dataInterface = interface;
  logger = subLogger;
  // We default some of our variables;
  if (submittedType == INCIDENT) {
    submittedLabel = label;
    if (submittedLabel == EMPTY) {
      name = "";
      permanentName = ";;NEW;;";
      description = "";
    } else {
      name = submittedLabel.toStdString();
      permanentName = name;
      std::vector <std::vector <std::string> >::iterator sIt;
      for (sIt = dataInterface->incidentAttributes.begin(); sIt != dataInterface->incidentAttributes.end(); sIt++) {
	std::vector<std::string> currentAttribute = *sIt;
	if (currentAttribute[0] == name) {
	  description = currentAttribute[1];
	  break;
	}
      }
    }
  } else if (submittedType == ENTITY) {
    submittedLabel = label;
    if (submittedLabel == EMPTY) {
      name = "";
      permanentName = ";;NEW;;";
      description = "";
    } else {
      name = submittedLabel.toStdString();
      permanentName = name;
      std::vector <std::vector <std::string> >::iterator sIt;
      for (sIt = dataInterface->entityAttributes.begin(); sIt != dataInterface->entityAttributes.end(); sIt++) {
	std::vector<std::string> currentAttribute = *sIt;
	if (currentAttribute[0] == name) {
	  description = currentAttribute[1];
	}
      }
    }
  }
  // We declare the entities of this dialog.
  informativeLabel = new QLabel(tr("Define the attribute:"));
  nameLabel = new QLabel(tr("Label:"));
  nameField = new QLineEdit();
  descriptionLabel = new QLabel(tr("Description:"));
  descriptionField = new QPlainTextEdit();

  nameField->setText(QString::fromStdString(name));
  descriptionField->setPlainText(QString::fromStdString(description));
  nameField->installEventFilter(this);
  descriptionField->installEventFilter(this);
   
  attributeCategoriesLabel = new QLabel(tr("Categories:"));
  categoriesPoolLabel = new QLabel(tr("Category pool:"));
  assignCategoryButton = new QPushButton("<<");
  unassignCategoryButton = new QPushButton(">>");
  assignedCategoriesListWidget = new QListWidget();
  assignedCategoriesListWidget->setSortingEnabled(true);
  unassignedCategoriesListWidget = new QListWidget();
  unassignedCategoriesListWidget->setSortingEnabled(true);
  categoriesFilterLabel = new QLabel(tr("Filter:"));
  categoriesFilterField = new QLineEdit();
  addCategoryButton = new QPushButton(tr("Add new Category"));
  editCategoryButton = new QPushButton(tr("Edit highlighted category"));
  removeCategoriesButton = new QPushButton(tr("Remove unused categories"));

  cancelCloseButton = new QPushButton(tr("Cancel"));
  saveCloseButton = new QPushButton(tr("Save attribute"));
  
  // Then we connect the signals.
  connect(nameField, SIGNAL(textChanged(const QString &)), SLOT(setName(const QString &)));
  connect(assignedCategoriesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(setCurrentCategory(QListWidgetItem*)));
  connect(unassignedCategoriesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(setCurrentCategory(QListWidgetItem*)));
  connect(categoriesFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterCategories(const QString &)));
  connect(assignCategoryButton, SIGNAL(clicked()), this, SLOT(assignCategory()));
  connect(unassignCategoryButton, SIGNAL(clicked()), this, SLOT(unassignCategory()));
  if (submittedType == INCIDENT) {
    connect(addCategoryButton, SIGNAL(clicked()), this, SLOT(addIncidentCategory()));
    connect(editCategoryButton, SIGNAL(clicked()), this, SLOT(editIncidentCategory()));
    connect(removeCategoriesButton, SIGNAL(clicked()), this, SLOT(removeIncidentCategories()));
    connect(assignedCategoriesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editIncidentCategory()));
    connect(unassignedCategoriesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editIncidentCategory()));
  } else if (submittedType == ENTITY) {
    connect(addCategoryButton, SIGNAL(clicked()), this, SLOT(addEntityCategory()));
    connect(editCategoryButton, SIGNAL(clicked()), this, SLOT(editEntityCategory()));
    connect(removeCategoriesButton, SIGNAL(clicked()), this, SLOT(removeEntityCategories()));
    connect(assignedCategoriesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editEntityCategory()));
    connect(unassignedCategoriesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editEntityCategory()));
  }

  connect(cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelAndClose()));
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
  
  // Then we create the layout for the dialog.
  setWindowTitle("Add / Edit Attribute");
  //  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);


  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  mainLayout->addWidget(informativeLabel);
  informativeLabel->setAlignment(Qt::AlignHCenter);
  
  QPointer<QHBoxLayout> topLayout = new QHBoxLayout;

  QPointer<QVBoxLayout> topLeftLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> labelLayout = new QHBoxLayout;
  labelLayout->addWidget(nameLabel);
  labelLayout->addWidget(nameField);
  topLeftLayout->addLayout(labelLayout);
  topLeftLayout->addWidget(descriptionLabel);
  topLeftLayout->addWidget(descriptionField);
  topLayout->addLayout(topLeftLayout);

  QPointer<QHBoxLayout> topRightLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> assignedLayout = new QVBoxLayout;
  assignedLayout->addWidget(attributeCategoriesLabel);
  attributeCategoriesLabel->setAlignment(Qt::AlignHCenter);
  assignedLayout->addWidget(assignedCategoriesListWidget);
  topRightLayout->addLayout(assignedLayout);
  QPointer<QVBoxLayout> leftButtonLayout = new QVBoxLayout;
  leftButtonLayout->addWidget(assignCategoryButton);
  leftButtonLayout->addWidget(unassignCategoryButton);
  leftButtonLayout->setAlignment(Qt::AlignVCenter);
  topRightLayout->addLayout(leftButtonLayout);
  QPointer<QVBoxLayout> unassignedLayout = new QVBoxLayout;
  unassignedLayout->addWidget(categoriesPoolLabel);
  categoriesPoolLabel->setAlignment(Qt::AlignHCenter);
  unassignedLayout->addWidget(unassignedCategoriesListWidget);
  topRightLayout->addLayout(unassignedLayout);
  QPointer<QVBoxLayout> rightButtonLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> filterLayout = new QHBoxLayout;
  filterLayout->addWidget(categoriesFilterLabel);
  filterLayout->addWidget(categoriesFilterField);
  rightButtonLayout->addLayout(filterLayout);
  rightButtonLayout->addWidget(addCategoryButton);
  rightButtonLayout->addWidget(editCategoryButton);
  rightButtonLayout->addWidget(removeCategoriesButton);
  rightButtonLayout->setAlignment(Qt::AlignVCenter);
  topRightLayout->addLayout(rightButtonLayout);
  topLayout->addLayout(topRightLayout);
  mainLayout->addLayout(topLayout);

  QPointer<QFrame> middleLine = new QFrame();
  middleLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(middleLine);

  QPointer<QHBoxLayout> optionLayout = new QHBoxLayout;
  optionLayout->addWidget(cancelCloseButton);
  optionLayout->addWidget(saveCloseButton);
  optionLayout->setSpacing(600);
  mainLayout->addLayout(optionLayout);

  setLayout(mainLayout);
  setFixedHeight(sizeHint().height());

  // Now we need to fetch some text as well.
  updateTexts();
  // This finishes the constructor.
}

void AttributesDialog::setName(const QString &newName) {
  disableCategorySelection();
  name = newName.toStdString();
}

void AttributesDialog::setCurrentCategory(QListWidgetItem *item) {
  currentCategory = item->text();
}

void AttributesDialog::assignCategory() {
  if (currentCategory != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    bool foundCategory = false;
    bool foundAttribute = false;
    std::vector <std::vector <std::string> >::size_type categoryIndex;
    if (submittedType == INCIDENT) {
      for (std::vector <std::vector <std::string> >::size_type i = 0;
	   i != dataInterface->assignedIncidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributeCategories[i];
	if (currentGroup[0] == currentCategory.toStdString()) {
	  foundCategory = true;
	  categoryIndex = i;
	  std::vector<std::string>::iterator pIt;
	  for (pIt = currentGroup.begin() + 1; pIt != currentGroup.end(); pIt++) {
	    if (*pIt == permanentName) {
	      foundAttribute = true;
	    }
	  }
	}
      }
      if (!foundCategory) {
	std::vector <std::string> tempPair;
	tempPair.push_back(currentCategory.toStdString());
	tempPair.push_back(permanentName); // SHOULD ALSO CHANGE THIS IF THE NAME IS EDITED.
	dataInterface->assignedIncidentAttributeCategories.push_back(tempPair);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Assigned category " + currentCategory + " to incident attribute "+
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      } else if (foundCategory && !foundAttribute) {
	dataInterface->assignedIncidentAttributeCategories[categoryIndex].push_back(permanentName);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Assigned category " + currentCategory + " to incident attribute " +
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      }
      currentCategory = "";
      updateTexts();
    } else if (submittedType == ENTITY) {
      for (std::vector <std::vector <std::string> >::size_type i = 0;
	   i != dataInterface->assignedEntityAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributeCategories[i];
	if (currentGroup[0] == currentCategory.toStdString()) {
	  foundCategory = true;
	  categoryIndex = i;
	  std::vector<std::string>::iterator pIt;
	  for (pIt = currentGroup.begin() + 1; pIt != currentGroup.end(); pIt++) {
	    if (*pIt == permanentName) {
	      foundAttribute = true;
	    }
	  }
	}
      }
      if (!foundCategory) {
	std::vector <std::string> tempPair;
	tempPair.push_back(currentCategory.toStdString());
	tempPair.push_back(permanentName); // SHOULD ALSO CHANGE THIS IF THE NAME IS EDITED.
	dataInterface->assignedEntityAttributeCategories.push_back(tempPair);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Assigned category " + currentCategory + " to entity attribute " +
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      } else if (foundCategory && !foundAttribute) {
	dataInterface->assignedEntityAttributeCategories[categoryIndex].push_back(permanentName);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Assigned category " + currentCategory + " to entity attribute " +
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      }
      currentCategory = "";
      updateTexts();
    }
  }
}

void AttributesDialog::unassignCategory() {
  if (currentCategory != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    std::vector<std::string>::size_type eraseIndex = 0;
    bool foundCategory = false;
    bool foundAttribute = false;
    std::vector <std::vector <std::string> >::size_type categoryIndex;
    if (submittedType == INCIDENT) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedIncidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributeCategories[i];
	if (currentGroup[0] == currentCategory.toStdString()) {
	  foundCategory = true;
	  categoryIndex = i;
	  for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	    if (currentGroup[j] == permanentName) {
	      foundAttribute = true;
	      eraseIndex = j;
	    }
	  }
	}
      }
      if (foundCategory && foundAttribute) {
	dataInterface->assignedIncidentAttributeCategories[categoryIndex].erase(dataInterface->
										assignedIncidentAttributeCategories[categoryIndex].begin() +
										eraseIndex);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Unassigned category " + currentCategory + " from incident attribute " +
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      }
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->assignedIncidentAttributeCategories.begin();
	   it != dataInterface->assignedIncidentAttributeCategories.end();) {
	std::vector<std::string> currentAssigned = *it;
	if (currentAssigned.size() < 2) {
	  dataInterface->assignedIncidentAttributeCategories.erase(it);
	} else {
	  it++;
	}
      }
    } else if (submittedType == ENTITY) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedEntityAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributeCategories[i];
	if (currentGroup[0] == currentCategory.toStdString()) {
	  foundCategory = true;
	  categoryIndex = i;
	  for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	    if (currentGroup[j] == permanentName) {
	      foundAttribute = true;
	      eraseIndex = j;
	    }
	  }
	}
      }
      if (foundCategory && foundAttribute) {
	dataInterface->assignedEntityAttributeCategories[categoryIndex].erase(dataInterface->
									      assignedEntityAttributeCategories[categoryIndex].begin() +
									      eraseIndex);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Unassigned category " + currentCategory + " from entity attribute " +
	  QString::fromStdString(permanentName);
	logger->addToLog(newLog);
      
	std::vector <std::vector <std::string> >::iterator it;
	for (it = dataInterface->assignedEntityAttributeCategories.begin();
	     it != dataInterface->assignedEntityAttributeCategories.end();) {
	  std::vector<std::string> currentAssigned = *it;
	  if (currentAssigned.size() < 2) {
	    dataInterface->assignedEntityAttributeCategories.erase(it);
	  } else {
	    it++;
	  }
	}
      }
    }
    updateTexts();
  }
}

void AttributesDialog::filterCategories(const QString &text) {
  disableCategorySelection();
  currentCategoryFilter = text;
  updateTexts();
}

void AttributesDialog::addIncidentCategory() {
  disableCategorySelection();
  CategoryDialog *categoryDialog = new CategoryDialog(this, dataInterface, EMPTY, INCIDENT, logger);
  categoryDialog->exec();
  delete categoryDialog;
  currentCategory = "";
  updateTexts();
}

void AttributesDialog::addEntityCategory() {
  CategoryDialog *categoryDialog = new CategoryDialog(this, dataInterface, EMPTY, ENTITY, logger);
  disableCategorySelection();
  categoryDialog->exec();
  delete categoryDialog;
  currentCategory = "";
  updateTexts();
}

void AttributesDialog::editIncidentCategory() {
  if (currentCategory != EMPTY) {
    CategoryDialog *categoryDialog = new CategoryDialog(this, dataInterface, currentCategory, INCIDENT, logger);
    disableCategorySelection();
    categoryDialog->exec();
    delete categoryDialog;
    currentCategory = "";
    updateTexts();
  }
}

void AttributesDialog::editEntityCategory() {
  if (currentCategory != EMPTY) {
    CategoryDialog *categoryDialog = new CategoryDialog(this, dataInterface, currentCategory, ENTITY, logger);
    disableCategorySelection();
    categoryDialog->exec();
    delete categoryDialog;
    currentCategory = "";
    updateTexts();
  }
}

void AttributesDialog::removeIncidentCategories() {
  disableCategorySelection();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->incidentAttributeCategories.begin(); it != dataInterface->incidentAttributeCategories.end();) {
    std::vector<std::string> currentCategory = *it;
    bool found = false;
    for (it2 = dataInterface->assignedIncidentAttributeCategories.begin();
	 it2 != dataInterface->assignedIncidentAttributeCategories.end(); it2++) {
      std::vector<std::string> currentAssigned = *it2;
      if (currentAssigned[0] == currentCategory[0]) {
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
      dataInterface->incidentAttributeCategories.erase(it);
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

void AttributesDialog::removeEntityCategories() {
  disableCategorySelection();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->entityAttributeCategories.begin(); it != dataInterface->entityAttributeCategories.end();) {
    std::vector<std::string> currentCategory = *it;
    bool found = false;
    for (it2 = dataInterface->assignedEntityAttributeCategories.begin();
	 it2 != dataInterface->assignedEntityAttributeCategories.end(); it2++) {
      std::vector<std::string> currentAssigned = *it2;
      if (currentAssigned[0] == currentCategory[0]) {
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
      dataInterface->entityAttributeCategories.erase(it);
    } else {
      it++;
    }
  }
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "Removed unused categories " + removed;
  logger->addToLog(newLog);
  updateTexts();
}

void AttributesDialog::disableCategorySelection() {
  currentCategory = "";
}

void AttributesDialog::updateTexts() {
  assignedCategoriesListWidget->clear();
  unassignedCategoriesListWidget->clear();
  std::vector <std::vector <std::string> >::iterator sIt;
  std::vector <std::vector <std::string> >::iterator sIt2;
  if (submittedType == INCIDENT) {
    for (sIt = dataInterface->incidentAttributeCategories.begin(); sIt != dataInterface->incidentAttributeCategories.end(); sIt++) {
      bool found = false;
      std::vector<std::string> currentCat= *sIt;
      for (sIt2 = dataInterface->assignedIncidentAttributeCategories.begin();
	   sIt2 != dataInterface->assignedIncidentAttributeCategories.end(); sIt2++) {
	std::vector<std::string> currentAssigned = *sIt2;
	if (currentAssigned[0] == currentCat[0] && !found) {
	  for (std::vector<std::string>::size_type i = 1; i != currentAssigned.size(); i++) {
	    std::string currentAttribute = currentAssigned[i];
	    if (currentAttribute == permanentName) {
	      found = true;
	    }
	  }
	}
      }
      if (!found) {
	if (currentCategoryFilter != "") {
	  std::size_t found = currentCat[0].find(currentCategoryFilter.toStdString());
	  if (found!=std::string::npos) {
	    unassignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	  }
	} else {
	  unassignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	}
      } else {
	if (currentCategoryFilter != "") {
	  std::size_t found = currentCat[0].find(currentCategoryFilter.toStdString());
	  if (found!=std::string::npos) {
	    assignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	  }
	} else {
	  assignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));
	}
      }
    }
    for (int i = 0; i != assignedCategoriesListWidget->count(); i++) {
      std::string currentItem = assignedCategoriesListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->incidentAttributeCategories.begin(); it != dataInterface->incidentAttributeCategories.end(); it++) {
	std::vector<std::string> currentCategory = *it;
	if (currentCategory[0] == currentItem) {
	  assignedCategoriesListWidget->item(i)->setToolTip(QString::fromStdString(currentCategory[1]));
	}
      }
    }
    for (int i = 0; i != unassignedCategoriesListWidget->count(); i++) {
      std::string currentItem = unassignedCategoriesListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->incidentAttributeCategories.begin(); it != dataInterface->incidentAttributeCategories.end(); it++) {
	std::vector<std::string> currentCategory = *it;
	if (currentCategory[0] == currentItem) {
	  unassignedCategoriesListWidget->item(i)->setToolTip(QString::fromStdString(currentCategory[1]));
	}
      }
    }
  } else if (submittedType == ENTITY) {
    for (sIt = dataInterface->entityAttributeCategories.begin(); sIt != dataInterface->entityAttributeCategories.end(); sIt++) {
      bool found = false;
      std::vector<std::string> currentCat= *sIt;
      for (sIt2 = dataInterface->assignedEntityAttributeCategories.begin();
	   sIt2 != dataInterface->assignedEntityAttributeCategories.end(); sIt2++) {
	std::vector<std::string> currentAssigned = *sIt2;
	if (currentAssigned[0] == currentCat[0] && !found) {
	  for (std::vector<std::string>::size_type i = 1; i != currentAssigned.size(); i++) {
	    std::string currentAttribute = currentAssigned[i];
	    if (currentAttribute == permanentName) {
	      found = true;
	    }
	  }
	}
      }
      if (!found) {
	if (currentCategoryFilter != "") {
	  std::size_t found = currentCat[0].find(currentCategoryFilter.toStdString());
	  if (found!=std::string::npos) {
	    unassignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	  }
	} else {
	  unassignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	}
      } else {
	if (currentCategoryFilter != "") {
	  std::size_t found = currentCat[0].find(currentCategoryFilter.toStdString());
	  if (found!=std::string::npos) {
	    assignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));	
	  }
	} else {
	  assignedCategoriesListWidget->addItem(QString::fromStdString(currentCat[0]));
	}
      }
    }
    for (int i = 0; i != assignedCategoriesListWidget->count(); i++) {
      std::string currentItem = assignedCategoriesListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->entityAttributeCategories.begin(); it != dataInterface->entityAttributeCategories.end(); it++) {
	std::vector<std::string> currentCategory = *it;
	if (currentCategory[0] == currentItem) {
	  assignedCategoriesListWidget->item(i)->setToolTip(QString::fromStdString(currentCategory[1]));
	}
      }
    }
    for (int i = 0; i != unassignedCategoriesListWidget->count(); i++) {
      std::string currentItem = unassignedCategoriesListWidget->item(i)->text().toStdString();
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->entityAttributeCategories.begin(); it != dataInterface->entityAttributeCategories.end(); it++) {
	std::vector<std::string> currentCategory = *it;
	if (currentCategory[0] == currentItem) {
	  unassignedCategoriesListWidget->item(i)->setToolTip(QString::fromStdString(currentCategory[1]));
	}
      }
    }
  }
  currentCategory = "";
}


void AttributesDialog::cancelAndClose() {
  disableCategorySelection(); 
  if (submittedLabel == EMPTY) {
    std::vector <std::vector <std::string> >::iterator sIt;
    if (submittedType == INCIDENT) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedIncidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributeCategories[i];
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  if (currentGroup[j] == ";;NEW;;") {
	    dataInterface->assignedIncidentAttributeCategories[i].erase(dataInterface->assignedIncidentAttributeCategories[i].begin() + j);
	  }
	}
      }
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->assignedIncidentAttributeCategories.begin();
	   it != dataInterface->assignedIncidentAttributeCategories.end();) {
	std::vector<std::string> currentAssigned = *it;
	if (currentAssigned.size() < 2) {
	  dataInterface->assignedIncidentAttributeCategories.erase(it);
	} else {
	  it++;
	}
      }
    } else if (submittedType == ENTITY) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedEntityAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributeCategories[i];
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  if (currentGroup[j] == ";;NEW;:") {
	    dataInterface->assignedEntityAttributeCategories[i].erase(dataInterface->assignedEntityAttributeCategories[i].begin() + j);
	  }
	}
	std::vector <std::vector <std::string> >::iterator it;
	for (it = dataInterface->assignedEntityAttributeCategories.begin();
	     it != dataInterface->assignedEntityAttributeCategories.end();) {
	  std::vector<std::string> currentAssigned = *it;
	  if (currentAssigned.size() < 2) {
	    dataInterface->assignedEntityAttributeCategories.erase(it);
	  } else {
	    it++;
	  }
	}
      }
    }
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new attribute";
    logger->addToLog(newLog);
  } else {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing attribute " + submittedLabel;
    logger->addToLog(newLog);
  }
  this->close();
}


void AttributesDialog::saveAndClose() {
  disableCategorySelection();
  description = (descriptionField->toPlainText()).toStdString();
  bool createNew = true;
  name.erase(std::remove(name.begin(), name.end(), ';'), name.end());
  name.erase(std::remove(name.begin(), name.end(), '|'), name.end());
  name.erase(name.find_last_not_of(" \n\r\t")+1);
  description.erase(std::remove(description.begin(), description.end(), ';'), description.end());
  description.erase(std::remove(description.begin(), description.end(), '|'), description.end());
  description.erase(description.find_last_not_of(" \n\r\t")+1);
  if (name != "" && description != "") {
    std::vector<std::string> tempAttribute;
    tempAttribute.push_back(name);
    tempAttribute.push_back(description);
    // I had to split these loops, because for some reason if I combine them into one, sometimes a bug occurs.
    if (submittedType == INCIDENT) {
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributes.size(); i++) {
	std::vector<std::string> currentAttribute = dataInterface->incidentAttributes[i];
	if (currentAttribute[0] == name && currentAttribute[0] != permanentName) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Attribute already exists.");
	  warningBox->setInformativeText("You cannot create two attributes with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributes.size(); i++) {
	std::vector<std::string> currentAttribute = dataInterface->incidentAttributes[i];
	if (currentAttribute[0] == permanentName && submittedLabel == EMPTY) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Attribute already exists.");
	  warningBox->setInformativeText("You cannot create two attributes with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributes.size(); i++) {
	std::vector<std::string> currentAttribute = dataInterface->incidentAttributes[i];
	if (currentAttribute[0] == permanentName && submittedLabel != EMPTY) {
	  dataInterface->incidentAttributes[i][0] = name;
	  dataInterface->incidentAttributes[i][1] = description;
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "Edited incident attribute " + submittedLabel +
	    ", which is now saved as " + QString::fromStdString(name);
	  logger->addToLog(newLog);
	  /* 
	     We should also check if this attribute has any categories assigned to it already
	     as well as any events to which the attribute has been assigned.

	     We should also check for values that have been attached to assigned attributes.
	  */
	  for (std::vector <std::vector <std::string> >::size_type j = 0;
	       j != dataInterface->assignedIncidentAttributeCategories.size(); j++) {
	    for (std::vector<std::string>::size_type k = 1; k != dataInterface->assignedIncidentAttributeCategories[j].size(); k++) {
	      if (dataInterface->assignedIncidentAttributeCategories[j][k] == permanentName) {
		dataInterface->assignedIncidentAttributeCategories[j][k] = name;
	      }
	    }
	  }
	  for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->assignedIncidentAttributes.size(); j++) {
	    if (dataInterface->assignedIncidentAttributes[j][0] == permanentName) {
	      dataInterface->assignedIncidentAttributes[j][0] = name;
	    }
	  }
	  for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->incidentValues.size(); j++) {
	    if (dataInterface->incidentValues[j][0] == permanentName) {
	      dataInterface->incidentValues[j][0] = name;
	    }
	  }
	  createNew = false;
	  this->close();
	}
      }
      if (createNew) {
	for (std::vector <std::vector <std::string> >::size_type j = 0;
	     j != dataInterface->assignedIncidentAttributeCategories.size(); j++) {
	  for (std::vector<std::string>::size_type k = 1; k != dataInterface->assignedIncidentAttributeCategories[j].size(); k++) {
	    if (dataInterface->assignedIncidentAttributeCategories[j][k] == ";;NEW;;") {
	      dataInterface->assignedIncidentAttributeCategories[j][k] = name;
	    }
	  }
	}
 	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Created new incident attribute " + QString::fromStdString(name);
	logger->addToLog(newLog);
	dataInterface->incidentAttributes.push_back(tempAttribute);
	this->close();
      }
    } else if (submittedType == ENTITY) {
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributes.size(); i++) {
	if (dataInterface->entityAttributes[i][0] == name && dataInterface->entityAttributes[i][0] != permanentName) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Attribute already exists.");
	  warningBox->setInformativeText("You cannot create two attributes with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributes.size(); i++) {
	if (dataInterface->entityAttributes[i][0] == permanentName && submittedLabel == EMPTY) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Attribute already exists.");
	  warningBox->setInformativeText("You cannot create two attributes with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributes.size(); i++) {
	if (dataInterface->entityAttributes[i][0] == permanentName && submittedLabel != EMPTY) {
	  dataInterface->entityAttributes[i][0] = name;
	  dataInterface->entityAttributes[i][1] = description;
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "Edited entity attribute " + submittedLabel +
	    ", which is now saved as " + QString::fromStdString(name);
	  logger->addToLog(newLog);

	  /* 
	     We should also check if this attribute has any categories assigned to it already
	     as well as any events to which the attribute has been assigned.

	     We should also check for values that have been attached to assigned attributes.
	  */
	  for (std::vector <std::vector <std::string> >::size_type j = 0;
	       j != dataInterface->assignedEntityAttributeCategories.size(); j++) {
	    for (std::vector<std::string>::size_type k = 1; k != dataInterface->assignedEntityAttributeCategories[j].size(); k++) {
	      if (dataInterface->assignedEntityAttributeCategories[j][k] == permanentName) {
		dataInterface->assignedEntityAttributeCategories[j][k] = name;
	      }
	    }
	  }
	  for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->assignedEntityAttributes.size(); j++) {
	    if (dataInterface->assignedEntityAttributes[j][0] == permanentName) {
	      dataInterface->assignedEntityAttributes[j][0] = name;
	    }
	  }
	  for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->entityValues.size(); j++) {
	    if (dataInterface->entityValues[j][0] == permanentName) {
	      dataInterface->entityValues[j][0] = name;
	    }
	  }
	  createNew = false;
	  this->close();
	}
      }
      if (createNew) {
	for (std::vector <std::vector <std::string> >::size_type j = 0;
	     j != dataInterface->assignedEntityAttributeCategories.size(); j++) {
	  for (std::vector<std::string>::size_type k = 1; k != dataInterface->assignedEntityAttributeCategories[j].size(); k++) {
	    if (dataInterface->assignedEntityAttributeCategories[j][k] == ";;NEW;;") {
	      dataInterface->assignedEntityAttributeCategories[j][k] = name;
	    }
	  }
	}
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Created new entity attribute " + QString::fromStdString(name);
	logger->addToLog(newLog);
	dataInterface->entityAttributes.push_back(tempAttribute);
	this->close();
      }
    }
  } else if (name == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No Label provided.");
    warningBox->setInformativeText("You need to enter a label to save the attribute.");
    warningBox->exec();
    return;
  } else if (description == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No description provided.");
    warningBox->setInformativeText("You need to enter a description to save the attribute.");
    warningBox->exec();
    return;
  }
}

bool AttributesDialog::eventFilter(QObject *target, QEvent *event) {
  if (target == nameField || target == descriptionField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Semicolon || keyEvent->key() == Qt::Key_Bar || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
	return true;
      }
    }
  }
  if (target == nameField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Comma) {
	return true;
      }
    }
  }
  return QObject::eventFilter(target, event);
}

void AttributesDialog::reject() {
  if (submittedLabel == EMPTY) {
    std::vector <std::vector <std::string> >::iterator sIt;
    if (submittedType == INCIDENT) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedIncidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributeCategories[i];
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  if (currentGroup[j] == ";;NEW;;") {
	    dataInterface->assignedIncidentAttributeCategories[i].erase(dataInterface->assignedIncidentAttributeCategories[i].begin() + j);
	  }
	}
      }
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->assignedIncidentAttributeCategories.begin();
	   it != dataInterface->assignedIncidentAttributeCategories.end();) {
	std::vector<std::string> currentAssigned = *it;
	if (currentAssigned.size() < 2) {
	  dataInterface->assignedIncidentAttributeCategories.erase(it);
	} else {
	  it++;
	}
      }
    } else if (submittedType == ENTITY) {
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedEntityAttributeCategories.size(); i++) {
	std::vector<std::string> currentGroup = dataInterface->assignedEntityAttributeCategories[i];
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  if (currentGroup[j] == ";;NEW;:") {
	    dataInterface->assignedEntityAttributeCategories[i].erase(dataInterface->assignedEntityAttributeCategories[i].begin() + j);
	  }
	}
	std::vector <std::vector <std::string> >::iterator it;
	for (it = dataInterface->assignedEntityAttributeCategories.begin();
	     it != dataInterface->assignedEntityAttributeCategories.end();) {
	  std::vector<std::string> currentAssigned = *it;
	  if (currentAssigned.size() < 2) {
	    dataInterface->assignedEntityAttributeCategories.erase(it);
	  } else {
	    it++;
	  }
	}
      }
    }
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new attribute";
    logger->addToLog(newLog);
  } else {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing attribute " + submittedLabel;
    logger->addToLog(newLog);
  }
  QDialog::reject();
}
