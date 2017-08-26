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
  See DataInterface.cpp for more details on what this class and its methods do.  

 */
   
#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "../include/ImportCodesDialog.h"
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QObject>
#include <string>
#include <vector>

class DataInterface : public QObject {
  friend class MainDialog;
  friend class CodeDialog;
  friend class ImportCodesDialog;
  friend class AttributesDialog;
  friend class CategoryDialog;
  friend class RelationsDialog;
  friend class EntityDialog;
  friend class RelTypeDialog;
  friend class ExportDialog;
  Q_OBJECT

public:
  // We first have the constructor.
  DataInterface();

  // Then we have functions for reading, saving and loading files.
  void readFile(const QString &fileName, const QString &sep);
  void writeSave(const QString &fileName, std::vector<std::vector <std::string> >::size_type eventIndex, const QString &sep);
  void readSave(const QString &fileName);
  void importCodes(const QString &fileName);

  void exportData(QVector<QString> &properties, QVector<bool> &includes);

  // The deconstructor.
  ~DataInterface() {}
  
signals:
  void importFinished(); // A signal that will be send to the main dialog to indicate that importing data was finished.
  void loadFinished(const QString &eventIndex, const QString &separator);
		       
private:
  std::vector<std::string> header; // This will hold the column names of the input data file.
  std::vector<std::vector <std::string> > rowData; // This will hold the data in the file.
  std::vector<bool> eventFlagIndex; // we use this to store user flags for events.

  std::vector<std::vector <std::string> > memos; // we use this to store memos on the events.
  std::vector<std::vector <std::string> > relationMemos; // We use this to store memos on relationships.
  
  std::vector<std::vector <std::string> > incidentAttributes; // This will hold all incident attributes that the user creates.
  std::vector<std::vector <std::string> > assignedIncidentAttributes; // This will hold all attributes that are assigned to incidents.
  std::vector<std::vector <std::string> > incidentAttributeCategories; // This will hold all attribute categories that the user creates.
  std::vector<std::vector <std::string> > assignedIncidentAttributeCategories; // This will hold all the assigned attribute categories.
  std::vector<std::vector <std::string> > incidentValues; // This will hold all values that the user assigns to incident variables.
  std::vector<std::vector <std::string> > relationships; // This will hold all the relationships that the user creates.
  std::vector<std::vector <std::string> > assignedRelationships; // This will hold all relationships that the user assigned to events.
  std::vector<std::vector <std::string> > entities; // This will hold all the entities that the use creates.
  std::vector<std::vector <std::string> > entityAttributes;// This will hold all the entity attributs that the user creates.
  std::vector<std::vector <std::string> > assignedEntityAttributes; // This will hold all the attributes that are assigned to entities.
  std::vector<std::vector <std::string> > entityValues; // This will hold all values that the user assigns to entity variables.
  std::vector<std::vector <std::string> > entityAttributeCategories; // This will hold all the entity categories that the user creates.
  std::vector<std::vector <std::string> > assignedEntityAttributeCategories; // This will hold all the assigned entity categories.
  std::vector<std::vector <std::string> > relationshipTypes; // This will hold all the relationship types that the user creates.
  
  // We set some constant static strings that we will use more when saving and loading files.
  const static std::string IMPORTED_BEGIN;
  const static std::string IMPORTED_END;
  const static std::string ROW_PART;
  const static std::string ROW_END;
  const static std::string INDEX_BEGIN;
  const static std::string INDEX_END;
  const static std::string SEP_BEGIN;
  const static std::string SEP_END;
  const static std::string ROW_PART_2;
  const static std::string MEMOS_BEGIN;
  const static std::string MEMOS_END;
  const static std::string RELATIONMEMOS_BEGIN;
  const static std::string RELATIONMEMOS_END;
  const static std::string EVENTFLAGS_BEGIN;
  const static std::string EVENTFLAGS_END;
  const static std::string INCIDENTATTRIBUTES_BEGIN;
  const static std::string INCIDENTATTRIBUTES_END;
  const static std::string ASSIGNEDINCIDENTATTRIBUTES_BEGIN;
  const static std::string ASSIGNEDINCIDENTATTRIBUTES_END;
  const static std::string ICATEGORIES_BEGIN;
  const static std::string ICATEGORIES_END;
  const static std::string ASSIGNEDICATEGORIES_BEGIN;
  const static std::string ASSIGNEDICATEGORIES_END;
  const static std::string INCIDENTVALUES_BEGIN;
  const static std::string INCIDENTVALUES_END;
  const static std::string RELATIONSHIPS_BEGIN;
  const static std::string RELATIONSHIPS_END;
  const static std::string ASSIGNEDRELATIONSHIPS_BEGIN;
  const static std::string ASSIGNEDRELATIONSHIPS_END;
  const static std::string ENTITIES_BEGIN;
  const static std::string ENTITIES_END;
  const static std::string ENTITYATTRIBUTES_BEGIN;
  const static std::string ENTITYATTRIBUTES_END;
  const static std::string ASSIGNEDENTITYATTRIBUTES_BEGIN;
  const static std::string ASSIGNEDENTITYATTRIBUTES_END;
  const static std::string ENTITYVALUES_BEGIN;
  const static std::string ENTITYVALUES_END;
  const static std::string ECATEGORIES_BEGIN;
  const static std::string ECATEGORIES_END;
  const static std::string ASSIGNEDECATEGORIES_BEGIN;
  const static std::string ASSIGNEDECATEGORIES_END;
  const static std::string RELTYPES_BEGIN;
  const static std::string RELTYPES_END;
};
#endif
