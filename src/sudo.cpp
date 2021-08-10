#include <Windows.h>
#include <stdlib.h>
#include <iostream>

BOOL IsRunAsAdministrator()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority, 
        2, 
        SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS, 
        0, 0, 0, 0, 0, 0, 
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:

    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}

void ElevateNow()
{
	BOOL bAlreadyRunningAsAdministrator = FALSE;
	try
	{
		bAlreadyRunningAsAdministrator = IsRunAsAdministrator();
	}
	catch(std::exception ex)
	{
        std::cout << ex.what();
	}
	if(!bAlreadyRunningAsAdministrator)
	{
		char szPath[MAX_PATH];
		if (GetModuleFileName(NULL, szPath, sizeof(szPath)))
		{
			
		
			SHELLEXECUTEINFO sei = { sizeof(sei) };
			
			sei.lpVerb = "runas";
			sei.lpFile = szPath;
			sei.hwnd = NULL;
			sei.nShow = SW_NORMAL;

			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_CANCELLED)
			//Annoys you to Elevate it LOL
			CreateThread(0,0,(LPTHREAD_START_ROUTINE)ElevateNow,0,0,0);
	       	}
		}
	}
}

int main(int argc, char* argv[]) {
    if(IsRunAsAdministrator()) {
        std::cout << "hey";
    }
    else {
        std::string exc = "";
        for(int i = 1; i < argc; i++) {
            exc.append(argv[i]);
            exc.append(" ");
        }
        ElevateNow();
        system(exc.c_str());
    }
}