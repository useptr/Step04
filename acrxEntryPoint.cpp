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
	static const TCHAR* g_szEmployeeDictionaryName;
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

	static void AsdkStep04_ADDENTRY(void)
	{
		// Prompt the user for an employee name 
		TCHAR szEmployeeName[128];
		if (acedGetString(0, _T("Enter employee name: "), szEmployeeName) != RTNORM)
			return;
		// Get the Named Objects Dictionary from the current working database 
		AcDbDictionaryPointer pNamedObjectsDictionary(acdbHostApplicationServices()->workingDatabase()->namedObjectsDictionaryId());
		if (pNamedObjectsDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open the Named Objects Dictionary"));
			return;
		}
		AcDbDictionaryPointer pEmployeeDictionary;
		// Check if the "ASDK_EMPLOYEE_DICTIONARY" is already in the NOD 
		AcDbObjectId employeeDictionaryId{ AcDbObjectId::kNull };
		if (Acad::eKeyNotFound == pNamedObjectsDictionary->getAt(g_szEmployeeDictionaryName, employeeDictionaryId)) {
			if (pNamedObjectsDictionary->upgradeOpen() != Acad::eOk) {
				acutPrintf(_T("\nERROR: Cannot write to the Named Objects Dictionary"));
				return;
			}
			pEmployeeDictionary.create();
			if (pNamedObjectsDictionary->setAt(g_szEmployeeDictionaryName, pEmployeeDictionary.object(), pEmployeeDictionary->objectId()) != Acad::eOk) {
				acutPrintf(_T("\nCannot add %s dictionary in the Named Objects Dictionary"), g_szEmployeeDictionaryName);
				return;
			}
		} else {
			pEmployeeDictionary.open(employeeDictionaryId);
		}
		if (pEmployeeDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open %s"), g_szEmployeeDictionaryName);
			return;
		}
		// Check if the name of the employee is already in the "ASDK_EMPLOYEE_DICTIONARY" dictionary
		AcDbObjectId objectId{ AcDbObjectId::kNull };
		if (pEmployeeDictionary->getAt(szEmployeeName, objectId) == Acad::eOk) {
			acutPrintf(_T("\nERROR: This employee is already registered"));
			return;
		}
		// If the employee dictionary is not present, then create a new AcDbXrecord and add it to the "ASDK_EMPLOYEE_DICTIONARY" 
		if (!pEmployeeDictionary->isWriteEnabled() && pEmployeeDictionary->upgradeOpen() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot opened %s to write"), g_szEmployeeDictionaryName);
			return;
		}
		AcDbObjectPointer<AcDbXrecord> pEmployeeEntry;
		pEmployeeEntry.create();
		if (pEmployeeDictionary->setAt(szEmployeeName, pEmployeeEntry.object(), pEmployeeEntry->objectId()) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Failed to add employee %s to %s"), szEmployeeName, g_szEmployeeDictionaryName);
			return;
		}
		acutPrintf(_T("\nemployee %s successfully added to %s"), szEmployeeName, g_szEmployeeDictionaryName);
	}

	static void AsdkStep04_LISTENTRIES(void) {
		// Get the Named Objects Dictionary from the current working database 
		AcDbDictionaryPointer pNamedObjectsDictionary(acdbHostApplicationServices()->workingDatabase()->namedObjectsDictionaryId());
		if (pNamedObjectsDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open the Named Objects Dictionary"));
			return;
		}
		AcDbObjectId employeeDictionaryId;
		// Get the "ASDK_EMPLOYEE_DICTIONARY" dictionary 
		if (pNamedObjectsDictionary->getAt(g_szEmployeeDictionaryName, employeeDictionaryId) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: '%s' not found"), g_szEmployeeDictionaryName);
			pNamedObjectsDictionary->close();
			return;
		}
		AcDbDictionaryPointer pEmployeeDictionary(employeeDictionaryId);
		if (pEmployeeDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open %s"), g_szEmployeeDictionaryName);
			return;
		}
		// Create a new iterator
		std::unique_ptr <AcDbDictionaryIterator> pIter(pEmployeeDictionary->newIterator());
		if (pIter == nullptr) {
			acutPrintf(_T("\nERROR: Cannot create AcDbDictionaryIterator"));
			return;
		}
		for (; !pIter->done(); pIter->next()) // Iterate through the ASDK_EMPLOYEE_DICTIONARY
			acutPrintf(_T("\nEmployee name: %s"), pIter->name()); // Print the dictionary key 
	}

	static void AsdkStep04_REMOVEENTRY(void) {
		// Get an employee name from the user 
		TCHAR szEmployeeName[128];
		if (acedGetString(NULL, _T("\nEnter employee name: "), szEmployeeName) != RTNORM)
			return;
		// Get the Named Objects Dictionary from the current working database 
		AcDbDictionaryPointer pNamedObjectsDictionary(acdbHostApplicationServices()->workingDatabase()->namedObjectsDictionaryId());
		if (pNamedObjectsDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open the Named Objects Dictionary"));
			return;
		}
		// Get the "ASDK_EMPLOYEE_DICTIONARY" dictionary
		AcDbObjectId employeeDictionaryId{ AcDbObjectId::kNull };
		if (pNamedObjectsDictionary->getAt(g_szEmployeeDictionaryName, employeeDictionaryId) != Acad::eOk)
		{
			acutPrintf(_T("\nERROR: %s not found"), g_szEmployeeDictionaryName);
			return;
		}
		AcDbDictionaryPointer pEmployeeDictionary(employeeDictionaryId);
		if (pEmployeeDictionary.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open %s"), g_szEmployeeDictionaryName);
			return;
		}
		
		AcDbObjectId employeeEntryId{ AcDbObjectId::kNull };
		if (pEmployeeDictionary->getAt(szEmployeeName, employeeEntryId) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Employee %s not found"), szEmployeeName); // Acad::eKeyNotFound or Invalid name
			pEmployeeDictionary->close();
			return;
		}
		AcDbObjectPointer<AcDbXrecord> pEmployeeEntry(employeeEntryId, AcDb::kForWrite);
		if (pEmployeeEntry->erase() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Unable to erase employee %s"), szEmployeeName);
			return;
		}
		acutPrintf(_T("\nEmployee %s successfully removed"), szEmployeeName);
	}
};

const TCHAR* CStep04App::g_szEmployeeDictionaryName = _T("ASDK_EMPLOYEE_DICTIONARY");

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep04App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _LISTENTRIES, LISTENTRIES, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _ADDENTRY, ADDENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AsdkStep04, _REMOVEENTRY, REMOVEENTRY, ACRX_CMD_TRANSPARENT, NULL)

