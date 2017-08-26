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
  This dialog will be used by the user to export data to output files.

*/

#include "../include/ExportDialog.h"


ExportDialog::ExportDialog(QWidget *parent, DataInterface *interface) : QDialog(parent) {
  // First we initialise variables en set up the dialog's entities.
  dataInterface = interface;
  informativeLabel = new QLabel(tr("<h3>Export options</h3>"));
  incidentsLabel = new QLabel(tr("<h4>Incident export options:</h4>"));

  QVector<QString> Qheader;
  std::vector<std::string>::iterator it;
  for (it = dataInterface->header.begin(); it != dataInterface->header.end(); it++) {
    std::string tempString = *it;
    Qheader.push_back(QString::fromStdString(tempString));
  }
  
  incidentPropertiesLabel = new QLabel(tr("Select incident properties:"));

  QVectorIterator<QString> it2(Qheader);
  while (it2.hasNext()) {
    QPointer<QCheckBox> tempBox = new QCheckBox(it2.next(), this);
    incidentPropertiesVector.push_back(tempBox);
  }
  
  incidentOthersLabel = new QLabel(tr("<h4>Other options for incidents:</h4>"));
  incidentAttributesCheckBox = new QCheckBox("Include incident attributes", this);
  incidentCategoriesCheckBox = new QCheckBox("Include incident categories", this);

  relationshipsLabel = new QLabel(tr("<h4>Relationship export options:</h4>"));
  relationshipsCheckBox = new QCheckBox("Export relationships", this);
  entityAttributesCheckBox = new QCheckBox("Include entity attributes", this);
  entityCategoriesCheckBox = new QCheckBox("Include entity categories", this);

  closeButton = new QPushButton(tr("Close"));
  exportButton = new QPushButton(tr("Export Data"));
  
  // Then we connect the signals
  connect(incidentAttributesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkAttributes()));
  connect(relationshipsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkRelations()));
  connect(entityAttributesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkEntities()));
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportData()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

  // Then we create the layout
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  mainLayout->addWidget(informativeLabel);
  informativeLabel->setAlignment(Qt::AlignHCenter);

  QPointer<QFrame> topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(topLine);

  mainLayout->addWidget(incidentsLabel);
  incidentsLabel->setAlignment(Qt::AlignHCenter);
  mainLayout->addWidget(incidentPropertiesLabel);
  
  QVectorIterator<QPointer<QCheckBox> > cI(incidentPropertiesVector);
  while (cI.hasNext()) {
    mainLayout->addWidget(cI.next());
  }
  
  mainLayout->addWidget(incidentOthersLabel);
  incidentOthersLabel->setAlignment(Qt::AlignHCenter);
  mainLayout->addWidget(incidentAttributesCheckBox);
  incidentAttributesCheckBox->setChecked(true);
  mainLayout->addWidget(incidentCategoriesCheckBox);
  incidentCategoriesCheckBox->setChecked(true);

  QPointer<QFrame> middleLine = new QFrame();
  middleLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(middleLine);

  mainLayout->addWidget(relationshipsLabel);
  relationshipsLabel->setAlignment(Qt::AlignHCenter);
  mainLayout->addWidget(relationshipsCheckBox);
  relationshipsCheckBox->setChecked(true);
  mainLayout->addWidget(entityAttributesCheckBox);
  entityAttributesCheckBox->setChecked(true);
  mainLayout->addWidget(entityCategoriesCheckBox);
  entityCategoriesCheckBox->setChecked(true);
  
  QPointer<QFrame> bottomLine = new QFrame();
  bottomLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(bottomLine);

  QPointer<QHBoxLayout> optionsLayout = new QHBoxLayout;
  optionsLayout->addWidget(closeButton);
  optionsLayout->addWidget(exportButton);
  mainLayout->addLayout(optionsLayout);

  setLayout(mainLayout);
  setWindowTitle("Export dialog");
  setFixedHeight(sizeHint().height());
  // And this finishes the constructor
}

void ExportDialog::closeDialog() {
  qDeleteAll(incidentPropertiesVector);
  incidentPropertiesVector.clear();
  this->close();
}

void ExportDialog::checkAttributes() {
  if (incidentAttributesCheckBox->checkState() == Qt::Unchecked) {
    incidentCategoriesCheckBox->setCheckState(Qt::Unchecked);
    incidentCategoriesCheckBox->setEnabled(false);
  } else {
    incidentCategoriesCheckBox->setEnabled(true);
  }
}

void ExportDialog::checkRelations() {
  if (relationshipsCheckBox->checkState() == Qt::Unchecked) {
    entityAttributesCheckBox->setCheckState(Qt::Unchecked);
    entityAttributesCheckBox->setEnabled(false);
  } else {
    entityAttributesCheckBox->setEnabled(true);
  }
}

void ExportDialog::checkEntities() {
 if (entityAttributesCheckBox->checkState() == Qt::Unchecked) {
    entityCategoriesCheckBox->setCheckState(Qt::Unchecked);
    entityCategoriesCheckBox->setEnabled(false);
  } else {
    entityCategoriesCheckBox->setEnabled(true);
  }
}

void ExportDialog::exportData() {
  QVector<QString> incidentProperties;
  QVector<bool> otherChecks;
  QVectorIterator<QPointer<QCheckBox> > cI(incidentPropertiesVector);
  while (cI.hasNext()) {
    QPointer<QCheckBox> tempBox = cI.next();
    if (tempBox->checkState() == Qt::Checked) {
      incidentProperties.push_back(tempBox->text());
    }
  }
  if (incidentAttributesCheckBox->checkState() == Qt::Checked) {
    otherChecks.push_back(true);
  } else {
    otherChecks.push_back(false);
  }
  if (incidentCategoriesCheckBox->checkState() == Qt::Checked) {
    otherChecks.push_back(true);
  } else {
    otherChecks.push_back(false);
  }
  if (relationshipsCheckBox->checkState() == Qt::Checked) {
    otherChecks.push_back(true);
  } else {
    otherChecks.push_back(false);
  }
  if (entityAttributesCheckBox->checkState() == Qt::Checked) {
    otherChecks.push_back(true);
  } else {
    otherChecks.push_back(false);
  }
  if (entityCategoriesCheckBox->checkState() == Qt::Checked) {
    otherChecks.push_back(true);
  } else {
    otherChecks.push_back(false);
  }
  dataInterface->exportData(incidentProperties, otherChecks);
  this->close();
}


