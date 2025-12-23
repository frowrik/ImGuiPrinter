#include "mini_printer.h"
#include "imgui_printer.h"

class ImGuiPrinter_Test : public ImGuiPrinter {
public:
    virtual bool InitResources() override {
        // SetStyle
        ImGui::StyleColorsLight();

        // SetFonts
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        static const ImWchar ranges[] = {
            0x0020, 0x00FF,  // Basic Latin + Latin Supplement
            0x0400, 0x052F,  // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF,  // Cyrillic Extended-A
            0xA640, 0xA69F,  // Cyrillic Extended-B
            0,
        };

        ImFontConfig merge_config = {};
        merge_config.MergeMode    = true;
        merge_config.OversampleH = 1;
        merge_config.OversampleV = 1;
        ImFont* font              = io.Fonts->AddFontFromFileTTF( "NotoSans-Regular.ttf", 32.0f, nullptr, ranges );
        io.FontDefault            = font;

        return true;
    }

    virtual void     FreeResources() override {
        // Nop
    }

    virtual void GetResolution( uint32_t& width_pixels, uint32_t& height_pixels ) override {
        width_pixels  = 1000;
        height_pixels = 1000 * sqrt( 2 );
    }

    virtual uint32_t GetCountPages() override { 
        //
        return 3;
    }

    virtual void DrawPage( uint32_t NumPage ) override {
        ImGui::Text( "Pagetest %d", NumPage );

        ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Borders;  // | ImGuiTableFlags_RowBg

        if ( ImGui::BeginTable( "simple_table", 3, flags ) ) {
            // 1. Setup headers
            ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize );
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize );
            ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize );
            ImGui::TableHeadersRow();

            for ( int row = (30 * NumPage); row < ( 30 * NumPage + 30 ); row++ ) {
                ImGui::TableNextRow();

                // Column 0: ID
                ImGui::TableNextColumn();
                ImGui::Text( "%03d", row );

                // Column 1: Name
                ImGui::TableNextColumn();
                ImGui::Text( "Item %c", 'A' + row );

                // Column 2: Value (with a widget)
                ImGui::TableNextColumn();
                static float values[5] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f };
                ImGui::SetNextItemWidth( -FLT_MIN );  // Fill the cell width
                ImGui::DragFloat( "##val", &values[row % 6], 0.01f );
            }

            ImGui::EndTable();
        }
    }

};

int main() {
    Printer  PrintTest;
    if ( !PrintTest.Create() ) 
        return -1;

    if ( !PrintTest.PrinterOpenDefault() )  
        return -1;
    
    if ( !PrintTest.PrinterOpenDoc( 
        "example imgui printer", "") )
        return false;

    // create padding options
    PrinterPageDesc PageDesc;
    PrintTest.PrinterGetPageDesc(PageDesc);

    PrinterPaddingDesc PaddingOptions;
    PaddingOptions.paddingLeft  = PageDesc.MillimetersToPixelsW(20);
    PaddingOptions.paddingTop   = PageDesc.MillimetersToPixelsH(5);
    PaddingOptions.paddingRight = PageDesc.MillimetersToPixelsW(10);
    PaddingOptions.paddingDown  = PageDesc.MillimetersToPixelsH(5);
    PrintTest.PrinterSetPagePadding(PaddingOptions);

    ImGuiPrinter_Test Printer_Test;
    Printer_Test.Printer( PrintTest );
    
    return 0;
}
