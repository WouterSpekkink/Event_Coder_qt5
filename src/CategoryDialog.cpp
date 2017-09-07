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
  This dialog can be used to create new categories for attributes or entities.

*/

#include "../include/CategoryDialog.h"

// Define some constants

const QString CategoryDialog::EMPTY = "";
const QString CategoryDialog::INCIDENT = "Incident";
const QString CategoryDialog::ENTITY = "Entity";

CategoryDialog::CategoryDialog(QWidget *parent, DataInterface *interface,
			       const QString &label, const QString type, Logger *subLogger) : QDialog(parent) {
  dataInterface = interface;
  logger = subLogger;
  currentType = type;
  submittedLabel = label;
  if (currentType == EMPTY) {
    // In this case we do nothing. Maybe I should just remove it.
  } else if (type == INCIDENT) {
    if (submittedLabel == EMPTY) {
      name == "";
      permanentName = "";
      description = "";
    } else {
      name = submittedLabel.toStdString();
      permanentName = name;
      std::vector <std::vector <std::string> >::iterator sIt;
      for (sIt = dataInterface->incidentAttributeCategories.begin(); sIt != dataInterface->incidentAttributeCategories.end(); sIt++) {
	std::vector<std::string> currentCategory = *sIt;
	if (currentCategory[0] == name) {
	  description = currentCategory[1];
	}
      }
    }
  } else if (type == ENTITY) {
    if (submittedLabel == EMPTY) {
      name = "";
      permanentName = "";
      description = "";
    } else {
      name = submittedLabel.toStdString();
      permanentName = name;
      std::vector <std::vector <std::string> >::iterator sIt;
      for (sIt = dataInterface->entityAttributeCategories.begin(); sIt != dataInterface->entityAttributeCategories.end(); sIt++) {
	std::vector<std::string> currentCategory = *sIt;
	if (currentCategory[0] == name) {
	  description = currentCategory[1];
	}
      }
    }
  }

  // First we create the dialog's entities.
  nameLabel = new QLabel(tr("Name:"));
  nameField = new QLineEdit();

  descriptionLabel = new QLabel(tr("Description:"));
  descriptionField = new QTextEdit();

  nameField->setText(QString::fromStdString(name));
  descriptionField->setText(QString::fromStdString(description));
  descriptionField->installEventFilter(this);
  nameField->installEventFilter(this);
  
  cancelCloseButton = new QPushButton(tr("Cancel"));
  saveCloseButton = new QPushButton(tr("Save category"));

  // Then we wire the signals.
  connect(nameField, SIGNAL(textChanged(const QString &)), SLOT(setName(const QString &)));
  connect(cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelAndClose()));
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));

  // Then we create the layout.
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> nameLayout = new QHBoxLayout;
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(nameField);
  mainLayout->addLayout(nameLayout);
  mainLayout->addWidget(descriptionLabel);
  mainLayout->addWidget(descriptionField);
  QPointer<QHBoxLayout> optionLayout = new QHBoxLayout;
  optionLayout->addWidget(cancelCloseButton);
  optionLayout->addWidget(saveCloseButton);
  optionLayout->setSpacing(100);
  mainLayout->addLayout(optionLayout);

  setLayout(mainLayout);
  setWindowTitle("Add / Edit Category");
  setFixedHeight(sizeHint().height());
  // And that finishes the constructor.
}

void CategoryDialog::setName(const QString &newName) {
  name = newName.toStdString();
  if (submittedLabel == EMPTY) {
    permanentName = name;
  }
}

void CategoryDialog::cancelAndClose() {
  if (submittedLabel == EMPTY) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new category";
    logger->addToLog(newLog);
  } else {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing category " + submittedLabel;
    logger->addToLog(newLog);
  }
  this->close();
}

void CategoryDialog::saveAndClose() {
  description = (descriptionField->toPlainText()).toStdString();
  bool createNew = true;
  name.erase(std::remove(name.begin(), name.end(), ';'), name.end());
  name.erase(std::remove(name.begin(), name.end(), '|'), name.end());
  description.erase(std::remove(description.begin(), description.end(), ';'), description.end());
  description.erase(std::remove(description.begin(), description.end(), '|'), description.end());
  if (name != "" && description != "") {
    std::vector<std::string> tempCategory;
    tempCategory.push_back(name);
    tempCategory.push_back(description);
    if (currentType == INCIDENT) {
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->incidentAttributeCategories[i];
	if (currentCategory[0] == name && currentCategory[0] != permanentName) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Category already exists.");
	  warningBox->setInformativeText("You cannot create two Categories with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->incidentAttributeCategories[i];
	if (currentCategory[0] == permanentName && submittedLabel == EMPTY) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Category already exists.");
	  warningBox->setInformativeText("You cannot create two categories with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->incidentAttributeCategories[i];
	if (currentCategory[0] == permanentName) {
	  dataInterface->incidentAttributeCategories[i][0] = name;
	  dataInterface->incidentAttributeCategories[i][1] = description;
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "Edited incident attribute category " + submittedLabel +
	    ", which is now saved as " + QString::fromStdString(name);
	  logger->addToLog(newLog);

	  /* 
	   We should also check if this category was already assigned to some attributes already.
	  */
	  for (std::vector <std::vector <std::string> >::size_type j = 0;
	       j != dataInterface->assignedIncidentAttributeCategories.size(); j++) {
	    std::vector<std::string> currentCategory = dataInterface->assignedIncidentAttributeCategories[j];
	    if (dataInterface->assignedIncidentAttributeCategories[j][0] == permanentName) {
	      dataInterface->assignedIncidentAttributeCategories[j][0] = name;
	    }
	  }
	  createNew = false;
	  this->close();
	}
      }
      if (createNew) {
	dataInterface->incidentAttributeCategories.push_back(tempCategory);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Created incident attribute category " + QString::fromStdString(name);
	logger->addToLog(newLog);
	this->close();
      }
    } else if (currentType == ENTITY) {
      // SHOULD CHECK THIS CAREFULLY
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->entityAttributeCategories[i];
	if (currentCategory[0] == name && currentCategory[0] != permanentName) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Category already exists.");
	  warningBox->setInformativeText("You cannot create two categories with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->entityAttributeCategories[i];
	if (currentCategory[0] == permanentName && submittedLabel == EMPTY) {
	  QPointer <QMessageBox> warningBox = new QMessageBox;
	  warningBox->addButton(QMessageBox::Ok);
	  warningBox->setIcon(QMessageBox::Warning);
	  warningBox->setText("Category already exists.");
	  warningBox->setInformativeText("You cannot create two categories with identical names.");
	  warningBox->exec();
	  return;
	}
      }
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->entityAttributeCategories.size(); i++) {
	std::vector<std::string> currentCategory = dataInterface->entityAttributeCategories[i];
	if (currentCategory[0] == permanentName) {
	  dataInterface->entityAttributeCategories[i][0] = name;
	  dataInterface->entityAttributeCategories[i][1] = description;
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "Edited entity attribute category " + submittedLabel +
	    ", which is now saved as " + QString::fromStdString(name);
	  logger->addToLog(newLog);

	  /* 
	   We should also check if this category was already assigned to some entity already.
	  */
	  for (std::vector <std::vector <std::string> >::size_type j = 0;
	       j != dataInterface->assignedEntityAttributeCategories.size(); j++) {
	    std::vector<std::string> currentCategory = dataInterface->assignedEntityAttributeCategories[j];
	    if (dataInterface->assignedEntityAttributeCategories[j][0] == permanentName) {
	      dataInterface->assignedEntityAttributeCategories[j][0] = name;
	    }
	  }
	  createNew = false;
	  this->close();
	}
      }
      if (createNew) {
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Created entity attribute category " + QString::fromStdString(name);
	logger->addToLog(newLog);
	dataInterface->entityAttributeCategories.push_back(tempCategory);
	this->close();
      }
    }
  } else if (name == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No name provided.");
    warningBox->setInformativeText("You need to enter a name to save the category.");
    warningBox->exec();
    return;
  } else if (description == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No description provided.");
    warningBox->setInformativeText("You need to enter a description to save the category.");
    warningBox->exec();
    return;
  }
}

bool CategoryDialog::eventFilter(QObject *target, QEvent *event) {
  if (target == nameField || target == descriptionField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Semicolon || keyEvent->key() == Qt::Key_Bar) {
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
