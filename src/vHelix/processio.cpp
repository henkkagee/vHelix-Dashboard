/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 *  Currently not used.
*/

#include "processio.h"

#define BUFSIZE 4096

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;


// Some modified boilerplate code for running an external executable from cmd and rerouting the output to the app console:
// Currently not in use.

std::string processio::handle(char* cmd)
{
   SECURITY_ATTRIBUTES saAttr;

   // Set the bInheritHandle flag so pipe handles are inherited.
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
   saAttr.bInheritHandle = TRUE;
   saAttr.lpSecurityDescriptor = NULL;

   // Create a pipe for the child process's STDOUT.
   if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
      processio::ErrorExit((char* const)TEXT("StdoutRd CreatePipe"));

   // Ensure the read handle to the pipe for STDOUT is not inherited.
   if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
      processio::ErrorExit((char* const)TEXT("Stdout SetHandleInformation"));

   // Create the child process.
   processio::CreateChildProcess(cmd);

    // Read from pipe that is the standard output for child process.
   std::stringstream buf;
   cout_redirect guard(buf.rdbuf());
   std::cout << "Running... \n";
   processio::ReadFromPipe();
   std::string out = buf.str();

   // The remaining open handles are cleaned up when this process terminates.
   return out;
}

// Create a child process that uses the previously created pipe for STDOUT.
void processio::CreateChildProcess(char* cmd)
{
   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure.
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.
   bSuccess = CreateProcess(NULL,
      cmd,           // command line
      NULL,          // process security attributes
      NULL,          // primary thread security attributes
      TRUE,          // handles are inherited
      0,             // creation flags
      NULL,          // use parent's environment
      NULL,          // use parent's current directory
      &siStartInfo,  // STARTUPINFO pointer
      &piProcInfo);  // receives PROCESS_INFORMATION

   // If an error occurs, exit the application.
   if ( ! bSuccess )
      processio::ErrorExit((char* const)TEXT("CreateProcess"));
   else
   {
      // Close handles to the child process and its primary thread.
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      CloseHandle(g_hChildStd_OUT_Wr);
      CloseHandle(g_hChildStd_IN_Rd);
   }
}

void processio::ReadFromPipe(void)

// Read output from the child process's pipe for STDOUT
// and write to the parent's std::cout
// Stop when there is no more data.
{
   DWORD dwRead;
   CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
   DWORD i;
   for (;;)
   {
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break;
      for (i = 0; i < dwRead; i++) {}
      chBuf[i] = 0;
      std::cout << chBuf;
      if (! bSuccess ) break;
   }
}

void processio::ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box,
// and exit from the application.
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}
