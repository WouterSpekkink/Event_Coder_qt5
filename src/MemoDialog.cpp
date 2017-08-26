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
  A very basic dialog to store a memo for the current event.

*/

#include "../include/MemoDialog.h"

MemoDialog::MemoDialog(QWidget *parent, std::vector<std::vector <std::string> >::size_type index,
		       const std::string currentMemo) : QDialog(parent) {
  // Entities
  memo = QString::fromStdString(currentMemo);
  eventIndex = index + 1;
  memoLabel = new QLabel("Memo for event " + QString::number(eventIndex) + ":");
  memoField = new QTextEdit;
  memoField->installEventFilter(this);

  memoField->setText(QString::fromStdString(currentMemo));
  memoField->installEventFilter(this);
  saveCloseButton = new QPushButton("Save and close");
  
  // Signals
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));

  // Layout
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  mainLayout->addWidget(memoLabel);
  mainLayout->addWidget(memoField);
  mainLayout->addWidget(saveCloseButton);

  setLayout(mainLayout);
  setWindowTitle("Edit memo");
  setFixedHeight(sizeHint().height());
}

QString MemoDialog::getMemoText() {
  return memo;
}
  
void MemoDialog::saveAndClose() {
  memo = memoField->toPlainText();
  this->close();
}

bool MemoDialog::eventFilter(QObject *target, QEvent *event) {
  if (target == memoField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Semicolon || keyEvent->key() == Qt::Key_Bar) {
	return true;
      }
    }
  }
  return QObject::eventFilter(target, event);
}
