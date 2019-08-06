#include "CIKExport.h"

int CIKExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *CIKExport::Ext(int n)
{		
	switch(n) {
	case 0:
		// This cause a static string buffer overwrite
		// return GetString(IDS_EXTENSION1);
		return _T("CIK"); 
	}
	return _T("");
}

const TCHAR *CIKExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return GetString(IDS_LONGDESC); //3ds max ASCII Scene Exporter CIK Fotmat √÷¿Œ±’
}

const TCHAR *CIKExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return GetString(IDS_SHORTDESC); //CIK Format Exporter
}

const TCHAR *CIKExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return GetString(IDS_COPYRIGHT);
}

const TCHAR *CIKExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *CIKExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *CIKExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int CIKExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void CIKExport::ShowAbout(HWND hWnd)
{			
	// Optional
}
