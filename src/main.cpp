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
#include <vector>

/*
DataPipeline templated
 - Input - LoadFromCSV(path)
 - Filters - ApplyFilter from DataFilters (AVG, min-max, normalize timestamp to 0)


 Effector objected
 - Input Pipeline
 - Convert to data
 - Update
 - Render


 Program:
 - Updata objects
 - Render game
 - Render UI

 UI:
 - Render()

*/

// ------------------------------------ Data layer ------------------------------------------


struct GyroData
{
    std::string ID;
    std::uint64_t Timestamp;
    double dx, dy, dz;
};

template <typename SensorDataT>
class DataFilter
{
public:
    virtual void ApplyFilter(std::vector<SensorDataT>& data) = 0;                                               // WILL CHANGE THE VECTORS DATA
    std::vector<SensorDataT> ApplyFilterNoOverride(const std::vector<SensorDataT>& data)      // will NOT change the data, TWO COPIES
    {
        std::vector<SensorDataT> returnValue(data);
        ApplyFilter(returnValue);
        return returnValue;
    }
};

template <typename SensorDataT>
class DataPipeline
{
public:
    void AddFilter(const DataFilter<SensorDataT>& filter)
    {
        m_filters.push_back(filter);
    }

    const std::vector<DataFilter<SensorDataT>>& GetFilters() const
    {
        return m_filters;
    }

    bool LoadDataFromCSVFile(const std::string& path)
    {
        // TO IMPLEMENT
        // Load into m_data


        m_path = path;
    }

    void Apply()
    {
        // TO IMPLEMENT
        // if not loaded error
        for (auto& filter : m_filters)
        {
            filter.ApplyFilter(m_data);
        }

        // sort by timestamp

    }


private:
    std::vector<SensorDataT> m_data;
    std::vector<DataFilter<SensorDataT>> m_filters; // applied in order
    std::string m_path;
};

// -------------------------------------- Objects, effectors ----------------------------------------

struct SingleGyroCompute
{
    std::string ID;
    double x, y, z;
};

template <typename SingleSensorComputeT>
class Constraints
{
public:
    virtual void ApplyConstraint(SingleSensorComputeT& data) = 0; // Will override
    
    SingleSensorComputeT ApplyFilterNoOverride(const SingleSensorComputeT& data)      // will NOT change the data, TWO COPIES
    {
       SingleSensorComputeT returnValue = data;
       ApplyConstraint(returnValue);
       return returnValue;
    }
};

class Object
{
public:
    virtual void Update(const float dt) = 0;
    virtual void Render() = 0;
};

class CharlesObject : public Object
{
public:

    CharlesObject(DataPipeline<GyroData>* dataSource)
    {
        m_dataSource = dataSource;
    }

    void AddConstraint(const Constraints<SingleGyroCompute> constraint)
    {
        m_constraints.push_back(constraint);
    }

    const std::vector<Constraints<SingleGyroCompute>>& GetConstraints() const
    {
        return m_constraints;
    }

    void Execute()
    {
        ConvertToSensorData();
        PreCompute();
        m_computed = true;
    }


    void Update(const float dt) override
    {
        if (!m_computed)
            return;

        m_currentTime += dt;
        // Change positions like in map 
    }

    void Render() override
    {
        if (!m_computed)
            return;
        // Draw raylib objects to show it
    }

    virtual ~CharlesObject()
    {
        if (m_dataSource != nullptr)
            delete m_dataSource;
    }

private:
    void ConvertToSensorData()
    {
        // Convert Sensor data by sensor into ordered map of ID - vec<SingleGyroCompte>
    }
    void PreCompute()
    {
        // Move
        // Apply Constraints
    }

    bool m_computed = false;
    float m_currentTime = 0;
    std::vector<Constraints<SingleGyroCompute>> m_constraints;
    DataPipeline<GyroData>* m_dataSource = nullptr;
};

// ------------------------------------ UI ------------------------------------------

class UI
{
public:
    bool Active = true;
    virtual void Render() = 0;
};

class TimelineUI : public UI
{
public:
    void Render() override
    {
        //...
    }

    float dtRate()
    {
        return 1;
    }

    float getTime()
    {
        return 0;
    }
};

// ----------------------------------- PROGRAM -------------------------------------------
class Program
{
public:
    void Start()
    {
        Configure();
        run();
    }

    ~Program()
    {
        for (auto* obj : m_objects)
            delete obj;
    }

    float dtRate = 0;
    float timelineTime = 0;
private:
    void Configure()
    {
        // Screen size, etc, probably more args in constructor
    }

    void run()
    {
        float dt = 0;

        // main loop + events
            Update(dt);
            Render();
            RenderUI();
    }

    void Update(float dt)
    {
        //...
        if (m_weak_Timeline)
        {
            dtRate = m_weak_Timeline->dtRate();
            timelineTime = m_weak_Timeline->getTime();
        }

    }

    void Render()
    {

    }

    void RenderUI()
    {
        for (auto* ui : m_UIs)
        {
            if (ui->Active)
                ui->Render();
        }
    }

    TimelineUI* m_weak_Timeline = nullptr; // Do not delete, will be deleted from UIs
    std::vector<UI*> m_UIs;
    std::vector<Object*> m_objects;

};


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
