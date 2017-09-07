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
  This dialog can be used to define new types of relationships to be used in the
  relationship dialog.

*/

#include "../include/RelTypeDialog.h"

// Initialize some constants
const QString RelTypeDialog::DIRECTED = "Directed";
const QString RelTypeDialog::UNDIRECTED = "Undirected";
const QString RelTypeDialog::EMPTY = "";

RelTypeDialog::RelTypeDialog(QWidget *parent, DataInterface *interface, const QString &label, Logger *subLogger) : QDialog(parent) {
  // First we declare the dialog's entities.
  dataInterface = interface;
  logger = subLogger;
  submittedLabel = label;
  if (submittedLabel == EMPTY) {
    directedness = DIRECTED;
    name = "";
    permanentName = "";
    description = "";
  } else {
    name = submittedLabel.toStdString();
    permanentName = name;
    std::vector <std::vector <std::string> >::iterator sIt;
    for (sIt = dataInterface->relationshipTypes.begin(); sIt != dataInterface->relationshipTypes.end(); sIt++) {
      std::vector<std::string> currentRelType = *sIt;
      if (currentRelType[0] == name) {
	description = currentRelType[1];
	directedness = QString::fromStdString(currentRelType[2]);
	permanentDirectedness = directedness;
	break;
      }
    }
  }
  
  nameLabel = new QLabel(tr("Label:"));
  nameField = new QLineEdit();

  descriptionLabel = new QLabel(tr("Description:"));
  descriptionField = new QTextEdit();
  
  nameField->installEventFilter(this);
  descriptionField->installEventFilter(this);
  nameField->setText(QString::fromStdString(name));
  descriptionField->setText(QString::fromStdString(description));
  
  directednessLabel = new QLabel(tr("Directedness:"));
  directedButton = new QPushButton(tr("Directed"));
  directedButton->setCheckable(true);
  undirectedButton = new QPushButton(tr("Undirected"));
  undirectedButton->setCheckable(true);
  if (directedness == DIRECTED) {
    directedButton->setChecked(true);
    undirectedButton->setChecked(false);
  } else if (directedness == UNDIRECTED) {
    directedButton->setChecked(false);
    undirectedButton->setChecked(true);
  }

  cancelCloseButton = new QPushButton(tr("Cancel"));
  saveCloseButton = new QPushButton(tr("Save relationship type"));

  // Then we wire the signals.
  connect(nameField, SIGNAL(textChanged(const QString &)), this, SLOT(setName(const QString &)));
  connect(directedButton, SIGNAL(clicked()), this, SLOT(checkDirectedButton()));
  connect(undirectedButton, SIGNAL(clicked()), this, SLOT(checkUndirectedButton()));
  connect(cancelCloseButton, SIGNAL(clicked()), this, SLOT(cancelAndClose()));
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
  
  // Then we create the layout.
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> nameLayout = new QHBoxLayout;
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(nameField);
  mainLayout->addLayout(nameLayout);
  QPointer<QHBoxLayout> descriptionLayout = new QHBoxLayout;
  descriptionLayout->addWidget(descriptionLabel);
  descriptionLayout->addWidget(descriptionField);
  mainLayout->addLayout(descriptionLayout);
  QPointer<QHBoxLayout> directednessLayout = new QHBoxLayout;
  directednessLayout->addWidget(directednessLabel);
  directednessLayout->addWidget(directedButton);
  directednessLayout->addWidget(undirectedButton);
  mainLayout->addLayout(directednessLayout);

  QPointer<QFrame> middleLine = new QFrame();
  middleLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(middleLine);

  QPointer<QHBoxLayout> optionLayout = new QHBoxLayout;
  optionLayout->addWidget(cancelCloseButton);
  optionLayout->addWidget(saveCloseButton);
  optionLayout->setSpacing(100);
  mainLayout->addLayout(optionLayout);

  setLayout(mainLayout);
  setWindowTitle("Add / Edit Relationship Type");
  setFixedHeight(sizeHint().height());
  // This finishes the constructor.
}

void RelTypeDialog::setName(const QString &newName) {
  name = newName.toStdString();
}

void RelTypeDialog::checkDirectedButton() {
  undirectedButton->setChecked(false);
  directedness = DIRECTED;
}

void RelTypeDialog::checkUndirectedButton() {
  directedButton->setChecked(false);
  directedness = UNDIRECTED;
}

void RelTypeDialog::cancelAndClose() {
  if (submittedLabel == EMPTY) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled adding new relationship type";
    logger->addToLog(newLog);
  } else {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Cancelled editing relationship type " + submittedLabel;
    logger->addToLog(newLog);
  }
  this->close();
}

void RelTypeDialog::saveAndClose() {
  description = (descriptionField->toPlainText()).toStdString();
  bool createNew = true;
  name.erase(std::remove(name.begin(), name.end(), ';'), name.end());
  name.erase(std::remove(name.begin(), name.end(), '|'), name.end());
  description.erase(std::remove(description.begin(), description.end(), ';'), description.end());
  description.erase(std::remove(description.begin(), description.end(), '|'), description.end());
  if (name != "" && description != "") {
    std::vector<std::string> tempRelType;
    tempRelType.push_back(name);
    tempRelType.push_back(description);
    tempRelType.push_back(directedness.toStdString());
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationshipTypes.size(); i++) {
      std::vector<std::string> currentRelType = dataInterface->relationshipTypes[i];
      if (currentRelType[0] == name && currentRelType[0] != permanentName) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship types already exists.");
	warningBox->setInformativeText("You cannot create two relationship types with identical labels.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationshipTypes.size(); i++) {
      std::vector<std::string> currentRelType = dataInterface->relationshipTypes[i];
      if (currentRelType[0] == permanentName && submittedLabel == EMPTY) {
	QPointer <QMessageBox> warningBox = new QMessageBox;
	warningBox->addButton(QMessageBox::Ok);
	warningBox->setIcon(QMessageBox::Warning);
	warningBox->setText("Relationship type already exists.");
	warningBox->setInformativeText("You cannot create two relationship types with identical labels.");
	warningBox->exec();
	return;
      }
    }
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationshipTypes.size(); i++) {
      std::vector<std::string> currentRelType = dataInterface->relationshipTypes[i];
      if (currentRelType[0] == permanentName && submittedLabel != EMPTY) {
	dataInterface->relationshipTypes[i][0] = name;
	dataInterface->relationshipTypes[i][1] = description;
	dataInterface->relationshipTypes[i][2] = directedness.toStdString();
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Edited relationship type " + submittedLabel +
	  ", which is now saved as " + QString::fromStdString(name) + " and has directedness set to " + directedness;
	logger->addToLog(newLog);

	// We should check if there are any relationships to which this relationship type was assigned.
	std::string tail = "";
	std::string permanentTail = "";
	std::string head = "]-->";
	if (directedness == DIRECTED) {
	  tail = "---[";
	} else if (directedness == UNDIRECTED) {
	  tail = "<--[";
	}
	if (permanentDirectedness == DIRECTED) {
	  permanentTail = "---[";
	} else if (permanentDirectedness == UNDIRECTED) {
	  permanentTail = "<--[";
	}
	  
	for (std::vector <std::vector <std::string> >::size_type j = 0; j != dataInterface->relationships.size(); j++) {
	  if (dataInterface->relationships[j][2] == permanentName) {
	    std::string newLabel = dataInterface->relationships[j][1] + tail + name + head + dataInterface->relationships[j][3];
	    std::string oldLabel = dataInterface->relationships[j][1] + permanentTail + permanentName + head +
	      dataInterface->relationships[j][3];
	    dataInterface->relationships[j][0] = newLabel;
	    dataInterface->relationships[j][2] = name;
	    for (std::vector <std::vector <std::string> >::size_type k = 0; k != dataInterface->assignedRelationships.size(); k++) {
	      if (dataInterface->assignedRelationships[k][0] == oldLabel) {
		dataInterface->assignedRelationships[k][0] = newLabel;
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
      QString newLog = timeText + " - " + "Created new relationship type " + QString::fromStdString(name) +
	", which has directedness set to " + directedness;
      logger->addToLog(newLog);
      dataInterface->relationshipTypes.push_back(tempRelType);
      this->close();
    }
  } else if (name == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No label provided.");
    warningBox->setInformativeText("You need to enter a label to save the relationship type.");
    warningBox->exec();
    return;
  } else if (description == "") {
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("No description provided.");
    warningBox->setInformativeText("You need to enter a description to save the new relationship type.");
    warningBox->exec();
    return;
  }
}

bool RelTypeDialog::eventFilter(QObject *target, QEvent *event) {
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
