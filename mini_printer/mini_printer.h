#pragma once

#define PRINTER_API_WINAPI_GDI          0 
#define PRINTER_API_WINAPI_XPSDocument  1 // TODO
#define PRINTER_API_LINUX_CUPS          2 // TODO


#define PRINTER_API PRINTER_API_WINAPI_GDI


#include <cstdlib>
#include <string>
#include <algorithm>
#include <memory>
#include <climits>
#include <cfloat>
#include <cstdint>
#include <cinttypes>
#include <limits>
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING  1
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 1

#if PRINTER_API == PRINTER_API_WINAPI_GDI
    #include "windows.h"
    #include "Winspool.h"
// https://learn.microsoft.com/en-us/windows/win32/printdocs/gdi-print-api-functions
#endif

#if PRINTER_API == PRINTER_API_WINAPI_XPSDocument
    #include "windows.h"
    #include "Xpsobjectmodel.h"
// https://learn.microsoft.com/en-us/previous-versions/windows/desktop/dd372918(v=vs.85)
#endif

#if PRINTER_API == PRINTER_API_LINUX_CUPS
    #include <cups/cups.h>
#endif

class Printer {
public:
    ~Printer();

    bool Create();
    void Destroy();

    void PrinterClose();
    bool PrinterOpenDefault();
    
    bool PrinterOpenDoc( const std::string& DocName, const std::string& OutputFile = "" );
    void PrinterCloseDoc();

    bool PrinterDrawPageImage( int width, int height, const BYTE* pixelData, UINT bitsPerPixel );

public:
#if PRINTER_API == PRINTER_API_WINAPI_XPSDocument
    struct PageDesk {
        XPS_SIZE pageSize = { 800, 600 };
    };

    IXpsOMObjectFactory* xpsFactory = nullptr;
#endif

#if PRINTER_API == PRINTER_API_WINAPI_GDI
    HANDLE          hPrinter   = nullptr;
    PRINTER_INFO_2* pPrinterInfo = nullptr;
    HDC             hdcPrinter = nullptr;
    int             jobId      = 0;

#endif

public:
    int     Paddint_Left_cm     = 1;
    int     Paddint_Right_cm    = 1;
    int     Paddint_Up_cm       = 1;
    int     Paddint_Down_cm     = 1;
};
