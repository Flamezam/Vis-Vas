#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>

extern "C" 
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

int main()
{
    // -----------------------------  L U A  ----------------------------------
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dostring(L, "result = 6 * 7") != LUA_OK)
    {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    lua_getglobal(L, "result");
    const int luaResult = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);

    // -------------------------  J S O N  --------------------------------------
    nlohmann::json payload;
    payload["engine"] = "raylib";
    payload["gui"]    = "imgui";
    payload["script"] = "lua";
    payload["answer"] = luaResult;

    const std::string jsonDump = payload.dump(2);
    std::cout << "JSON payload:\n" << jsonDump << std::endl;

    // ---------------------------  r a y l i b   +   i m g u i  ------------------------------------
    constexpr int screenWidth  = 900;
    constexpr int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib + imgui + lua + json - proof of life");
    SetTargetFPS(60);

    rlImGuiSetup(true); /* dark theme */

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("raylib is rendering this frame", 20, 20, 20, DARKGRAY);
        DrawText(TextFormat("Lua computed 6 * 7 = %d", luaResult), 20, 50, 20, DARKGRAY);

        rlImGuiBegin();

        ImGui::Begin("Vendored Libraries - Proof of Life");
        ImGui::Text("raylib : window + rendering (this frame)");
        ImGui::Text("imgui  : this window, via the rlImGui backend");
        ImGui::Text("lua    : 6 * 7 = %d (computed in a Lua VM)", luaResult);
        ImGui::Separator();
        ImGui::TextUnformatted("nlohmann::json output:");
        ImGui::TextWrapped("%s", jsonDump.c_str());
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    lua_close(L);

    return 0;
}
