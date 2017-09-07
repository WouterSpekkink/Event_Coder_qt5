/*
 Copyright 2017 Wouter Spekkink 
 Authors : Wouter Spekkink <wouterspekkink@gmail.com> 
 Website : http://www.wouterspekkink.org

 This file is part of the Event Code Tool.

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
  ==NOTES==
  This class handles all interaction of the program with 'external data'. These data
  typically take the form csv files selected by the user, or save files created by the 
  user while interacting with the program. The DataInterface class can be used to read,
  store, and write data. 
*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <QMessageBox>
#include <QPointer>
#include "../include/DataInterface.h"

const std::string DataInterface::IMPORTED_BEGIN = "<imported_data>";
const std::string DataInterface::IMPORTED_END = "</imported_data>";
const std::string DataInterface::ROW_PART = "<row rowid=";
const std::string DataInterface::ROW_END =  "</row>";
const std::string DataInterface::INDEX_BEGIN = "<index>";
const std::string DataInterface::INDEX_END = "</index>";
const std::string DataInterface::SEP_BEGIN = "<sep>";
const std::string DataInterface::SEP_END = "</sep>";
const std::string DataInterface::ROW_PART_2 = "<row_";
const std::string DataInterface::MEMOS_BEGIN = "<memos>";
const std::string DataInterface::MEMOS_END = "</memos>";
const std::string DataInterface::RELATIONMEMOS_BEGIN = "<relationship_memos>";
const std::string DataInterface::RELATIONMEMOS_END = "</relationship_memos>";
const std::string DataInterface::EVENTFLAGS_BEGIN = "<event_flags>";
const std::string DataInterface::EVENTFLAGS_END = "</event_flags>";
const std::string DataInterface::INCIDENTATTRIBUTES_BEGIN = "<incident_attributes>";
const std::string DataInterface::INCIDENTATTRIBUTES_END = "</incident_attributes>";
const std::string DataInterface::ASSIGNEDINCIDENTATTRIBUTES_BEGIN = "<assigned_incident_attributes>";
const std::string DataInterface::ASSIGNEDINCIDENTATTRIBUTES_END = "</assigned_incident_attributes>";
const std::string DataInterface::ICATEGORIES_BEGIN = "<incident_attribute_categories>";
const std::string DataInterface::ICATEGORIES_END = "</incident_attribute_categories>";
const std::string DataInterface::ASSIGNEDICATEGORIES_BEGIN = "<assigned_incident_attribute_categories>";
const std::string DataInterface::ASSIGNEDICATEGORIES_END = "</assigned_incident_attribute_categories>";
const std::string DataInterface::INCIDENTVALUES_BEGIN = "<incident_attribute_values>";
const std::string DataInterface::INCIDENTVALUES_END = "</incident_attribute_values>";
const std::string DataInterface::RELATIONSHIPS_BEGIN = "<relationships>";
const std::string DataInterface::RELATIONSHIPS_END = "</relationships>";
const std::string DataInterface::ASSIGNEDRELATIONSHIPS_BEGIN = "<assigned_relationships>";
const std::string DataInterface::ASSIGNEDRELATIONSHIPS_END = "</assigned_relationships>";
const std::string DataInterface::ENTITIES_BEGIN = "<entities>";
const std::string DataInterface::ENTITIES_END = "</entities>";
const std::string DataInterface::ENTITYATTRIBUTES_BEGIN = "<entity_attributes>";
const std::string DataInterface::ENTITYATTRIBUTES_END = "</entity_attributes>"; 
const std::string DataInterface::ASSIGNEDENTITYATTRIBUTES_BEGIN = "</assigned_entity_attributes>";
const std::string DataInterface::ASSIGNEDENTITYATTRIBUTES_END = "</assigned_entity_attributes>";
const std::string DataInterface::ENTITYVALUES_BEGIN = "<entity_attribute_values>";
const std::string DataInterface::ENTITYVALUES_END = "</entity_attribute_values>";
const std::string DataInterface::ECATEGORIES_BEGIN = "<entity_attribute_categories>";
const std::string DataInterface::ECATEGORIES_END = "</entity_attribute_categories>";
const std::string DataInterface::ASSIGNEDECATEGORIES_BEGIN = "<assigned_entity_attribute_categories>";
const std::string DataInterface::ASSIGNEDECATEGORIES_END = "</assigned_entity_attribute_categories>";
const std::string DataInterface::RELTYPES_BEGIN = "<relationship_types>";
const std::string DataInterface::RELTYPES_END = "</relationship_types>";

// Constructor for this class.
DataInterface::DataInterface()  
{
}

// This method is used to read data from a newly imported csv file. 
void DataInterface::readFile(const QString &fileName, const QString &sep) 
{
  // Let's first make sure that our current DataInterface is clean.
  header.clear();
  rowData.clear();
  memos.clear();
  relationMemos.clear();
  eventFlagIndex.clear();

  incidentAttributes.clear();
  assignedIncidentAttributes.clear();
  incidentAttributeCategories.clear();
  assignedIncidentAttributeCategories.clear();
  incidentValues.clear();
  relationships.clear();
  assignedRelationships.clear();
  entities.clear();
  entityAttributes.clear();
  assignedEntityAttributes.clear();
  entityValues.clear();
  entityAttributeCategories.clear();
  assignedEntityAttributeCategories.clear();
  relationshipTypes.clear();

  // Translating the method arguments to something that this class can use.
  // We sometimes need the separator as a char, and sometimes we need it as a string.
  const std::string inputFile = fileName.toStdString();
  std::string sepString = sep.toStdString();
  std::istringstream convert(sepString.c_str());
  char sepChar;
  convert >> sepChar;
    
  // Let's first make a vector of vectors of strings. Currently, this will make a matrix of strings.
  std::vector <std::vector <std::string> > dataVector;

  // Set up an file instream for the input file.
  std::ifstream myFile (inputFile.c_str());
  // Then we read all the lines of data in the input file.
  while(myFile) {
    // The buffer will hold one line of raw data temporarily, and we will do some processing on it.
    std::string buffer;

    if (!getline(myFile, buffer)) break;
    /* I had some problems reading files with embedded line breaks. 
       It should be easy to spot these linebreaks by checking for unmatched quotes.  
       However, I am not sure whether unmatched quotes are necessarily caused by embedded linebreaks.
       Therefore, the program shows an error box, cancels the import process, and leaves it to the 
       user to deal with the problem outside the program. 
    */
    bool quoteFound = false;
      
    for (std::string::size_type i = 0; i != buffer.length(); i++) {
      if (quoteFound == false && buffer[i] == '"') {
	quoteFound = true;
      } else if (quoteFound == true && buffer[i] == '"') {
	  quoteFound = false;
      }
    }
    if (quoteFound == true) {
      QPointer<QMessageBox> errorBox = new QMessageBox;
      errorBox->setText(tr("<b>ERROR: Import cancelled</b>"));
      errorBox->setInformativeText("Unmatched quotes (\") were found in one of the lines of the file.");
      errorBox->exec();
      return;
    }
    // This boolean will indicate whether or not we find ourselves in a text field. These may hold
    // delimiter symbols that should be ignored. The code below is customized to do just that.
    bool inTextField = false;
    std::vector <std::string> currentLineProcessed;
    std::string::size_type stringLength = 0;
    std::string::size_type previousPos = 0;
    for (std::string::size_type i = 0; i != buffer.length(); i++) {
      if (inTextField == false && buffer[i] == '"') {
	inTextField = true;
	previousPos++;
	stringLength--;
      } else if (inTextField == true && buffer[i] == '"') {
	inTextField = false;
	stringLength--;
      }
      if (inTextField == false && buffer[i] == sepChar) {
	while (buffer[previousPos] == ' ') {
	  previousPos++;
	  stringLength--;
	}
	std::string tempString = buffer.substr(previousPos, stringLength);
	currentLineProcessed.push_back(tempString);
	previousPos = i + 1;
	stringLength = 0;
      } else {
	stringLength++;
      }
    }
    while (buffer[previousPos] == ' ') {
      previousPos++;
      stringLength--;
    }
    std::string tempString = buffer.substr(previousPos, stringLength);
    currentLineProcessed.push_back(tempString);
    // And then we push this line of data in the larger data vector.
    dataVector.push_back(currentLineProcessed);
  }

  // This will disect the data into the header row and the data rows. 
  std::vector<std::vector <std::string> >::iterator it;
  for(it = dataVector.begin(); it != dataVector.end(); it++) {
    // The first line is always the header
    if(it == dataVector.begin()) {
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++)
	header.push_back(*it2);
    } else {
      // Then we handle the other 'rows' in the data vector.
      rowData.push_back(*it);
    }
  }
  if (header.empty()) {
    QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Import cancelled</b>"));
    errorBox->setInformativeText("Something strange happened during the import. Did you set the delimiters correctly?");
    errorBox->exec();
    return;
  }
  // This signal is sent to the main dialog to let it know we have finished importing the file.

  // And we set all entries of the flag indexes to false.
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != rowData.size(); i++) {
    eventFlagIndex.push_back(false);
  }
  
  emit importFinished();
  return;
}

void DataInterface::writeSave(const QString &fileName, std::vector<std::vector <std::string> >::size_type eventIndex, const QString &sep) {
  // Let's first create versions of the arguments that we can work with.
  std::stringstream ss;
  ss << eventIndex;
  const std::string index = ss.str();
  const std::string saveFile = fileName.toStdString();
  const std::string separator = sep.toStdString();

  // Let's then set up our save file.
  std::ofstream fileOut(saveFile.c_str());

  if (!fileOut.is_open()) {
    QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Cannot write file</b>"));
    errorBox->setInformativeText("It appears impossible to open the file to write data.");
    errorBox->exec();
    return;
  }

  // We first write the header of the xml-file.
  fileOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

  // Next we write the data from the imported file.
  fileOut << IMPORTED_BEGIN << "\n";
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != rowData.size(); i++) {
    std::vector <std::string> currentRow = rowData[i];
    ss << i;
    std::string rowLabel = "row_" + ss.str();
    ss.str(std::string());
    
    fileOut << ROW_PART << "\"" << rowLabel << "\">\n";
    for (std::vector <std::string>::size_type j = 0; j != header.size(); j++) {
      std::string currentHeader = header[j];
      std::string currentCell = currentRow[j];
      /*
	When writing and reading save files, we would potentially get in trouble if the user's data
	file has lines that start with "<", so we change these now in case there are any.
      */
      if (currentCell[0] == '<') {
	currentCell[0] = '_';
      }
      fileOut << "<" << currentHeader << ">\n" << currentCell << "\n" << "</" << currentHeader << ">\n";
    }
    fileOut << ROW_END << "\n";
  }
  fileOut << IMPORTED_END << "\n";

  // Next, we will write the indexes that the user is currently at. 
 
  fileOut << INDEX_BEGIN << "\n" << index << "\n" << INDEX_END << "\n";
  fileOut << SEP_BEGIN << "\n" << separator << "\n" << SEP_END << "\n";
							       
  fileOut << MEMOS_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator emIt;
  for (emIt = memos.begin(); emIt != memos.end(); emIt++) {
    std::vector<std::string> currentMemo = *emIt;
    std::vector<std::string>::iterator emIt2;
    if (emIt != memos.end() - 1) {
      for (emIt2 = currentMemo.begin(); emIt2 != currentMemo.end(); emIt2++) {
	if (emIt2 != currentMemo.end() - 1) {
	  fileOut << *emIt2 << "|";
	} else {
	  fileOut << *emIt2 << ";";
	}
      }
    } else {
      for (emIt2 = currentMemo.begin(); emIt2 != currentMemo.end(); emIt2++) {
	if (emIt2 != currentMemo.end() - 1) {
	  fileOut << *emIt2 << "|";
	} else {
	  fileOut << *emIt2 << "\n";
	}
      }
    }
  }
  
  fileOut << MEMOS_END << "\n"; 

  fileOut << RELATIONMEMOS_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator rmIt;
  for (rmIt = relationMemos.begin(); rmIt != relationMemos.end(); rmIt++) {
    std::vector<std::string> currentMemo = *rmIt;
    std::vector<std::string>::iterator rmIt2;
    if (rmIt != relationMemos.end() - 1) {
      for (rmIt2 = currentMemo.begin(); rmIt2 != currentMemo.end(); rmIt2++) {
	if (rmIt2 != currentMemo.end() - 1) {
	  fileOut << *rmIt2 << "|";
	} else {
	  fileOut << *rmIt2 << ";";
	}
      }
    } else {
      for (rmIt2 = currentMemo.begin(); rmIt2 != currentMemo.end(); rmIt2++) {
	if (rmIt2 != currentMemo.end() - 1) {
	  fileOut << *rmIt2 << "|";
	} else {
	  fileOut << *rmIt2 << "\n";
	}
      }
    }
  }
  fileOut << RELATIONMEMOS_END << "\n"; 

  
  fileOut << EVENTFLAGS_BEGIN << "\n";
  std::vector<bool>::iterator flagIt;
  for (flagIt = eventFlagIndex.begin(); flagIt != eventFlagIndex.end(); flagIt++) {
    if (flagIt != eventFlagIndex.end() - 1) {
      if (*flagIt == true) {
	fileOut << "1;"; 
      } else if (*flagIt == false) {
	fileOut << "0;";
      }
    } else {
      if (*flagIt == true) {
	fileOut << "1\n"; 
      } else if (*flagIt == false) {
	fileOut << "0\n";
      }
    }
  }
  
  fileOut << EVENTFLAGS_END << "\n";

  fileOut << INCIDENTATTRIBUTES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator attrIt;
  for (attrIt = incidentAttributes.begin(); attrIt != incidentAttributes.end(); attrIt++) {
    std::vector<std::string> currentAttribute = *attrIt;
    std::vector<std::string>::iterator attrIt2;
    if (attrIt != incidentAttributes.end() - 1) {
      for (attrIt2 = currentAttribute.begin(); attrIt2 != currentAttribute.end(); attrIt2++) {
	if (attrIt2 != currentAttribute.end() - 1) {
	  fileOut << *attrIt2 << "|";
	} else {
	  fileOut << *attrIt2 << ";";
	}
      }
    } else {
      for (attrIt2 = currentAttribute.begin(); attrIt2 != currentAttribute.end(); attrIt2++) {
	if (attrIt2 != currentAttribute.end() - 1) {
	  fileOut << *attrIt2 << "|";
	} else {
	  fileOut << *attrIt2 << "\n";
	}
      }
    }
  }
  
  fileOut << INCIDENTATTRIBUTES_END << "\n";

  fileOut << ASSIGNEDINCIDENTATTRIBUTES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator aaIt;
  for (aaIt = assignedIncidentAttributes.begin(); aaIt != assignedIncidentAttributes.end(); aaIt++) {
    std::vector<std::string> currentPair = *aaIt;
    std::vector<std::string>::iterator aaIt2;
    if (aaIt != assignedIncidentAttributes.end() - 1) {
      for (aaIt2 = currentPair.begin(); aaIt2 != currentPair.end(); aaIt2++) {
	if (aaIt2 != currentPair.end() - 1) {
	  fileOut << *aaIt2 << "|";
	} else {
	  fileOut << *aaIt2 << ";";
	}
      }
    } else {
      for (aaIt2 = currentPair.begin(); aaIt2 != currentPair.end(); aaIt2++) {
	if (aaIt2 != currentPair.end() - 1) {
	  fileOut << *aaIt2 << "|";
	} else {
	  fileOut << *aaIt2 << "\n";
	}
      }
    }
  }  
  fileOut << ASSIGNEDINCIDENTATTRIBUTES_END << "\n";

  fileOut << INCIDENTVALUES_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator vIt;
  for (vIt = incidentValues.begin(); vIt != incidentValues.end(); vIt++) {
    std::vector<std::string> currentGroup = *vIt;
    std::vector<std::string>::iterator vIt2;
    if (vIt != incidentValues.end() - 1) {
      for (vIt2 = currentGroup.begin(); vIt2 != currentGroup.end(); vIt2++) {
	if (vIt2 != currentGroup.end() - 1) {
	  fileOut << *vIt2 << "|";
	} else {
	  fileOut << *vIt2 << ";";
	}
      }
    } else {
      for (vIt2 = currentGroup.begin(); vIt2 != currentGroup.end(); vIt2++) {
	if (vIt2 != currentGroup.end() - 1) {
	  fileOut << *vIt2 << "|";
	} else {
	  fileOut << *vIt2 << "\n";
	}
      }
    }
  }
  fileOut << INCIDENTVALUES_END << "\n";
 
  fileOut << ICATEGORIES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator cIt;
  for (cIt = incidentAttributeCategories.begin(); cIt != incidentAttributeCategories.end(); cIt++) {
    std::vector<std::string> currentCategory = *cIt;
    std::vector<std::string>::iterator cIt2;
    if (cIt != incidentAttributeCategories.end() - 1) {
      for (cIt2 = currentCategory.begin(); cIt2 != currentCategory.end(); cIt2++) {
	if (cIt2 != currentCategory.end() - 1) {
	  fileOut << *cIt2 << "|";
	} else {
	  fileOut << *cIt2 << ";";
	}
      }
    } else {
      for (cIt2 = currentCategory.begin(); cIt2 != currentCategory.end(); cIt2++) {
	if (cIt2 != currentCategory.end() - 1) {
	  fileOut << *cIt2 << "|";
	} else {
	  fileOut << *cIt2 << "\n";
	}
      }
    }
  }
  fileOut << ICATEGORIES_END << "\n";

  fileOut << ASSIGNEDICATEGORIES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator acIt;
  for (acIt = assignedIncidentAttributeCategories.begin(); acIt != assignedIncidentAttributeCategories.end(); acIt++) {
    std::vector<std::string> currentAssigned = *acIt;
    std::vector<std::string>::iterator acIt2;
    if (acIt != assignedIncidentAttributeCategories.end() - 1) {
      for (acIt2 = currentAssigned.begin(); acIt2 != currentAssigned.end(); acIt2++) {
	if (acIt2 != currentAssigned.end() - 1) {
	  fileOut << *acIt2 << "|";
	} else {
	  fileOut << *acIt2 << ";";
	}
      }
    } else {
      for (acIt2 = currentAssigned.begin(); acIt2 != currentAssigned.end(); acIt2++) {
	if (acIt2 != currentAssigned.end() - 1) {
	  fileOut << *acIt2 << "|";
	} else {
	  fileOut << *acIt2 << "\n";
	}
      }
    }
  }
  fileOut << ASSIGNEDICATEGORIES_END << "\n";

  fileOut << RELATIONSHIPS_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator rIt;
  for (rIt = relationships.begin(); rIt != relationships.end(); rIt++) {
    std::vector<std::string> currentRelationship = *rIt;
    std::vector<std::string>::iterator rIt2;
    if (rIt != relationships.end() - 1) {
      for (rIt2 = currentRelationship.begin(); rIt2 != currentRelationship.end(); rIt2++) {
	if (rIt2 != currentRelationship.end() - 1) {
	  fileOut << *rIt2 << "|";
	} else {
	  fileOut << *rIt2 << ";";
	}
      }
    } else {
      for (rIt2 = currentRelationship.begin(); rIt2 != currentRelationship.end(); rIt2++) {
	if (rIt2 != currentRelationship.end() - 1) {
	  fileOut << *rIt2 << "|";
	} else {
	  fileOut << *rIt2 << "\n";
	}
      }
    }
  }
  
  fileOut << RELATIONSHIPS_END << "\n";

  fileOut << ASSIGNEDRELATIONSHIPS_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator arIt;
  for (arIt = assignedRelationships.begin(); arIt != assignedRelationships.end(); arIt++) {
    std::vector<std::string> currentAssignedRelationship = *arIt;
    std::vector<std::string>::iterator arIt2;
    if (arIt != assignedRelationships.end() - 1) {
      for (arIt2 = currentAssignedRelationship.begin(); arIt2 != currentAssignedRelationship.end(); arIt2++) {
	if (arIt2 != currentAssignedRelationship.end() - 1) {
	  fileOut << *arIt2 << "|";
	} else {
	  fileOut << *arIt2 << ";";
	}
      }
    } else {
      for (arIt2 = currentAssignedRelationship.begin(); arIt2 != currentAssignedRelationship.end(); arIt2++) {
	if (arIt2 != currentAssignedRelationship.end() - 1) {
	  fileOut << *arIt2 << "|";
	} else {
	  fileOut << *arIt2 << "\n";
	}
      }
    }
  }
  
  fileOut << ASSIGNEDRELATIONSHIPS_END << "\n";
  
  fileOut << ENTITIES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator enIt;
  for (enIt = entities.begin(); enIt != entities.end(); enIt++) {
    std::vector<std::string> currentEntity = *enIt;
    std::vector<std::string>::iterator enIt2;
    if (enIt != entities.end() - 1) {
      for (enIt2 = currentEntity.begin(); enIt2 != currentEntity.end(); enIt2++) {
	if (enIt2 != currentEntity.end() - 1) {
	  fileOut << *enIt2 << "|";
	} else {
	  fileOut << *enIt2 << ";";
	}
      }
    } else {
      for (enIt2 = currentEntity.begin(); enIt2 != currentEntity.end(); enIt2++) {
	if (enIt2 != currentEntity.end() - 1) {
	  fileOut << *enIt2 << "|";
	} else {
	  fileOut << *enIt2 << "\n";
	}
      }
    }
  }
  fileOut << ENTITIES_END << "\n";
  
  fileOut << ENTITYATTRIBUTES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator ettrIt;
  for (ettrIt = entityAttributes.begin(); ettrIt != entityAttributes.end(); ettrIt++) {
    std::vector<std::string> currentAttribute = *ettrIt;
    std::vector<std::string>::iterator ettrIt2;
    if (ettrIt != entityAttributes.end() - 1) {
      for (ettrIt2 = currentAttribute.begin(); ettrIt2 != currentAttribute.end(); ettrIt2++) {
	if (ettrIt2 != currentAttribute.end() - 1) {
	  fileOut << *ettrIt2 << "|";
	} else {
	  fileOut << *ettrIt2 << ";";
	}
      }
    } else {
      for (ettrIt2 = currentAttribute.begin(); ettrIt2 != currentAttribute.end(); ettrIt2++) {
	if (ettrIt2 != currentAttribute.end() - 1) {
	  fileOut << *ettrIt2 << "|";
	} else {
	  fileOut << *ettrIt2 << "\n";
	}
      }
    }
  }
  fileOut << ENTITYATTRIBUTES_END << "\n";

  fileOut << ASSIGNEDENTITYATTRIBUTES_BEGIN << "\n";
  std::vector<std::vector <std::string> >::iterator eaIt;
  for (eaIt = assignedEntityAttributes.begin(); eaIt != assignedEntityAttributes.end(); eaIt++) {
    std::vector<std::string> currentPair = *eaIt;
    std::vector<std::string>::iterator eaIt2;
    if (eaIt != assignedEntityAttributes.end() - 1) {
      for (eaIt2 = currentPair.begin(); eaIt2 != currentPair.end(); eaIt2++) {
	if (eaIt2 != currentPair.end() - 1) {
	  fileOut << *eaIt2 << "|";
	} else {
	  fileOut << *eaIt2 << ";";
	}
      }
    } else {
      for (eaIt2 = currentPair.begin(); eaIt2 != currentPair.end(); eaIt2++) {
	if (eaIt2 != currentPair.end() - 1) {
	  fileOut << *eaIt2 << "|";
	} else {
	  fileOut << *eaIt2 << "\n";
	}
      }
    }
  }  
  fileOut << ASSIGNEDENTITYATTRIBUTES_END << "\n";

  fileOut << ENTITYVALUES_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator evIt;
  for (evIt = entityValues.begin(); evIt != entityValues.end(); evIt++) {
    std::vector<std::string> currentGroup = *evIt;
    std::vector<std::string>::iterator evIt2;
    if (evIt != entityValues.end() - 1) {
      for (evIt2 = currentGroup.begin(); evIt2 != currentGroup.end(); evIt2++) {
	if (evIt2 != currentGroup.end() - 1) {
	  fileOut << *evIt2 << "|";
	} else {
	  fileOut << *evIt2 << ";";
	}
      }
    } else {
      for (evIt2 = currentGroup.begin(); evIt2 != currentGroup.end(); evIt2++) {
	if (evIt2 != currentGroup.end() - 1) {
	  fileOut << *evIt2 << "|";
	} else {
	  fileOut << *evIt2 << "\n";
	}
      }
    }
  }
  fileOut << ENTITYVALUES_END << "\n";
    
  fileOut << ECATEGORIES_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator ecIt;
  for (ecIt = entityAttributeCategories.begin(); ecIt != entityAttributeCategories.end(); ecIt++) {
    std::vector<std::string> currentEntity = *ecIt;
    std::vector<std::string>::iterator ecIt2;
    if (ecIt != entityAttributeCategories.end() - 1) {
      for (ecIt2 = currentEntity.begin(); ecIt2 != currentEntity.end(); ecIt2++) {
	if (ecIt2 != currentEntity.end() - 1) {
	  fileOut << *ecIt2 << "|";
	} else {
	  fileOut << *ecIt2 << ";";
	}
      }
    } else {
      for (ecIt2 = currentEntity.begin(); ecIt2 != currentEntity.end(); ecIt2++) {
	if (ecIt2 != currentEntity.end() - 1) {
	  fileOut << *ecIt2 << "|";
	} else {
	  fileOut << *ecIt2 << "\n";
	}
      }
    }
  }
  fileOut << ECATEGORIES_END << "\n";

  fileOut << ASSIGNEDECATEGORIES_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator aeIt;
  for (aeIt = assignedEntityAttributeCategories.begin(); aeIt != assignedEntityAttributeCategories.end(); aeIt++) {
    std::vector<std::string> currentAssigned = *aeIt;
    std::vector<std::string>::iterator aeIt2;
    if (aeIt != assignedEntityAttributeCategories.end() - 1) {
      for (aeIt2 = currentAssigned.begin(); aeIt2 != currentAssigned.end(); aeIt2++) {
	if (aeIt2 != currentAssigned.end() - 1) {
	  fileOut << *aeIt2 << "|";
	} else {
	  fileOut << *aeIt2 << ";";
	}
      }
    } else {
      for (aeIt2 = currentAssigned.begin(); aeIt2 != currentAssigned.end(); aeIt2++) {
	if (aeIt2 != currentAssigned.end() - 1) {
	  fileOut << *aeIt2 << "|";
	} else {
	  fileOut << *aeIt2 << "\n";
	}
      }
    }
  }
  fileOut << ASSIGNEDECATEGORIES_END << "\n";

  fileOut << RELTYPES_BEGIN << "\n";
  std::vector <std::vector <std::string> >::iterator rtIt;
  for (rtIt = relationshipTypes.begin(); rtIt != relationshipTypes.end(); rtIt++) {
    std::vector<std::string> currentType = *rtIt;
    std::vector<std::string>::iterator rtIt2;
    if (rtIt != relationshipTypes.end() - 1) {
      for (rtIt2 = currentType.begin(); rtIt2 != currentType.end(); rtIt2++) {
	if (rtIt2 != currentType.end() - 1) {
	  fileOut << *rtIt2 << "|";
	} else {
	  fileOut << *rtIt2 << ";";
	}
      }
    } else {
      for (rtIt2 = currentType.begin(); rtIt2 != currentType.end(); rtIt2++) {
	if (rtIt2 != currentType.end() - 1) {
	  fileOut << *rtIt2 << "|";
	} else {
	  fileOut << *rtIt2 << "\n";
	}
      }
    }
  }
  fileOut << RELTYPES_END;
  // No newline needed because we have reached the end of the file.
}

void DataInterface::readSave(const QString &fileName) {
  const std::string loadFile = fileName.toStdString();
  
  // Let's first make sure that our current DataInterface is clean.
  header.clear();
  rowData.clear();
  memos.clear();
  relationMemos.clear();

  incidentAttributes.clear();
  assignedIncidentAttributes.clear();
  incidentAttributeCategories.clear();
  assignedIncidentAttributeCategories.clear();
  incidentValues.clear();
  relationships.clear();
  assignedRelationships.clear();
  entities.clear();
  entityAttributes.clear();
  assignedEntityAttributes.clear();
  entityValues.clear();
  entityAttributeCategories.clear();
  assignedEntityAttributeCategories.clear();
  relationshipTypes.clear();

  std::vector<bool>::iterator efIt;
  for (efIt = eventFlagIndex.begin(); efIt != eventFlagIndex.end(); efIt++) {
    *efIt = false;
  }
  
  // Then we make some strings that we need to store data temporarily.
  std::string eventIndex;
  std::string separator;

  // Set up an file instream for the input file.
  std::ifstream myFile (loadFile.c_str());
  
  // We use this to identify the field that we are in.
  enum FieldStatus {none, imported, index, sep, memosField, relationMemosField, eventFlagsField,
		    incidentAttributesField, incidentAttributesAssignedField, acategoriesField,
		    acategoriesAssignedField, incidentValuesField, relationsField, relationsAssignedField,
		    entitiesField, entityAttributesField, entityAttributesAssignedField, entityValuesField,
		    ecategoriesField, ecategoriesAssignedField, relationshipTypesField};
  FieldStatus field = none;

  /* 
     We use inRow to check whether or not we are in a row of imported data.
  */
  bool inRow = false;
  // We only need to import the headers once. this bool helps with that.
  bool headersImported = false;

  // We beed to declare a vector for holding row data here, for scope reasons.
  std::vector <std::string> currentRow;
 
  // Then we read all the lines of data in the input file.
  while(myFile) {
    // The buffer will hold one line of raw data temporarily, and we will do some processing on it.
    std::string buffer;
   
    if (!getline(myFile, buffer)) break;

    // First we check whether we are in a field and, if yes, which one.
    if (buffer == IMPORTED_BEGIN) {      
      field = imported;
    } else if (buffer == IMPORTED_END) {
      field = none;
    } else if (buffer == INDEX_BEGIN) {
      field = index;
    } else if (buffer == INDEX_END) {
      field = none;
    } else if (buffer == SEP_BEGIN) {
      field = sep;
    } else if (buffer == SEP_END) {
      field = none;
    } else if (buffer == MEMOS_BEGIN) {
      field = memosField;
    } else if (buffer == MEMOS_END) {
      field = none;
    } else if (buffer == RELATIONMEMOS_BEGIN) {
      field = relationMemosField;
    } else if (buffer == RELATIONMEMOS_END) {
      field = none;
    } else if (buffer == EVENTFLAGS_BEGIN) {
      field = eventFlagsField;
    } else if (buffer == EVENTFLAGS_END) {
      field = none;
    } else if (buffer == INCIDENTATTRIBUTES_BEGIN) {
      field = incidentAttributesField;
    } else if (buffer == INCIDENTATTRIBUTES_END) {
      field = none;
    } else if (buffer == ASSIGNEDINCIDENTATTRIBUTES_BEGIN) {
      field = incidentAttributesAssignedField;
    } else if (buffer == ASSIGNEDINCIDENTATTRIBUTES_END) {
      field = none;
    } else if (buffer == INCIDENTVALUES_BEGIN) {
      field = incidentValuesField;
    } else if (buffer == INCIDENTVALUES_END) {
      field = none;
    } else if (buffer == ICATEGORIES_BEGIN) {
      field = acategoriesField;
    } else if (buffer == ICATEGORIES_END) {
      field = none;
    } else if (buffer == ASSIGNEDICATEGORIES_BEGIN) {
      field = acategoriesAssignedField;
    } else if (buffer == ASSIGNEDICATEGORIES_END) {
      field = none;
    } else if (buffer == RELATIONSHIPS_BEGIN) {
      field = relationsField;
    } else if (buffer == RELATIONSHIPS_END) {
      field = none;
    } else if (buffer == ASSIGNEDRELATIONSHIPS_BEGIN) {
      field = relationsAssignedField;
    } else if (buffer == ASSIGNEDRELATIONSHIPS_END) {
      field = none;
    } else if (buffer == ENTITIES_BEGIN) {
      field = entitiesField;
    } else if (buffer == ENTITIES_END) {
      field = none;
    } else if (buffer == ENTITYATTRIBUTES_BEGIN) {
      field = entityAttributesField;
    } else if (buffer == ENTITYATTRIBUTES_END) {
      field = none;
    } else if (buffer == ASSIGNEDENTITYATTRIBUTES_BEGIN) {
      field = entityAttributesAssignedField;
    } else if (buffer == ASSIGNEDENTITYATTRIBUTES_END) {
      field = none;
    } else if (buffer == ENTITYVALUES_BEGIN) {
      field = entityValuesField;
    } else if (buffer == ENTITYVALUES_END) {
      field = none;
    } else if (buffer == ECATEGORIES_BEGIN) {
      field = ecategoriesField;
    } else if (buffer == ECATEGORIES_END) {
      field = none;
    } else if (buffer == ASSIGNEDECATEGORIES_BEGIN) {
      field = ecategoriesAssignedField;
    } else if (buffer == ASSIGNEDECATEGORIES_END) {
      field = none;
    } else if (buffer == RELTYPES_BEGIN) {
      field = relationshipTypesField;
    } else if (buffer == RELTYPES_END) {
      field = none;
    }

    // Then we do something, and what we do depends on the field we are in.
    switch(field) {
    case imported:
      {
	// First we check whether we are in a new row.
	if(buffer.substr(0, 11) == ROW_PART) {
	  inRow = true;
	}
	if (inRow && buffer[0] != '<') {
	  currentRow.push_back(buffer);
	}
	if (inRow && !headersImported && buffer[0] == '<' && buffer.substr(0,2) != "</" &&
	    buffer.substr(0, 11) != ROW_PART) {
	  std::string::size_type labelLength = 0;
	  for (std::string::size_type i = 1; i != buffer.length(); i++) {
	    if (buffer[i] == '>') {
	      labelLength = i - 1;
	      break;
	    }
	  }
	  std::string currentHeader = buffer.substr(1, labelLength);
	  header.push_back(currentHeader);
	}
	if (buffer == ROW_END){
	  inRow = false;
	  if (!headersImported) {
	    headersImported =  true; // The headers should have been imported after the first row.
	  }
	  // And we can also push back the data read so far to rowData.
	  rowData.push_back(currentRow);
	  currentRow.clear();
	}
	// We only need to import the headers once.
	break;
      }
    case index:
      {
	if (buffer != INDEX_BEGIN) eventIndex = buffer;
	break;
      }
    case sep:
      {
	if (buffer != SEP_BEGIN) separator = buffer;
	break;
      }
    case memosField:
      {
	if (buffer != MEMOS_BEGIN && buffer != MEMOS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempMemo;
	    std::string memoString;
	    if (!getline(stringStream, memoString, ';')) break;
	    std::istringstream memostream(memoString);
	    while (memostream) {
	      std::string indivItem;
	      if (!getline(memostream, indivItem, '|')) break;
	      tempMemo.push_back(indivItem);
	    }
	    memos.push_back(tempMemo);
	  }
	}
	break;
      }
    case relationMemosField:
      {
	if (buffer != RELATIONMEMOS_BEGIN && buffer != RELATIONMEMOS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempMemo;
	    std::string memoString;
	    if (!getline(stringStream, memoString, ';')) break;
	    std::istringstream memostream(memoString);
	    while (memostream) {
	      std::string indivItem;
	      if (!getline(memostream, indivItem, '|')) break;
	      tempMemo.push_back(indivItem);
	    }
	    relationMemos.push_back(tempMemo);
	  }
	}
	break;
      }
    case eventFlagsField:
      {
	if (buffer != EVENTFLAGS_BEGIN && buffer != EVENTFLAGS_END) {
	  std::istringstream stringStream(buffer);
	  while(stringStream) {
	    std::string s;
	    if (!getline(stringStream, s, ';')) break;
	    if (s == "1") {
	      eventFlagIndex.push_back(true);
	    } else if (s == "0") {
	      eventFlagIndex.push_back(false);
	    }
	  }
	}
	break;
      }
    case incidentAttributesField:
      {
	if (buffer != INCIDENTATTRIBUTES_BEGIN && buffer != INCIDENTATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempAttribute;
	    std::string incidentAttributesString;
	    if (!getline(stringStream, incidentAttributesString, ';')) break;
	    std::istringstream incidentAttributestream(incidentAttributesString);
	    while (incidentAttributestream) {
	      std::string indivAttribute;
	      if (!getline(incidentAttributestream, indivAttribute, '|')) break;
	      tempAttribute.push_back(indivAttribute);
	    }
	    incidentAttributes.push_back(tempAttribute);
	  }
	}
	break;
      }
    case incidentAttributesAssignedField:
      {
	if (buffer != ASSIGNEDINCIDENTATTRIBUTES_BEGIN && buffer != ASSIGNEDINCIDENTATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedIncidentAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case incidentValuesField:
      {
	if (buffer != INCIDENTVALUES_BEGIN && buffer != INCIDENTVALUES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    incidentValues.push_back(tempGroup);
	  }
	}
	break;
      }
    case acategoriesField:
      {
	if (buffer != ICATEGORIES_BEGIN && buffer != ICATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    incidentAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case acategoriesAssignedField:
      {
	if (buffer != ASSIGNEDICATEGORIES_BEGIN && buffer != ASSIGNEDICATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedIncidentAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case relationsField:
      {
	if (buffer != RELATIONSHIPS_BEGIN && buffer != RELATIONSHIPS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempRelationship;
	    std::string relationshipString;
	    if (!getline(stringStream, relationshipString, ';')) break;
	    std::istringstream relationshipStream(relationshipString);
	    while (relationshipStream) {
	      std::string indivItem;
	      if (!getline(relationshipStream, indivItem, '|')) break;
	      tempRelationship.push_back(indivItem);
	    }
	    relationships.push_back(tempRelationship);
	  }
	}
	break;
      }
    case relationsAssignedField:
      {
	if (buffer != ASSIGNEDRELATIONSHIPS_BEGIN && buffer != ASSIGNEDRELATIONSHIPS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempAssigned;
	    std::string assignedString;
	    if (!getline(stringStream, assignedString, ';')) break;
	    std::istringstream assignedStream(assignedString);
	    while (assignedStream) {
	      std::string indivItem;
	      if (!getline(assignedStream, indivItem, '|')) break;
	      tempAssigned.push_back(indivItem);
	    }
	    assignedRelationships.push_back(tempAssigned);
	  }
	}
	break;
      }
    case entitiesField:
      {
	if (buffer != ENTITIES_BEGIN && buffer != ENTITIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entities.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityAttributesField:
      {
	if (buffer != ENTITYATTRIBUTES_BEGIN && buffer != ENTITYATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityAttributesAssignedField:
      {
	if (buffer != ASSIGNEDENTITYATTRIBUTES_BEGIN && buffer != ASSIGNEDENTITYATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedEntityAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityValuesField:
      {
	if (buffer != ENTITYVALUES_BEGIN && buffer != ENTITYVALUES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityValues.push_back(tempGroup);
	  }
	}
	break;
      }
    case ecategoriesField:
      {
	if (buffer != ECATEGORIES_BEGIN && buffer != ECATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case ecategoriesAssignedField:
      {
	if (buffer != ASSIGNEDECATEGORIES_BEGIN && buffer != ASSIGNEDECATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedEntityAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case relationshipTypesField:
      {
	if (buffer != RELTYPES_BEGIN && buffer != RELTYPES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    relationshipTypes.push_back(tempGroup);
	  }
	}
	break;
      }
    case none:
      {
	break;
      }
    }
  }
  // We should check whether data has actually been read.
  if (header.empty() || rowData.empty()) {
      QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Could not load data</b>"));
    errorBox->setInformativeText("It appears there was a problem while importing data. Perhaps your save file has been corrupted.");
    errorBox->exec();
    return; 
  } else {
    QString eI = QString::fromStdString(eventIndex);
    QString qS = QString::fromStdString(separator);
    
    emit loadFinished(eI, qS); 
  }
}

// The following function updates codes from an older file.
void DataInterface::importCodes(const QString &fileName) {
  const std::string loadFile = fileName.toStdString();
  
  // We need to create some temporary vectors and clear the ones we will reuse.
  std::vector<std::string> tempHeader;
  std::vector <std::vector <std::string> > tempRowData;
  memos.clear();
  relationMemos.clear();
  incidentAttributes.clear();
  assignedIncidentAttributes.clear();
  incidentAttributeCategories.clear();
  assignedIncidentAttributeCategories.clear();
  incidentValues.clear();
  relationships.clear();
  assignedRelationships.clear();
  entities.clear();
  entityAttributes.clear();
  assignedEntityAttributes.clear();
  entityValues.clear();
  entityAttributeCategories.clear();
  assignedEntityAttributeCategories.clear();
  relationshipTypes.clear();

  std::vector<bool>::iterator efIt;
  for (efIt = eventFlagIndex.begin(); efIt != eventFlagIndex.end(); efIt++) {
    *efIt = false;
  }
  
  // Then we make some strings that we need to store data temporarily.
  std::string eventIndex;
  std::string separator;
  std::vector<std::string> tempMemos;
  std::vector<std::string> tempRelMemos;

  // Set up an file instream for the input file.
  std::ifstream myFile (loadFile.c_str());
  
  // We use this to identify the field that we are in.
  enum FieldStatus {none, imported, memosField, relationMemosField, eventFlagsField,
		    incidentAttributesField, incidentAttributesAssignedField, acategoriesField,
		    acategoriesAssignedField, incidentValuesField, relationsField, relationsAssignedField,
		    entitiesField, entityAttributesField, entityAttributesAssignedField, entityValuesField,
		    ecategoriesField, ecategoriesAssignedField, relationshipTypesField};
  FieldStatus field = none;

  /* 
     We use inRow to check whether or not we are in a row of imported data.
  */
  bool inRow = false;
  // We only need to import the headers once. this bool helps with that.
  bool headersImported = false;

  // We beed to declare a vector for holding row data here, for scope reasons.
  std::vector <std::string> currentRow;

  // We will eventually put the columns that match in the following vector.
  std::vector <std::string> matchColumns;

  // This vector stores the names of the columns we need to match.
  QVector<QString> Qmatch; 

  // A boolean to check if the user wants to mark new rows or not.
  bool markNew = false;
  
  // Then we read all the lines of data in the input file.
  while(myFile) {
    //The buffer will hold one line of raw data temporarily, and we will do some processing on it.
    std::string buffer;
   
    if (!getline(myFile, buffer)) break;

    // First we check whether we are in a field and, if yes, which one.
    if (buffer == IMPORTED_BEGIN) {      
      field = imported;
    } else if (buffer == IMPORTED_END) {
      field = none;
    } else if (buffer == MEMOS_BEGIN) {
      field = memosField;
    } else if (buffer == MEMOS_END) {
      field = none;
    } else if (buffer == RELATIONMEMOS_BEGIN) {
      field = relationMemosField;
    } else if (buffer == RELATIONMEMOS_END) {
      field = none;
    } else if (buffer == INCIDENTATTRIBUTES_BEGIN) {
      field = incidentAttributesField;
    } else if (buffer == INCIDENTATTRIBUTES_END) {
      field = none;
    } else if (buffer == ASSIGNEDINCIDENTATTRIBUTES_BEGIN) {
      field = incidentAttributesAssignedField;
    } else if (buffer == ASSIGNEDINCIDENTATTRIBUTES_END) {
      field = none;
    } else if (buffer == INCIDENTVALUES_BEGIN) {
      field = incidentValuesField;
    } else if (buffer == INCIDENTVALUES_END) {
      field = none;
    } else if (buffer == ICATEGORIES_BEGIN) {
      field = acategoriesField;
    } else if (buffer == ICATEGORIES_END) {
      field = none;
    } else if (buffer == ASSIGNEDICATEGORIES_BEGIN) {
      field = acategoriesAssignedField;
    } else if (buffer == ASSIGNEDICATEGORIES_END) {
      field = none;
    } else if (buffer == RELATIONSHIPS_BEGIN) {
      field = relationsField;
    } else if (buffer == RELATIONSHIPS_END) {
      field = none;
    } else if (buffer == ASSIGNEDRELATIONSHIPS_BEGIN) {
      field = relationsAssignedField;
    } else if (buffer == ASSIGNEDRELATIONSHIPS_END) {
      field = none;
    } else if (buffer == ENTITIES_BEGIN) {
      field = entitiesField;
    } else if (buffer == ENTITIES_END) {
      field = none;
    } else if (buffer == ENTITYATTRIBUTES_BEGIN) {
      field = entityAttributesField;
    } else if (buffer == ENTITYATTRIBUTES_END) {
      field = none;
    } else if (buffer == ASSIGNEDENTITYATTRIBUTES_BEGIN) {
      field = entityAttributesAssignedField;
    } else if (buffer == ASSIGNEDENTITYATTRIBUTES_END) {
      field = none;
    } else if (buffer == ENTITYVALUES_BEGIN) {
      field = entityValuesField;
    } else if (buffer == ENTITYVALUES_END) {
      field = none;
    } else if (buffer == ECATEGORIES_BEGIN) {
      field = ecategoriesField;
    } else if (buffer == ECATEGORIES_END) {
      field = none;
    } else if (buffer == ASSIGNEDECATEGORIES_BEGIN) {
      field = ecategoriesAssignedField;
    } else if (buffer == ASSIGNEDECATEGORIES_END) {
      field = none;
    } else if (buffer == RELTYPES_BEGIN) {
      field = relationshipTypesField;
    } else if (buffer == RELTYPES_END) {
      field = none;
    }

    // Then we do something, and what we do depends on the field we are in.
    switch(field) {
    case imported:
      {
	// First we check whether we are in a new row.
	if(buffer.substr(0, 11) == ROW_PART) {
	  inRow = true;
	}
	if (inRow && buffer[0] != '<') {
	  currentRow.push_back(buffer);
	}
	if (inRow && !headersImported && buffer[0] == '<' && buffer.substr(0,2) != "</" &&
	    buffer.substr(0, 11) != ROW_PART) {
	  std::string::size_type labelLength = 0;
	  for (std::string::size_type i = 1; i != buffer.length(); i++) {
	    if (buffer[i] == '>') {
	      labelLength = i - 1;
	      break;
	    }
	  }
	  std::string currentHeader = buffer.substr(1, labelLength);
	  tempHeader.push_back(currentHeader);
	}
	if (buffer == ROW_END){
	  inRow = false;
	  if (!headersImported) {
	    headersImported =  true; // The headers should have been imported after the first row.
	    std::vector <std::string>::iterator sIt;
	    std::vector <std::string>::iterator sIt2;
	    for (sIt = header.begin(); sIt != header.end(); sIt++) {
	      for (sIt2 = tempHeader.begin(); sIt2 != tempHeader.end(); sIt2++) {
		if (*sIt == *sIt2) {
		  QString temp = QString::fromUtf8((*sIt).c_str());
		  Qmatch.push_back(temp);
		}
	      }
	    }
	    // First we need the user to select which columns should match between the two files.
	    ImportCodesDialog *import = new ImportCodesDialog(0, Qmatch); // A dialog where these columns will be selected.
	    import->deleteLater(); // We need to be able to access this after closure.
	    import->exec(); // Open te dialog.
	
	    Qmatch = import->getColumnNames(); // Recover the column names.
	
	    markNew = import->getMarked(); //SHOULD REACTIVE THIS LATER(?)
	
	    delete import; // And now we can delete the dialog.

	    // Let us convert the column names to strings.
	    QVectorIterator<QString> cI(Qmatch);
	    cI.toFront();
	    while (cI.hasNext()) {
	      QString temp = cI.next();
	      matchColumns.push_back(temp.toStdString());
	    }

	    if (matchColumns.size() == 0) {
	      QPointer <QMessageBox> warningBox = new QMessageBox;
	      warningBox->addButton(QMessageBox::Ok);
	      warningBox->setIcon(QMessageBox::Warning);
	      warningBox->setText("WARNING: No columns were selected!");
	      warningBox->exec();
	      return;
	    }
	  }
	  // And we can also push back the data read so far to rowData.
	  tempRowData.push_back(currentRow);
	  currentRow.clear();
	}
	// We only need to import the headers once.
	
	// We first fill Qmatch with common column headers (those that are not common cannot be matched).
	break;
      }
    case memosField:
      {
	if (buffer != MEMOS_BEGIN && buffer != MEMOS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempMemo;
	    std::string memoString;
	    if (!getline(stringStream, memoString, ';')) break;
	    std::istringstream memostream(memoString);
	    while (memostream) {
	      std::string indivItem;
	      if (!getline(memostream, indivItem, '|')) break;
	      tempMemo.push_back(indivItem);
	    }
	    memos.push_back(tempMemo);
	  }
	}
	break;
      }
    case relationMemosField:
      {
	if (buffer != MEMOS_BEGIN && buffer != MEMOS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempMemo;
	    std::string memoString;
	    if (!getline(stringStream, memoString, ';')) break;
	    std::istringstream memostream(memoString);
	    while (memostream) {
	      std::string indivItem;
	      if (!getline(memostream, indivItem, '|')) break;
	      tempMemo.push_back(indivItem);
	    }
	    relationMemos.push_back(tempMemo);
	  }
	}
	break;
      }
    case eventFlagsField:
      {
	std::istringstream stringStream(buffer);
	while(stringStream) {
	  std::string s;
	  if (!getline(stringStream, s, ';')) break;
	  if (s == "1") {
	    eventFlagIndex.push_back(true);
	  } else if (s == "0") {
	    eventFlagIndex.push_back(false);
	  }
	}
	break;
      }
    case incidentAttributesField:
      {
	if (buffer != INCIDENTATTRIBUTES_BEGIN && buffer != INCIDENTATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempAttribute;
	    std::string incidentAttributesString;
	    if (!getline(stringStream, incidentAttributesString, ';')) break;
	    std::istringstream incidentAttributestream(incidentAttributesString);
	    while (incidentAttributestream) {
	      std::string indivAttribute;
	      if (!getline(incidentAttributestream, indivAttribute, '|')) break;
	      tempAttribute.push_back(indivAttribute);
	    }
	    incidentAttributes.push_back(tempAttribute);
	  }
	}
	break;
      }
    case incidentAttributesAssignedField:
      {
	if (buffer != ASSIGNEDINCIDENTATTRIBUTES_BEGIN && buffer != ASSIGNEDINCIDENTATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedIncidentAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case incidentValuesField:
      {
	if (buffer != INCIDENTVALUES_BEGIN && buffer != INCIDENTVALUES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    incidentValues.push_back(tempGroup);
	  }
	}
	break;
      }
    case acategoriesField:
      {
	if (buffer != ICATEGORIES_BEGIN && buffer != ICATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    incidentAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case acategoriesAssignedField:
      {
	if (buffer != ASSIGNEDICATEGORIES_BEGIN && buffer != ASSIGNEDICATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedIncidentAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case relationsField:
      {
	if (buffer != RELATIONSHIPS_BEGIN && buffer != RELATIONSHIPS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempRelationship;
	    std::string relationshipString;
	    if (!getline(stringStream, relationshipString, ';')) break;
	    std::istringstream relationshipStream(relationshipString);
	    while (relationshipStream) {
	      std::string indivItem;
	      if (!getline(relationshipStream, indivItem, '|')) break;
	      tempRelationship.push_back(indivItem);
	    }
	    relationships.push_back(tempRelationship);
	  }
	}
	break;
      }
    case relationsAssignedField:
      {
	if (buffer != ASSIGNEDRELATIONSHIPS_BEGIN && buffer != ASSIGNEDRELATIONSHIPS_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempAssigned;
	    std::string assignedString;
	    if (!getline(stringStream, assignedString, ';')) break;
	    std::istringstream assignedStream(assignedString);
	    while (assignedStream) {
	      std::string indivItem;
	      if (!getline(assignedStream, indivItem, '|')) break;
	      tempAssigned.push_back(indivItem);
	    }
	    assignedRelationships.push_back(tempAssigned);
	  }
	}
	break;
      }
    case entitiesField:
      {
	if (buffer != ENTITIES_BEGIN && buffer != ENTITIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entities.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityAttributesField:
      {
	if (buffer != ENTITYATTRIBUTES_BEGIN && buffer != ENTITYATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityAttributesAssignedField:
      {
	if (buffer != ASSIGNEDENTITYATTRIBUTES_BEGIN && buffer != ASSIGNEDENTITYATTRIBUTES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedEntityAttributes.push_back(tempGroup);
	  }
	}
	break;
      }
    case entityValuesField:
      {
	if (buffer != ENTITYVALUES_BEGIN && buffer != ENTITYVALUES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityValues.push_back(tempGroup);
	  }
	}
	break;
      }
    case ecategoriesField:
      {
	if (buffer != ECATEGORIES_BEGIN && buffer != ECATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    entityAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case ecategoriesAssignedField:
      {
	if (buffer != ASSIGNEDECATEGORIES_BEGIN && buffer != ASSIGNEDECATEGORIES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    assignedEntityAttributeCategories.push_back(tempGroup);
	  }
	}
	break;
      }
    case relationshipTypesField:
      {
	if (buffer != RELTYPES_BEGIN && buffer != RELTYPES_END) {
	  std::istringstream stringStream(buffer);
	  while (stringStream) {
	    std::vector<std::string> tempGroup;
	    std::string groupString;
	    if (!getline(stringStream, groupString, ';')) break;
	    std::istringstream groupStream(groupString);
	    while (groupStream) {
	      std::string indivItem;
	      if (!getline(groupStream, indivItem, '|')) break;
	      tempGroup.push_back(indivItem);
	    }
	    relationshipTypes.push_back(tempGroup);
	  }
	}
	break;
      }
    case none:
      {
	break;
      }
    }
  }

  // Now we compare the file we just loaded to the file that is being coded currently.
  
  std::vector <int> indexOriginal;
  std::vector <int> indexLoaded;
  int goal= 0;
  int counter = 0;
    
  for (std::vector<std::string>::size_type i = 0; i != matchColumns.size(); i++) {
    for (std::vector<std::string>::size_type j = 0; j != header.size(); j++) {
      if (header[j] == matchColumns[i]) {
	indexOriginal.push_back(j);
	goal++;
      }
    }
  }
  for (std::vector <std::string>::size_type i = 0; i != matchColumns.size(); i++) {
    for (std::vector <std::string>::size_type j = 0; j != tempHeader.size(); j++) {
      if (tempHeader[j] == matchColumns[i]) {
	indexLoaded.push_back(j);
      }
    }
  }
  bool rowFound = false;
  std::vector <std::vector <std::string> > eventPairs;
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != rowData.size(); i++) {
    for (std::vector <std::vector <std::string> >::size_type j = 0; j != tempRowData.size(); j++) {
      counter = 0;
      for (std::vector<int>::size_type k = 0; k != indexOriginal.size(); k++) {
	for (std::vector<int>::size_type l = 0; l != indexLoaded.size(); l++) {
	  if (rowData[i][indexOriginal[k]] == tempRowData[j][indexLoaded[l]]) {
	    counter++;
	  }
	}
      }
      if (counter == goal) {
	rowFound = true;
	std::stringstream ss;
	ss << j;
	std::string currentEvent = ss.str();
	ss.clear();
	ss.str(std::string());
	ss << i;
	std::string newEvent = ss.str();
	std::vector <std::string> tempPair;
	tempPair.push_back(currentEvent);
	tempPair.push_back(newEvent);
	eventPairs.push_back(tempPair);
      }
    }
    // NEW CODE
    if (rowFound == false) {
      std::stringstream ss;
      ss << i;
      std::string tempIndex = ss.str();
      // This one works.
      for (std::vector <std::vector <std::string> >::size_type f = 0; f != memos.size();) {
	if (memos[f][0] == tempIndex) {
	  memos.erase(memos.begin() + f);
	} else {
	  f++;
	}
      }
      std::vector <std::vector <std::string> >::iterator f;
      std::vector <std::string>::iterator g;
      for (f = assignedIncidentAttributes.begin(); f != assignedIncidentAttributes.end();) {
	for (g = f->begin() + 1; g != f->end();) {
	  if (*g == tempIndex) {
	    f->erase(g);
	  } else {
	    g++;
	  }
	}
	if (f->size() < 2) {
	  assignedIncidentAttributes.erase(f);
	} else {
	  f++;
	}
      }
      for (f = incidentValues.begin(); f != incidentValues.end();) {
	std::vector<std::string> currentValue = *f;
	if (*(f->begin() + 1) == tempIndex) {
	  incidentValues.erase(f);
	} else {
	  f++;
	}
      }
      for (f = assignedRelationships.begin(); f != assignedRelationships.end();) {
	for (g = f->begin() + 1; g != f->end();) {
	  if (*g == tempIndex) {
	    f->erase(g);
	  } else {
	    g++;
	  }
	}
	if (f->size() < 2) {
	  assignedRelationships.erase(f);
	} else {
	  f++;
	} 
      }
    }
    // END NEW CODE
    if (rowFound == false && markNew == true) {
      eventFlagIndex[i] = true;
    } else {
      rowFound = false;
    }
  }
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != memos.size(); i++) {
    std::vector<std::string> currentMemo = memos[i];
    for (std::vector <std::vector <std::string> >::size_type j = 0; j != eventPairs.size(); j++) {
      std::vector<std::string> currentPair = eventPairs[j];
      if (currentMemo[0] == currentPair[0]) {
	memos[i][0] = currentPair[1];
      }
    }
  }
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != assignedIncidentAttributes.size(); i++) {
    std::vector<std::string> currentAttribute = assignedIncidentAttributes[i];
    for (std::vector <std::vector <std::string> >::size_type j = 0; j != eventPairs.size(); j++) {
      std::vector<std::string> currentPair = eventPairs[j];
      for (std::vector<std::string>::size_type k = 1; k != currentAttribute.size(); k++) {
	if (currentAttribute[k] == currentPair[0]) {
	  assignedIncidentAttributes[i][k] = currentPair[1];
	}
      }
    }
  }
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != assignedRelationships.size(); i++) {
    std::vector<std::string> currentRelationship = assignedRelationships[i];
    for (std::vector <std::vector <std::string> >::size_type j = 0; j != eventPairs.size(); j++) {
      std::vector<std::string> currentPair = eventPairs[j];
      for (std::vector<std::string>::size_type k = 1; k != currentRelationship.size(); k++) {
	if (currentRelationship[k] == currentPair[0]) {
	  assignedRelationships[i][k] = currentPair[1];
	}
      }
    }
  }
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != incidentValues.size(); i++) {
    std::vector<std::string> currentValue = incidentValues[i];
    for (std::vector <std::vector <std::string> >::size_type j = 0; j != eventPairs.size(); j++) {
      std::vector<std::string> currentPair = eventPairs[j];
      if (currentValue[1] == currentPair[0]) {
	incidentValues[i][1] = currentPair[1];
      }
    }
  }
}
 
void DataInterface::exportData(QVector<QString> &properties, QVector<bool> &includes) {
  std::vector<std::string> exportProperties;
  QVectorIterator<QString> pI(properties);
  while (pI.hasNext()) {
    exportProperties.push_back(pI.next().toStdString());
  }

  bool incidentAttributesBool = false;
  bool incidentCategoriesBool = false;
  bool relationshipsBool = false;
  bool entityAttributesBool = false;
  bool entityCategoriesBool = false;

  std::vector<bool> includeBools;
  QVectorIterator<bool> nI(includes);
  while (nI.hasNext()) {
    if (nI.next() == true) {
      includeBools.push_back(true);
    } else {
      includeBools.push_back(false);
    }
  }
  if (includeBools[0] == true) {
    incidentAttributesBool = true;
  }
  if (includeBools[1] == true) {
    incidentCategoriesBool = true;
  }
  if (includeBools[2] == true) {
    relationshipsBool = true;
    if (includeBools[3] == true) {
      entityAttributesBool = true;
    }
    if (includeBools[4] == true) {
      entityCategoriesBool = true;
    }
  }
  QString path = QCoreApplication::applicationDirPath();
  path.append("/export/");
  if (!QDir(path).exists()) {
    QDir().mkdir(path);
  }
  // We first write the file with Incident Nodes.
  QString fileName = "Incidents_Nodes.csv";
  QString filePath = path;
  filePath.append(fileName);
  std::ofstream fileOut(filePath.toStdString().c_str());

  if (!fileOut.is_open()) {
    QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Cannot write file</b>"));
    errorBox->setInformativeText("It appears impossible to open the file to write data.");
    errorBox->exec();
    return;
  }

  // Now we write the headers of this output file.
  std::vector<std::vector<std::string>::size_type> index;

  for (std::vector<std::string>::size_type i = 0; i != exportProperties.size(); i++) {
    for (std::vector<std::string>::size_type j = 0; j != header.size(); j++) {
      if (exportProperties[i] == header[j]) {
	index.push_back(j);
      }
    }
  }

  if (index.size() == 0) {
    fileOut << "Id" << "," << "Label" << "\n";
  } else {
    fileOut << "Id" << "," << "Label" << ",";
    std::vector<std::string>::iterator it;
    for (it = exportProperties.begin(); it != exportProperties.end(); it++) {
      if (it != exportProperties.end() - 1) {
	fileOut << "\"" << *it << "\"" << ",";
      } else {
	fileOut << "\"" << *it << "\"" << "\n";
      }
    }
  }

  int counter = 1;
  std::vector <std::vector <std::string> >::iterator dIt;
  for (dIt = rowData.begin(); dIt != rowData.end(); dIt++) {
    std::vector<std::string> currentRow = *dIt;
    if (index.size() == 0) {
      fileOut << counter << "," << counter << "," << "\n";
    } else {
      fileOut << counter << "," << counter << ",";
      for (std::vector<std::vector<std::string>::size_type>::size_type i = 0;i != index.size(); i++) {
	if (i != index.size() - 1) {
	  fileOut << "\"" << currentRow[index[i]] << "\"" << ",";
	} else {
	  fileOut << "\"" << currentRow[index[i]] << "\"" << "\n";
	}
      }
    }
    counter++;
  }
  
  fileOut.close();
  if (incidentAttributesBool == true) {
    fileName  = "Incident_Attributes_Nodes.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream nodesOut(filePath.toStdString().c_str());
    fileName  = "Incident_Attributes_to_Incidents_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream edgesOut(filePath.toStdString().c_str());

    // Now we right the headers of this output file.
    nodesOut << "Id" << "," << "Label" << "," << "Description" << "," << "Type" << "\n";
    edgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "," << "Value" << "\n";
    std::vector <std::vector <std::string> >::iterator iaIt;
    for (iaIt = assignedIncidentAttributes.begin(); iaIt != assignedIncidentAttributes.end(); iaIt++) {
      std::vector<std::string> currentAttribute = *iaIt;
      std::vector <std::vector <std::string> >::iterator uiaIt;
      std::string desc;
      for (uiaIt = incidentAttributes.begin(); uiaIt != incidentAttributes.end(); uiaIt++) {
	std::vector<std::string> currentItem = *uiaIt;
	if (currentItem[0] == currentAttribute[0]) {
	  desc = currentItem[1];
	  break;
	}
      }
      nodesOut << currentAttribute[0] << "," << currentAttribute[0] << "," << "\"" << desc << "\"" << "," << "Attribute" << "\n";
      std::vector<std::string>::iterator eIt;
      for (eIt = currentAttribute.begin() + 1; eIt != currentAttribute.end(); eIt++) {
	std::vector<std::vector <std::string> >::size_type currentEvent;
	std::istringstream ss(*eIt);
	ss >> currentEvent;
	std::vector <std::vector <std::string> >::iterator vIt;
	std::string currentValue;
	for (vIt = incidentValues.begin(); vIt != incidentValues.end(); vIt++) {
	  std::vector<std::string> currentGroup = *vIt;
	  if (currentGroup[0] == currentAttribute[0] && currentGroup[1] == *eIt) {
	    currentValue = currentGroup[2];
	    break;
	  }
	}
	edgesOut << currentAttribute[0] << "," << currentEvent + 1 << "," << "Directed" << ","
		 << "IS_ATTRIBUTE_OF" << "," <<  "\"" << currentValue << "\"" << "\n";
      }
    }
    nodesOut.close();
    edgesOut.close();

    fileName  = "Incident_Attributes_Matrix.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream matrixOut(filePath.toStdString().c_str());
    std::vector <std::vector <std::string> >::iterator attIt;
    matrixOut << ","; // First we have an empty cell for the matrix
    // Then we print the column names, which are the names of all attributes.
    for (attIt = assignedIncidentAttributes.begin(); attIt != assignedIncidentAttributes.end();attIt++) {
      if (attIt != assignedIncidentAttributes.end() - 1) {
	matrixOut << *attIt->begin() << ",";
      } else {
	matrixOut << *attIt->begin() << "\n";
      }
    }
    // Then we write each row.
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != rowData.size(); i++) {
      // We output the current incident id.
      matrixOut << i + 1 << ",";
      for (attIt = assignedIncidentAttributes.begin(); attIt != assignedIncidentAttributes.end(); attIt++) {
	std::string value = "0";
	std::vector <std::string>::iterator attIt2;
	for (attIt2 = attIt->begin() + 1; attIt2 != attIt->end(); attIt2++) {
	  std::vector <std::vector <std::string> >::size_type currentIncident = 0;
	  std::istringstream ss(*attIt2);
	  ss >> currentIncident;
	  if (currentIncident == i) {
	  	    value = "1";
	    // Now we need to check if we want to assign a value or just a 1;
	    std::vector <std::vector <std::string> >::iterator valIt;
	    for (valIt = incidentValues.begin(); valIt != incidentValues.end(); valIt++) {
	      if (*valIt->begin() == *attIt->begin() && *(valIt->begin() + 1) == *attIt2) {
		value = *(valIt->begin() + 2);
	      }
	    }
	  }
	}
	if (attIt != assignedIncidentAttributes.end() - 1) {
	  matrixOut << "\"" << value << "\"" << ",";
	} else {
	  matrixOut << "\"" << value << "\"" << "\n";
	} 
      }
    }
    matrixOut.close();
    
    if (incidentCategoriesBool == true) {
      fileName  = "Incident_Categories_Nodes.csv";
      filePath = path;
      filePath.append(fileName);
      std::ofstream nodesOut(filePath.toStdString().c_str());
      fileName = "Incident_Attributes_to_Categories_Edges.csv";
      filePath = path;
      filePath.append(fileName);
      std::ofstream edgesOut(filePath.toStdString().c_str());
      
      nodesOut << "Id" << "," << "Label" << "," << "Description" << "," << "Type" << "\n";
      edgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "\n";
      std::vector <std::vector <std::string> >::iterator icIt;
      for (icIt = assignedIncidentAttributeCategories.begin(); icIt != assignedIncidentAttributeCategories.end(); icIt++) {
	std::vector<std::string> currentCategory = *icIt;
	std::vector <std::vector <std::string> >::iterator uicIt;
	std::string desc;
	for (uicIt = incidentAttributeCategories.begin(); uicIt != incidentAttributeCategories.end(); uicIt++) {
	  std::vector<std::string> currentItem = *uicIt;
	  if (currentItem[0] == currentCategory[0]) {
	    desc = currentItem[1];
	    break;
	  }
	}
	nodesOut << currentCategory[0] << "," << currentCategory[0] << "," << "\"" << desc << "\"" << "," << "Category" << "\n";
	std::vector<std::string>::iterator caIt;
	for (caIt = currentCategory.begin() + 1; caIt != currentCategory.end(); caIt++) {
	  edgesOut << *caIt << "," << currentCategory[0]  << "," << "Directed" << "," << "IS_IN_CATEGORY" << "\n";
	}
      }
      nodesOut.close();
      edgesOut.close();
    }
  }
  if (relationshipsBool == true) {
    fileName = "Entities_Nodes.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream relNodesOut(filePath.toStdString().c_str());
    fileName = "Entities_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream relEdgesOut(filePath.toStdString().c_str());
    fileName = "Relationships_Nodes.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream relIndNodesOut(filePath.toStdString().c_str());
    fileName = "Incidents_to_Relationships_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream relIndEdgesOut(filePath.toStdString().c_str());
    fileName = "Entities_to_Relationships_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream relEntEdgesOut(filePath.toStdString().c_str());
    
    relNodesOut << "Id" << "," << "Label" << "," << "Description" << "," << "Type" << "\n";
    relEdgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "," << "Description" << "\n";
    
    relIndNodesOut << "Id" << "," << "Label" << "," << "Memo" << "," << "Type" << "\n";
    relIndEdgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "\n";
    
    relEntEdgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "\n";

    std::vector<std::vector <std::string> >tempEntities;
    std::vector<std::vector <std::string> >::iterator rIt;
    for (rIt = assignedRelationships.begin(); rIt != assignedRelationships.end(); rIt++) {
      std::vector<std::string> currentAssigned = *rIt;
      std::string currentMemo;
      for (std::vector<std::vector <std::string> >::size_type i = 0; i != relationMemos.size(); i++) {
	std::vector<std::string> currentRelMemo = relationMemos[i];
	if (currentRelMemo[0] == currentAssigned[0]) {
	  currentMemo = currentRelMemo[1];
	}
      }
      relIndNodesOut  << currentAssigned[0] << "," << currentAssigned[0] << "," << "\""
		      << currentMemo << "\"" << "," << "Relationship" << "\n";
      std::vector<std::string>::iterator arIt;
      for (arIt = currentAssigned.begin() + 1; arIt != currentAssigned.end(); arIt++) {
	std::vector<std::vector <std::string> >::size_type currentEvent;
	std::istringstream ss(*arIt);
	ss >> currentEvent;
	relIndEdgesOut << currentEvent + 1 << "," << currentAssigned[0] << "," << "Directed"
		       << "," << "IS_INDICATOR_OF" << "\n";
      }
      
      std::vector<std::vector <std::string> >::iterator rIt2;
      for (rIt2 = relationships.begin(); rIt2 != relationships.end(); rIt2++) {
	std::vector<std::string> currentItem = *rIt2;
	if (currentItem[0] == currentAssigned[0]) {
	  std::vector<std::string> currentRelationship = currentItem;
	  relEntEdgesOut << currentItem[1] << "," << currentItem[0] << "," << "Directed" << "," << "IS_IN_RELATIONSHIP" << "\n";
	  relEntEdgesOut << currentItem[3] << "," << currentItem[0] << "," << "Directed" << "," << "IS_IN_RELATIONSHIP" << "\n";
	  std::string desc;
	  std::vector <std::vector <std::string> >::iterator aeIt;
	  for (aeIt = entities.begin(); aeIt != entities.end(); aeIt++) {
	    std::vector<std::string> currentEntity = *aeIt;
	    if (currentEntity[0] == currentRelationship[1]) {
	      desc = currentEntity[1];
	    }
	  }
	  std::vector<std::string> tempEntity;
	  tempEntity.push_back(currentRelationship[1]);
	  tempEntity.push_back(desc);
	  tempEntities.push_back(tempEntity);
	  tempEntity.clear();
	  
	  for (aeIt = entities.begin(); aeIt != entities.end(); aeIt++) {
	    std::vector<std::string> currentEntity = *aeIt;
	    if (currentEntity[0] == currentRelationship[3]) {
	      desc = currentEntity[1];
	    }
	  }
	  tempEntity.push_back(currentRelationship[3]);
	  tempEntity.push_back(desc);
	  tempEntities.push_back(tempEntity);
	  tempEntity.clear();
	  
	  std::vector <std::vector <std::string> >::iterator rtIt;
	  std::string direction;
	  std::string relLabel;
	  std::string descRel;
	  for (rtIt = relationshipTypes.begin(); rtIt != relationshipTypes.end(); rtIt++) {
	    std::vector<std::string> currentType = *rtIt;
	    if (currentType[0] == currentRelationship[2]) {
	      direction = currentType[2];
	      relLabel = currentType[0];
	      descRel = currentType[1];
	    }
	  }
	  relEdgesOut << "\"" << currentRelationship[1] << "\"" << "," << "\"" << currentRelationship[3] << "\"" << ","
		      << "\"" << direction << "\"" << "," << relLabel << "," "\"" << descRel << "\"" << "\n";
	}
      }
    }
    sort(tempEntities.begin(), tempEntities.end());
    tempEntities.erase(unique(tempEntities.begin(), tempEntities.end()), tempEntities.end());
    std::vector <std::vector <std::string> >::iterator teIt;
    for (teIt = tempEntities.begin(); teIt != tempEntities.end(); teIt++) {
      std::vector<std::string> currentTemp = *teIt;
      relNodesOut << "\"" << currentTemp[0] << "\"" << "," << "\"" << currentTemp[0] << "\"" << ","
		  << "\"" << currentTemp[1] << "\"" << "," << "Entity" << "\n";
    }
    relNodesOut.close();
    relEdgesOut.close();
    relIndNodesOut.close();
    relIndEdgesOut.close();
    relEntEdgesOut.close();
  }
  if (entityAttributesBool == true) {
    fileName = "Entity_Attributes_Nodes.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream eaNodesOut(filePath.toStdString().c_str());
    fileName = "Entity_Attributes_to_Entities_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream eaEdgesOut(filePath.toStdString().c_str());

    eaNodesOut << "Id" << "," << "Label" << "," << "Description" << "," << "Type" << "\n";
    eaEdgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "\n";    
    
    std::vector<std::vector <std::string> >::iterator aIt;
    for (aIt = assignedEntityAttributes.begin(); aIt != assignedEntityAttributes.end(); aIt++) {
      std::vector<std::string> currentAttribute = *aIt;
      std::vector<std::vector <std::string> >::iterator aIt2;
      std::string attDesc;
      for (aIt2 = entityAttributes.begin(); aIt2 != entityAttributes.end(); aIt2++) {
	std::vector<std::string> currentAtt = *aIt2;
	if (currentAtt[0] == currentAttribute[0]) {
	  attDesc = currentAtt[1];
	}
      }
      eaNodesOut << currentAttribute[0] << "," << currentAttribute[0] << "," << "\"" << attDesc << "\"" << "," << "Attribute" << "\n";
      std::vector<std::string>::iterator aIt3;
      for (aIt3 = currentAttribute.begin() + 1; aIt3 != currentAttribute.end(); aIt3++) {
	eaEdgesOut << currentAttribute[0] << "," << *aIt3 << "," << "Directed" << "," << "IS_ATTRIBUTE_OF" << "\n";
      }
    }
    eaNodesOut.close();
    eaEdgesOut.close();

    fileName  = "Entity_Attributes_Matrix.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream matrixOut(filePath.toStdString().c_str());
    std::vector <std::vector <std::string> >::iterator attIt;
    matrixOut << ","; // First we have an empty cell for the matrix
    // Then we print the column names, which are the names of all attributes.
    for (attIt = assignedEntityAttributes.begin(); attIt != assignedEntityAttributes.end();attIt++) {
      if (attIt != assignedEntityAttributes.end() - 1) {
	matrixOut << *attIt->begin() << ",";
      } else {
	matrixOut << *attIt->begin() << "\n";
      }
    }
    // Then we write each row.
    std::vector <std::vector <std::string> >::iterator entIt;
    for (entIt = entities.begin(); entIt != entities.end(); entIt++) {
      bool found = false;
      std::vector <std::vector <std::string> >::iterator relIt;
      for (relIt = assignedRelationships.begin(); relIt != assignedRelationships.end(); relIt++) {
	// First we do entity one in this relationship
	std::vector <std::vector <std::string> >::iterator relIt2;
	for (relIt2 = relationships.begin(); relIt2 != relationships.end(); relIt2++) {
	  if (*relIt->begin() == *relIt2->begin()) {
	    if ((*entIt->begin() ==  *(relIt2->begin() + 1) || *entIt->begin() == *(relIt2->begin() + 3)) && found == false) {
	      found = true;
	      matrixOut << *entIt->begin() << ",";
	      for (attIt = assignedEntityAttributes.begin(); attIt != assignedEntityAttributes.end(); attIt++) {
		std::string value = "0";
		std::vector <std::string>::iterator attIt2;
		for (attIt2 = attIt->begin() + 1; attIt2 != attIt->end(); attIt2++) {
		  if (*attIt2 == *entIt->begin()) {
		    value = "1";
		    // Now we need to check if we want to assign a value or just a 1;
		    std::vector <std::vector <std::string> >::iterator valIt;
		    for (valIt = entityValues.begin(); valIt != entityValues.end(); valIt++) {
		      if (*valIt->begin() == *attIt->begin() && *(valIt->begin() + 1) == *attIt2) {
			value = *(valIt->begin() + 2);
		      }
		    }
		  }
		}
		if (attIt != assignedEntityAttributes.end() - 1) {
		  matrixOut << "\"" << value << "\"" << ",";
		} else {
		  matrixOut << "\"" << value << "\"" << "\n";
		} 
	      }
	    }
	  }
	}
      }
    }
    matrixOut.close();
  }
  
  if (entityCategoriesBool == true) {
    fileName = "Entity_Categories_Nodes.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream ecNodesOut(filePath.toStdString().c_str());
    fileName = "Entity_Attributes_to_Categories_Edges.csv";
    filePath = path;
    filePath.append(fileName);
    std::ofstream ecEdgesOut(filePath.toStdString().c_str());
    
    ecNodesOut << "Id" << "," << "Label" << "," << "Description" << "," << "Type" << "\n";
    ecEdgesOut << "Source" << "," << "Target" << "," << "Type" << "," << "Label" << "\n";

    std::vector <std::vector <std::string> >::iterator ecIt;
    for (ecIt = assignedEntityAttributeCategories.begin(); ecIt != assignedEntityAttributeCategories.end(); ecIt++) {
      std::vector<std::string> currentCategory = *ecIt;
      std::string catDesc;
      std::vector<std::vector <std::string> >::iterator ecIt2;
      for (ecIt2 = entityAttributeCategories.begin(); ecIt2 != entityAttributeCategories.end(); ecIt2++) {
	std::vector<std::string> currentCat = *ecIt2;
	if (currentCategory[0] == currentCat[0]) {
	  catDesc = currentCat[1];
	}
      }
      std::vector<std::string>::iterator ecIt3;
      for (ecIt3 = currentCategory.begin() + 1; ecIt3 != currentCategory.end(); ecIt3++) {
	ecEdgesOut << *ecIt3 << "," << currentCategory[0] << "," << "Directed" << "," << "IS_IN_CATEGORY" << "\n";
      }
      ecNodesOut << currentCategory[0] << "," << currentCategory[0] << "," << "\"" << catDesc << "\"" << "," << "Category" << "\n";
    }
    ecNodesOut.close();
    ecEdgesOut.close();
  }
  QPointer<QMessageBox> errorBox = new QMessageBox;
  errorBox->setText(tr("<b>Files exported</b>"));
  errorBox->setInformativeText("Your files have been exported to the \"../export\" folder in the program's directory.");
  errorBox->exec();
}




