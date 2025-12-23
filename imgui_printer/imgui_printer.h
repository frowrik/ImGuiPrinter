#pragma once

#include <imgui/imgui.h>
#include <imgui_sw.hpp>
#include <vector>

class ImGuiPrinter {
public:
    virtual bool     InitResources()                                                  = 0;
    virtual void     FreeResources()                                                  = 0;
    virtual void     GetResolution( uint32_t& width_pixels, uint32_t& height_pixels ) = 0;
    virtual uint32_t GetCountPages()                                                  = 0;
    virtual void     DrawPage( uint32_t NumPage )                                     = 0;

public:
    void Printer( Printer& Printer ) {
        //
        ImGuiContext* oldctx = ImGui::GetCurrentContext();
        ImGuiContext* drawctx = ImGui::CreateContext();
        ImGui::SetCurrentContext( drawctx );

        //
        InitResources();

        //
        uint32_t width_pixels  = 800;
        uint32_t height_pixels = 600;
        GetResolution( width_pixels, height_pixels );

        // set display proportins
        ImGuiIO& io    = ImGui::GetIO();
        io.DisplaySize = ImVec2( width_pixels, height_pixels );
        io.DeltaTime   = 0.001;

        // soft init
        imgui_sw::bind_imgui_painting();

        // 
        std::vector<uint32_t> pixel_buffer;
        pixel_buffer.resize( width_pixels * height_pixels, 0 );
        for ( size_t PageNum = 0; PageNum < GetCountPages(); PageNum++ ) {
            // draw
            ImGui::NewFrame();
            ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
            ImGui::SetNextWindowSize( ImGui::GetIO().DisplaySize);

            ImGui::Begin( ( std::string( "printer_page_" ) + std::to_string( PageNum ) ).c_str(), nullptr, 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | 
                ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
            );  //
            DrawPage( PageNum );
            ImGui::End();
            ImGui::Render();

            // draw printer page
            ImDrawData* draw_data = ImGui::GetDrawData();
            std::fill_n( pixel_buffer.data(), pixel_buffer.size(), 0x00FFFFFFu ); // bg ground
            imgui_sw::SwOptions sw_options;
            sw_options.optimize_rectangles = false;
            sw_options.optimize_text       = false;
            imgui_sw::paint_imgui( pixel_buffer.data(), width_pixels, height_pixels, sw_options );

            // send to printer page
            uint32_t* Data = pixel_buffer.data();
            Printer.PrinterDrawPageImage( width_pixels, height_pixels, (const BYTE*)Data, 32 );
        }

        // free softrender
        imgui_sw::unbind_imgui_painting();

        // free resources
        FreeResources();

        // free imgui context
        ImGui::DestroyContext( drawctx );
        ImGui::SetCurrentContext( oldctx );
    }
};
