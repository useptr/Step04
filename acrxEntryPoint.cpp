// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "Tchar.h" 
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep04App : public AcRxArxApp {
private:
	static const TCHAR* m_employeeDictionaryName;
public:
	CStep04App() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}

	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CStep04App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.

	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	static void AsdkStep04_ADDENTRY(void)
	{
		// Prompt the user for an employee name 
		TCHAR employeeName[128];
		if (acedGetString(0, _T("Enter employee name: "), employeeName) != RTNORM)
			return;
		// Get the Named Objects Dictionary from the current working database 
		AcDbDictionary* pNamedObjectsDictionary{ nullptr };
		if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObjectsDictionary) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: Unable to get the Named Objects Dictionary"));
			return;
		}
		// pNamedObjectsDictionary succesfully opened
		AcDbDictionary* pEmployeeDictionary{ nullptr };
		// Check if the "ASDK_EMPLOYEE_DICTIONARY" is already in the NOD 
		// getAt return Acad::eOk, Acad::eInvalidKey, Acad::eKeyNotFound 
		if (Acad::eKeyNotFound == pNamedObjectsDictionary->getAt(m_employeeDictionaryName, pEmployeeDictionary)) {
			if (pNamedObjectsDictionary->upgradeOpen() != Acad::eOk) {
				acutPrintf(_T("\nERROR: Cannot write to the Named Objects Dictionary"));
				pNamedObjectsDictionary->close();
				return;
			}
			// TODO put dictionary cretion in a seperate function
			pEmployeeDictionary = new AcDbDictionary;
			if (pNamedObjectsDictionary->setAt(m_employeeDictionaryName, pEmployeeDictionary, pEmployeeDictionary->objectId()) != Acad::eOk) {
				acutPrintf(_T("\nCannot add %s dictionary in the Named Objects Dictionary"), m_employeeDictionaryName);
				delete pEmployeeDictionary;
				pNamedObjectsDictionary->close();
				return;
			}
		}
		if (nullptr == pEmployeeDictionary) {
			acutPrintf(_T("\nERROR: Cannot open %s"), m_employeeDictionaryName);
			pNamedObjectsDictionary->close();
			return;
		}
		// pEmployeeDictionary succesfully opened
		pNamedObjectsDictionary->close();
		// Check if the name of the employee is already in the "ASDK_EMPLOYEE_DICTIONARY" dictionary
		AcDbObjectId objectId; // TODO kNULL
		if (pEmployeeDictionary->getAt(employeeName, objectId) == Acad::eOk) {
			acutPrintf(_T("\nERROR: This employee is already registered"));
			pEmployeeDictionary->close();
			return;
		}
		// If the employee dictionary is not present, then create a new AcDbXrecord and add it to the "ASDK_EMPLOYEE_DICTIONARY" 
		if (!pEmployeeDictionary->isWriteEnabled() && pEmployeeDictionary->upgradeOpen() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot opened %s to write"), m_employeeDictionaryName);
			pEmployeeDictionary->close();
			return;
		}
		AcDbXrecord* pEmployeeEntry = new AcDbXrecord;
		if (pEmployeeDictionary->setAt(employeeName, pEmployeeEntry, pEmployeeEntry->objectId()) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Failed to add employee %s to %s"), employeeName, m_employeeDictionaryName);
			delete pEmployeeEntry;
			pEmployeeDictionary->close();
			return;
		}
		acutPrintf(_T("\nemployee %s successfully added to %s"), employeeName, m_employeeDictionaryName);
		pEmployeeEntry->close();
		pEmployeeDictionary->close();
	}

	static void AsdkStep04_LISTENTRIES(void) {
		AcDbDictionary* pNamedObjectsDictionary;
		// Get the Named Objects Dictionary from the current working database 
		if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObjectsDictionary) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Unable to get the Named Objects Dictionary"));
			return;
		}
		// pNamedObjectsDictionary succesfully opened 
		AcDbDictionary* pEmployeeDictionary;
		// Get the "ASDK_EMPLOYEE_DICTIONARY" dictionary 
		if (pNamedObjectsDictionary->getAt(m_employeeDictionaryName, pEmployeeDictionary) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: %s not found"), m_employeeDictionaryName);
			pNamedObjectsDictionary->close();
			return;
		}
		// pEmployeeDictionary succesfully opened 
		pNamedObjectsDictionary->close();
		// Create a new iterator
		AcDbDictionaryIterator* pIter{ pEmployeeDictionary->newIterator() };
		if (pIter == nullptr) {
			acutPrintf(_T("\nERROR: Cannot create AcDbDictionaryIterator"));
			pEmployeeDictionary->close();
			return;
		}
		for (; !pIter->done(); pIter->next()) // Iterate through the ASDK_EMPLOYEE_DICTIONARY
			acutPrintf(_T("\nEmployee name: %s"), pIter->name()); // Print the dictionary key 
		delete pIter;
		pEmployeeDictionary->close();
	}

	static void AsdkStep04_REMOVEENTRY(void) {
		// Get an employee name from the user 
		TCHAR employeeName[128];
		if (acedGetString(NULL, _T("\nEnter employee name: "), employeeName) != RTNORM)
			return;
		// Get the Named Objects Dictionary from the current working database 
		AcDbDictionary* pNamedObjectsDictionary;
		if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNamedObjectsDictionary) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: Unable to get the Named Objects Dictionary"));
			return;
		}
		// pNamedObjectsDictionary successfully opened
		// Get the "ASDK_EMPLOYEE_DICTIONARY" dictionary
		AcDbDictionary* pEmployeeDictionary;
		if (pNamedObjectsDictionary->getAt(m_employeeDictionaryName, pEmployeeDictionary) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: %s not found"), m_employeeDictionaryName);
			pNamedObjectsDictionary->close();
			return;
		}
		pNamedObjectsDictionary->close();
		// pEmployeeDictionary successfully opened
		AcDbXrecord* pXrecord;
		if (pEmployeeDictionary->getAt(employeeName, pXrecord, AcDb::kForWrite) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Employee %s not found"), employeeName); // Acad::eKeyNotFound or Invalid name
			pEmployeeDictionary->close();
			return;
		}
		// pXrecord successfully opened
		pEmployeeDictionary->close();
		if (pXrecord->erase() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Unable to erase employee %s"), employeeName);
			pXrecord->close();
			return;
		}
		acutPrintf(_T("\nEmployee %s removed successfully"), employeeName);
		pXrecord->close();

	}
};

const TCHAR* CStep04App::m_employeeDictionaryName = _T("ASDK_EMPLOYEE_DICTIONARY");

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep04App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _LISTENTRIES, LISTENTRIES, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _ADDENTRY, ADDENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _REMOVEENTRY, REMOVEENTRY, ACRX_CMD_TRANSPARENT, NULL)

