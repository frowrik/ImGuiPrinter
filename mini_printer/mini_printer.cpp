
#include "mini_printer.h"

#undef min
#undef max

Printer::~Printer() { Destroy(); }

#if PRINTER_API == PRINTER_API_WINAPI_XPSDocument

bool Printer::Create() {
    HRESULT hr;
    hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if ( FAILED( hr ) ) 
        hr = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );
    if ( FAILED( hr ) ) return false;
    
    hr = CoCreateInstance( __uuidof( XpsOMObjectFactory ), NULL, CLSCTX_INPROC_SERVER, __uuidof( IXpsOMObjectFactory ), reinterpret_cast<LPVOID*>( &xpsFactory ) );
    if ( FAILED( hr ) ) return false;
    
    //..
    
    return true;
}

void Printer::Destroy() {
    PrinterClose();

    if ( xpsFactory ) {
        xpsFactory->Release();
        xpsFactory = nullptr;
    }

    CoUninitialize();
}

void Printer::PrinterClose() {
    //
}

bool Printer::PrinterOpenDefault() {
    //..
    return true;
}

#endif

#if PRINTER_API == PRINTER_API_WINAPI_GDI

bool Printer::Create() {
    //..
    return true;
}

void Printer::Destroy() { 
    //
    PrinterClose();
}

void Printer::PrinterClose() {
    PrinterCloseDoc();

    if ( pPrinterInfo ) {
        free( pPrinterInfo );
        pPrinterInfo = nullptr;
    }
    if ( hPrinter ) {
        ClosePrinter( hPrinter );
        hPrinter = nullptr;
    }
}

bool Printer::PrinterOpenDefault() {
    //
    char  szPrinterName[MAX_PATH];
    DWORD cchPrinterName = ARRAYSIZE( szPrinterName );
    if ( !GetDefaultPrinterA( szPrinterName, &cchPrinterName ) ) 
        return false;
    
    //
    PRINTER_DEFAULTS pd;
    ZeroMemory( &pd, sizeof( pd ) );
    pd.DesiredAccess = PRINTER_ACCESS_USE;
    if ( !OpenPrinterA( szPrinterName, &hPrinter, &pd ) ) {
        return false;
    }

    //
    DWORD info_size = 0;
    GetPrinterA( hPrinter, 2, NULL, info_size, &info_size );
    if ( info_size > 0) {
        pPrinterInfo = static_cast<PRINTER_INFO_2*>( malloc( info_size ) );
        if ( pPrinterInfo ) {
            memset( pPrinterInfo, 0, info_size );
            if ( !GetPrinterA( hPrinter, 2, (PBYTE)pPrinterInfo, info_size, &info_size ) ) {
                free( pPrinterInfo );
                pPrinterInfo = nullptr;
                return false;
            }
            pPrinterInfo->pDevMode->dmOrientation = DMORIENT_PORTRAIT;
            pPrinterInfo->pDevMode->dmPaperSize = DMPAPER_A4;
            pPrinterInfo->pDevMode->dmFields |= DM_ORIENTATION | DM_PAPERSIZE;

            SetPrinterA( hPrinter, 2, (LPBYTE)pPrinterInfo, 0 );
        }
    }
    
    return true;
}

bool Printer::PrinterOpenDoc(const std::string& DocName, const std::string& OutputFile) {
    if ( !hPrinter || !pPrinterInfo ) return false;

    hdcPrinter = CreateDCA( NULL, pPrinterInfo->pPrinterName, NULL, NULL );
    if ( !hdcPrinter ) return false;

    DOCINFO di;
    memset( &di, 0, sizeof( di ) );
    di.cbSize      = sizeof( di );
    di.lpszDocName = (LPTSTR)DocName.c_str();
    if ( !OutputFile.empty()) di.lpszOutput = (LPTSTR)OutputFile.c_str();
    jobId          = StartDocA( hdcPrinter, &di );

    if ( jobId == 0 ) return false;

    return true;
}


void Printer::PrinterCloseDoc() {
    if ( !hPrinter || !pPrinterInfo ) return ;

    if ( jobId > 0 ) {
        EndDoc( hdcPrinter );
    }

    if ( hdcPrinter ) {
        DeleteDC( hdcPrinter );
        hdcPrinter = nullptr;
    }
}

bool Printer::PrinterDrawPageImage( int width, int height, const BYTE* pixelData, UINT bitsPerPixel ) {
    if ( !hPrinter || !pPrinterInfo || jobId == 0 || !hdcPrinter ) //
        return false;

    //int paperSize_w = GetDeviceCaps( hdcPrinter, PHYSICALWIDTH );
    //int paperSize_h = GetDeviceCaps( hdcPrinter, PHYSICALHEIGHT );
    int printable_x = GetDeviceCaps( hdcPrinter, PHYSICALOFFSETX );
    int printable_y = GetDeviceCaps( hdcPrinter, PHYSICALOFFSETY );
    int printable_w = GetDeviceCaps( hdcPrinter, HORZRES );
    int printable_h = GetDeviceCaps( hdcPrinter, VERTRES );
    int dpiX        = GetDeviceCaps( hdcPrinter, LOGPIXELSX );
    int dpiY        = GetDeviceCaps( hdcPrinter, LOGPIXELSY );

    int       Padding_Left  = Paddint_Left_cm  / 2.54 * dpiX;  
    int       Padding_Right = Paddint_Right_cm / 2.54 * dpiX;  
    int       Padding_Up    = Paddint_Up_cm    / 2.54 * dpiY; 
    int       Padding_Down  = Paddint_Down_cm  / 2.54 * dpiY; 

    int       startX          = std::max( 0, Padding_Left - printable_x );
    int       startY          = std::max( 0, Padding_Up - printable_y );

    printable_w               -= Padding_Left + Padding_Right;
    printable_h               -= Padding_Up + Padding_Down;

    float finalScale = std::min( (float)printable_w / (float)width, (float)printable_h / (float)height );

    int   newW       = (int)( width * finalScale );
    int   newH       = (int)( height * finalScale );
    
    StartPage( hdcPrinter );

    {
        //
        BITMAPINFO bmi              = { 0 };
        bmi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER );
        bmi.bmiHeader.biWidth       = width;
        bmi.bmiHeader.biHeight      = -height;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = bitsPerPixel;
        bmi.bmiHeader.biCompression = BI_RGB;
        HBITMAP hBitmap;
        void*   dibBits = nullptr;
        hBitmap         = CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &dibBits, NULL, 0 );
        if ( hBitmap && dibBits ) {
            int    bytesPerRow = ( ( width * bitsPerPixel + 31 ) / 32 ) * 4;
            size_t totalSize   = bytesPerRow * height;
            memcpy( dibBits, pixelData, totalSize );
        }

        //
        HDC     hMemDC  = CreateCompatibleDC( hdcPrinter );
        HBITMAP hOldBmp = (HBITMAP)SelectObject( hMemDC, hBitmap );
        BITMAP  bm;
        GetObjectA( hBitmap, sizeof( bm ), &bm );
        SetStretchBltMode( hdcPrinter, HALFTONE );
        StretchBlt( hdcPrinter, startX, startY, newW, newH, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
        SelectObject( hMemDC, hOldBmp );
        DeleteDC( hMemDC );

        //
        DeleteObject( hBitmap );
    }

    EndPage( hdcPrinter );

    return true;
}

#endif

 