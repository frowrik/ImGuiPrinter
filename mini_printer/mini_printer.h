#pragma once

// def includes
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

// platfor includes
#define PRINTER_API_WINAPI_GDI          0 
#define PRINTER_API_WINAPI_XPSDocument  1
#define PRINTER_API_LINUX_CUPS          2

#if ((defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)))
    #include "windows.h"

    #if(WINVER >= 0x0601 && 0) // Windows > 7. TODO XPSDocument 
        #define PRINTER_API PRINTER_API_WINAPI_XPSDocument

        #include "Xpsobjectmodel.h" // https://learn.microsoft.com/en-us/previous-versions/windows/desktop/dd372918(v=vs.85)
    #else
        #define PRINTER_API PRINTER_API_WINAPI_GDI

        #include "Winspool.h"
    #endif
    
#elif (defined(__linux) || defined(__linux__) || defined(linux)) // TODO CUPS

    #define PRINTER_API PRINTER_API_LINUX_CUPS
    
    #include <cups/cups.h>
#else
    #error Unknown platform
#endif

// paper info
struct PrinterPageDesc {
public:
    int32_t paperSizeW     = 0;
    int32_t paperSizeH     = 0;
    int32_t printableX     = 0;
    int32_t printableY     = 0;
    int32_t printableW     = 0;
    int32_t printableH     = 0;
    float   pixelsPerInchW = 1.0; // DPI
    float   pixelsPerInchH = 1.0; // DPI

public: // Tools
    float   PixelsToInchW(float W) { return W / pixelsPerInchW; }
    float   PixelsToInchH(float H) { return H / pixelsPerInchH; }
    float   PixelsToMillimetersW(float W) { return PixelsToInchW(W) * 25.4; }
    float   PixelsToMillimetersH(float H) { return PixelsToInchH(H) * 25.4; }

    float   InchToPixelsW(float W) { return W * pixelsPerInchW; }
    float   InchToPixelsH(float H) { return H * pixelsPerInchH; }
    float   MillimetersToPixelsW(float W) { return InchToPixelsW(W / 25.4); }
    float   MillimetersToPixelsH(float H) { return InchToPixelsH(H / 25.4); }
};

// draw page desc
struct PrinterPaddingDesc {
    float   paddingLeft = 0.0; 
    float   paddingTop = 0.0; 
    float   paddingRight = 0.0; 
    float   paddingDown = 0.0; 
};

// main code
class Printer {
public:
    ~Printer();

    bool Create();
    void Destroy();

    //
    void PrinterClose();
    bool PrinterOpenDefault();
    
    //
    bool PrinterOpenDoc( const std::string& DocName, const std::string& OutputFile = "" );
    void PrinterCloseDoc();

    // get info page
    void PrinterGetPageDesc(PrinterPageDesc& OutDesc);

    // set def padding
    void PrinterSetPagePadding(PrinterPaddingDesc& Desc);

    // draw page image full
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
    PrinterPaddingDesc Padding = {};
};
