/*
 Copyright 2017 Wouter Spekkink 
 Authors : Wouter Spekkink <wouterspekkink@gmail.com> 
 Website : http://www.wouterspekkink.org

 This file is part of the Linkage Coder Tool.

 DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.

 Copyright 2017 Wouter Spekkink. All rights reserved.

 The Linkage Coder Tool is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 The Linkage Coder Tool is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with the Linkage Coder Tool.  If not, see <http://www.gnu.org/licenses/>.

*/ 

/*
  ===NOTES===
  This dialog will be used for the user to input two indexes, which the program will
  then attempt to jump to.
*/

#include "../include/IndexDialog.h"
#include <QIntValidator>

IndexDialog::IndexDialog(QWidget *parent, std::vector <std::vector <std::string> >::size_type max) : QDialog(parent) {
  // First we construct our dialog's entities.
  indexLabel = new QLabel(tr("Incident index:"));
  indexText = "";
  indexField = new QLineEdit();
  indexField->setValidator(new QIntValidator(1, max, this)); 
  goButton = new QPushButton(tr("Go"));
  goButton->setEnabled(false); // We only activate this button if there is something in the fields.
  cancelButton = new QPushButton(tr("Cancel"));

  // We connect the signals to the appropriate slots.
  connect(goButton, SIGNAL(clicked()), this, SLOT(go()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
  connect(indexField, SIGNAL(textChanged(const QString &)), this, SLOT(setIndexText(const QString &)));
  
  // Then we create the layout.
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;

  QPointer<QHBoxLayout> fieldLayout = new QHBoxLayout;
  fieldLayout->addWidget(indexLabel);
  fieldLayout->addWidget(indexField);

  QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(goButton);
  buttonLayout->addWidget(cancelButton);
  
  mainLayout->addLayout(fieldLayout);
  mainLayout->addLayout(buttonLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("Jump to index"));
  setFixedHeight(sizeHint().height());
  // And this finished the constructor.
}

void IndexDialog::setIndexText(const QString newIndex) {
  indexText = newIndex.toStdString();
  indexText.erase(indexText.find_last_not_of(" \n\r\t")+1);
  // Let us immediately check if we should active the go Button.
  if (indexText != "") {
    goButton->setEnabled(true); 
  } else {
    goButton->setEnabled(false);
  }
}

void IndexDialog::go() {
  this->close();
}

void IndexDialog::cancel() {
  indexText = "0";
  this->close();
}

std::string IndexDialog::getIndexText() {
  return indexText;
}

