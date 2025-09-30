#include <stdint.h>
#include <allegro5/system.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <iostream>
#include <mach/mach.h>
#include <mach/host_info.h>

struct CPUUsage 
{
    float user;
    float sys;
    float idle;
};

bool getCPUUsage( CPUUsage &usage ) 
{
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

    kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, 
                                    (host_info_t)&cpuinfo, &count);
    if (kr != KERN_SUCCESS) {
        std::cerr << "Failed to get CPU info\n";
        return false;
    }

    unsigned long totalTicks = cpuinfo.cpu_ticks[CPU_STATE_USER] +
                               cpuinfo.cpu_ticks[CPU_STATE_SYSTEM] +
                               cpuinfo.cpu_ticks[CPU_STATE_IDLE] +
                               cpuinfo.cpu_ticks[CPU_STATE_NICE];


    usage.idle = 100.0f * cpuinfo.cpu_ticks[CPU_STATE_IDLE] / totalTicks;
    usage.user = 100.0f * cpuinfo.cpu_ticks[CPU_STATE_USER] / totalTicks;
    usage.sys  = 100.0f * cpuinfo.cpu_ticks[CPU_STATE_SYSTEM] / totalTicks;

    return true;
}

int main(int, char**)
{
    // Setup Allegro
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_new_window_position(3100, 1200);
    ALLEGRO_DISPLAY* display = al_create_display(900, 500);
    al_set_window_title(display, "Resource Manager");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 1.5f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplAllegro5_Init(display);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool running = true;
    while (running)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ALLEGRO_EVENT ev;
        while (al_get_next_event(queue, &ev))
        {
            ImGui_ImplAllegro5_ProcessEvent(&ev);
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = false;
            if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
            {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                al_acknowledge_resize(display);
                ImGui_ImplAllegro5_CreateDeviceObjects();
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();

        {
            CPUUsage usage;
            if(!getCPUUsage( usage ))
                return 0;

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

            ImGui::Begin("CPU Data");

            ImGui::Text("idle %f  ", usage.idle);
            ImGui::Text("user %f  ", usage.user);
            ImGui::Text("sys  %f  ", usage.sys);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        al_clear_to_color(al_map_rgba_f(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w));
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
    }

    // Cleanup
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
