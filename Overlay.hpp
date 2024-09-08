#include <D3DX11.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#pragma comment(lib, "d3d11.lib")

#include <thread>
#include "imgui/imgui_internal.h"

#include "imgui/byte.h"
#include "imgui/elements.h"

enum heads {
    HEAD_1,
    HEAD_2,
    HEAD_3
};

namespace fonts {
    ImFont* medium;
    ImFont* semibold;

    ImFont* logo;
}

ImFont* LogoFont;

struct _globals_
{
    float
        width = GetSystemMetrics(0),
        height = GetSystemMetrics(1),
        field_of_view = 100.f;

    bool
        render_menu = false,
        crosshair = true,
        DrawFov = true;
}; static _globals_* globals = new _globals_();

class Color {
public:
    float r;
    float g;
    float b;
    float a;

    Color()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }


    Color(float r, float g, float b, float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color GetUnityColor()
    {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    Color Zero()
    {
        return Color(0, 0, 0, 0);
    }
};

struct _settings_
{
    float
        NoRecoilValue = 1.f,
        PlayerFieldOfView = 90.f;

    //MISC
    bool
        SuperBow = false,
        DoFov = false,
        Zoom = false,
        AdminFlags = false,
        AmbientColor = false,
        NoRecoil = false;

    //MISC
    bool
        Chams = false,
        Skeleton = false,
        BoxEsp = false,
        usernameEsp = false,
        DistanceEsp = false;

}; static _settings_* settings = new _settings_();

#include "BasePlayer.hpp"

float NormalizeAngle(float angle) {
    while (angle > 360.0f) {
        angle -= 360.0f;
    }
    while (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

Vector2 NormalizeVector2Angle(Vector2 angle) {
    angle.x = NormalizeAngle(angle.x);
    angle.y = NormalizeAngle(angle.y);
    return angle;
}

Vector3 NormalizeAngles(Vector3 angles) {
    angles.x = NormalizeAngle(angles.x);
    angles.y = NormalizeAngle(angles.y);
    angles.z = NormalizeAngle(angles.z);
    return angles;
}

Vector3 EulerAngles(Vector4 q1) {
    float num = q1.w * q1.w;
    float num2 = q1.x * q1.x;
    float num3 = q1.y * q1.y;
    float num4 = q1.z * q1.z;
    float num5 = num2 + num3 + num4 + num;
    float num6 = q1.x * q1.w - q1.y * q1.z;
    Vector3 vector;
    if (num6 > 0.4995f * num5) {
        vector.y = 2.0f * atan2(q1.y, q1.x);
        vector.x = 1.57079637f;
        vector.z = 0.0f;
        return NormalizeAngles(vector * 57.2958f);
    }
    if (num6 < -0.4995f * num5) {
        vector.y = -2.0f * atan2(q1.y, q1.x);
        vector.x = -1.57079637f;
        vector.z = 0.0f;
        return NormalizeAngles(vector * 57.2958f);
    }
    Vector4 quaternion = Vector4(q1.w, q1.z, q1.x, q1.y);
    vector.y = atan2(2.0f * quaternion.x * quaternion.w + 2.0f * quaternion.y * quaternion.z, 1.0f - 2.0f * (quaternion.z * quaternion.z + quaternion.w * quaternion.w));
    vector.x = asin(2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y));
    vector.z = atan2(2.0f * quaternion.x * quaternion.y + 2.0f * quaternion.z * quaternion.w, 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z));
    return NormalizeAngles(vector * 57.2958f);
}
//
//VOID Draw_Radar(_BasePlayer_* Entity, _Base_Local_Player_* Lplr)
//{
//    auto LocalPos = Lplr->get_player_model()->get_position();
//    auto PlayerPos = Entity->get_player_model()->get_position();
//    auto Distance = Vector3::Distance(LocalPos, PlayerPos);
//
//    const float y = LocalPos.x - PlayerPos.x;
//    const float x = LocalPos.z - PlayerPos.z;
//
//    //Vector3 eulerAngles = EulerAngles(Lplr->get_eyes()->get_eye_offset());
//
//    const float num = atan2(y, x) * 57.29578f - 270 - eulerAngles.y;
//    float RealDist = 0.0;
//
//    Distance *= 0.66;
//    if (Distance > 198)
//    {
//        float Calc = Distance - 198;
//        RealDist = Distance - Calc;
//    }
//    else
//    {
//        RealDist = Distance;
//    }
//
//    float PointPos_X = RealDist * cos(num * 0.0174532924f);
//    float PointPos_Y = RealDist * sin(num * 0.0174532924f);
//    PointPos_X = PointPos_X * (250 / 200) / 2.f;
//    PointPos_Y = PointPos_Y * (250 / 200) / 2.f;
//    Vector2 point = Vector2(150 + 250 / 2.f + PointPos_X, 200 + 250 / 2.f + PointPos_Y);
//
//    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(point.x, point.y), 5, ImColor(255,0,0,255));
//}

std::string string_To_UTF8(const std::string& str) {
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    ZeroMemory(pwBuf, nwLen * 2 + 2);
    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);
    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
    std::string retStr(pBuf);
    delete[]pwBuf;
    delete[]pBuf;
    pwBuf = NULL;
    pBuf = NULL;
    return retStr;
}

enum bone_list
{
    pelvis = 1,
    l_hip = 2,
    l_knee = 3,
    l_foot = 4,
    l_toe = 5,
    l_ankle_scale = 6,
    penis = 7,
    GenitalCensor = 8,
    GenitalCensor_LOD0 = 9,
    Inner_LOD0 = 10,
    GenitalCensor_LOD1 = 11,
    GenitalCensor_LOD2 = 12,
    r_hip = 13,
    r_knee = 14,
    r_foot = 15,
    r_toe = 16,
    r_ankle_scale = 17,
    spine1 = 18,
    spine1_scale = 19,
    spine2 = 20,
    spine3 = 21,
    spine4 = 22,
    l_clavicle = 23,
    l_upperarm = 24,
    l_forearm = 25,
    l_hand = 26,
    l_index1 = 27,
    l_index2 = 28,
    l_index3 = 29,
    l_little1 = 30,
    l_little2 = 31,
    l_little3 = 32,
    l_middle1 = 33,
    l_middle2 = 34,
    l_middle3 = 35,
    l_prop = 36,
    l_ring1 = 37,
    l_ring2 = 38,
    l_ring3 = 39,
    l_thumb1 = 40,
    l_thumb2 = 41,
    l_thumb3 = 42,
    IKtarget_righthand_min = 43,
    IKtarget_righthand_max = 44,
    l_ulna = 45,
    neck = 46,
    head = 47,
    jaw = 48,
    eyeTranform = 49,
    l_eye = 50,
    l_Eyelid = 51,
    r_eye = 52,
    r_Eyelid = 53,
    r_clavicle = 54,
    r_upperarm = 55,
    r_forearm = 56,
    r_hand = 57,
    r_index1 = 58,
    r_index2 = 59,
    r_index3 = 60,
    r_little1 = 61,
    r_little2 = 62,
    r_little3 = 63,
    r_middle1 = 64,
    r_middle2 = 65,
    r_middle3 = 66,
    r_prop = 67,
    r_ring1 = 68,
    r_ring2 = 69,
    r_ring3 = 70,
    r_thumb1 = 71,
    r_thumb2 = 72,
    r_thumb3 = 73,
    IKtarget_lefthand_min = 74,
    IKtarget_lefthand_max = 75,
    r_ulna = 76,
    l_breast = 77,
    r_breast = 78,
    BoobCensor = 79,
    BreastCensor_LOD0 = 80,
    BreastCensor_LOD1 = 81,
    BreastCensor_LOD2 = 82,
    collision = 83,
    displacement = 84
};

struct bones_t
{
public:

    Vector3 head{};

    Vector3 upper_spine{};

    Vector3 left_shoulder{};
    Vector3 left_elbow{};
    Vector3 left_wrist{};

    Vector3 right_shoulder{};
    Vector3 right_elbow{};
    Vector3 right_wrist{};

    Vector3 lower_spine{};

    Vector3 left_hip{};
    Vector3 left_knee{};
    Vector3 left_ankle{};

    Vector3 right_hip{};
    Vector3 right_knee{};
    Vector3 right_ankle{};
};
namespace Render
{
    void DrawLine(Vector2 p1, Vector2 p2, ImColor clr, float thick)
    {
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImColor(0,0,0,255), thick + 2);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), clr, thick);
    }

    void DrawCornerBox(int X, int Y, int W, int H, const ImColor color, int thickness) {
        float lineW = (W / 3);
        float lineH = (H / 3);

        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), color, thickness);
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), color, thickness);
    }

    void DrawBox(float X, float Y, float W, float H, ImColor color)
    {
        ImGui::GetForegroundDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), color);
        ImGui::GetForegroundDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), color);
    }
}

VOID Entity_Loop()
{
    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(150, 200), 125, ImColor(30, 30, 30, 150));

    ImGui::GetForegroundDrawList()->AddLine(ImVec2(150, 75), ImVec2(150, 325), ImColor(15, 15, 15, 100));

    ImGui::GetForegroundDrawList()->AddLine(ImVec2(25, 200), ImVec2(275, 200), ImColor(15, 15, 15, 100));

    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(150, 200), 5, ImColor(0, 255, 0, 255));

    auto Convar_Graphics_C = IO::read<uintptr_t>(game_image_address + 61217712);
    auto m_pGraphicsInstance = IO::read<uintptr_t>(Convar_Graphics_C + 0xB8);

    if (settings->DoFov)
    {
        IO::write<float>(m_pGraphicsInstance + 0x18, settings->PlayerFieldOfView);
    }

    main_camera camera;

    for (auto& Entity : EntCacheList)
    {
        //Draw_Radar(Entity.Entity, Entity.LocalPlayer);

        Vector3 EntPos = Entity.Entity->get_player_model()->get_position();

        Vector2 EntityPos = WorldToScreen(EntPos);

        std::string _displayName = string_To_UTF8(Entity.Entity->get_display_name());

        if (EntityPos.x == 0 || EntityPos.y == 0)
            continue;

        if (settings->DistanceEsp)
        {
            auto distanceFromEntity = Vector3::Distance(Entity.LocalPlayer->get_player_model()->get_position(), Entity.Entity->get_player_model()->get_position());

            std::string nignog = std::to_string(static_cast<int>(distanceFromEntity)) + "m";
            ImGui::GetForegroundDrawList()->AddText(ImVec2(EntityPos.x, EntityPos.y), ImColor(15,15,15, 255), nignog.c_str());
        }

        if (GetAsyncKeyState(VK_RBUTTON))
        {
            auto head_b = Entity.Entity->get_bone(head);
            Vector2 head = WorldToScreen(head_b);

            auto dx = head.x - (globals->width / 2);
            auto dy = head.y - (globals->height / 2);
            auto dist = sqrtf(dx * dx + dy * dy);

            if (dist < globals->field_of_view) // IF PLAYER IN FOV
            {
                Entity.LocalPlayer->get_eyes()->look_at(CalculateAngle(camera.position(), head_b)); // PSILENT
            }
        }

        ImGui::GetForegroundDrawList()->AddLine(ImVec2(globals->width / 2, globals->height), ImVec2(EntityPos.x, EntityPos.y), ImColor(15,15,15,255));

        if (settings->usernameEsp)
        {
            auto head_b = Entity.Entity->get_bone(head);
            Vector2 head = WorldToScreen(head_b);
            
            ImGui::GetForegroundDrawList()->AddText(ImVec2(head.x, head.y - 15), ImColor(15,15,15, 255), _displayName.c_str());
        }
    }

    for (auto& Skeleton : EntBoneList)
    {
        if (settings->BoxEsp)
        {
            float BoxHeight = (float)(Skeleton.head.y - Skeleton.bottom.y);
            float CornerHeight = abs(Skeleton.head.y - Skeleton.bottom.y);
            float CornerWidth = BoxHeight * 0.50;

            Render::DrawBox(Skeleton.head.x - (CornerWidth / 2), Skeleton.head.y, CornerWidth, CornerHeight, ImColor(0,0,0,255));
        }

        if (settings->Skeleton)
        {
            ImColor clr = ImColor(15,15,15, 255);
            float Wight = 1.f;

            Render::DrawLine(Skeleton.head, Skeleton.spine, clr, Wight);
            Render::DrawLine(Skeleton.spine, Skeleton.l_upperarm, clr, Wight);
            Render::DrawLine(Skeleton.l_upperarm, Skeleton.l_forearm, clr, Wight);
            Render::DrawLine(Skeleton.l_forearm, Skeleton.l_hand, clr, Wight);

            Render::DrawLine(Skeleton.spine, Skeleton.r_upperarm, clr, Wight);
            Render::DrawLine(Skeleton.r_upperarm, Skeleton.r_forearm, clr, Wight);
            Render::DrawLine(Skeleton.r_forearm, Skeleton.r_hand, clr, Wight);
            Render::DrawLine(Skeleton.spine, Skeleton.pelvis1, clr, Wight);

            Render::DrawLine(Skeleton.pelvis, Skeleton.l_hip, clr, Wight);
            Render::DrawLine(Skeleton.l_hip, Skeleton.l_knee, clr, Wight);
            Render::DrawLine(Skeleton.l_knee, Skeleton.l_foot, clr, Wight);
            Render::DrawLine(Skeleton.pelvis, Skeleton.r_hip, clr, Wight);

            Render::DrawLine(Skeleton.r_hip, Skeleton.r_knee, clr, Wight);
            Render::DrawLine(Skeleton.r_knee, Skeleton.r_foot, clr, Wight);
            Render::DrawLine(Skeleton.r_foot, Skeleton.r_toe, clr, Wight);
            Render::DrawLine(Skeleton.l_foot, Skeleton.l_toe, clr, Wight);
        }
    }
}

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;

namespace Render
{
    auto Init() -> bool {
        DXGI_SWAP_CHAIN_DESC swap_chain_description = {};
        swap_chain_description.BufferCount = 2;
        swap_chain_description.BufferDesc.Width = globals->width; // Set the appropriate width based on your requirements
        swap_chain_description.BufferDesc.Height = globals->height; // Set the appropriate height based on your requirements
        swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_description.BufferDesc.RefreshRate.Numerator = 1000;
        swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_description.OutputWindow = window_handle;
        swap_chain_description.SampleDesc.Count = 1;
        swap_chain_description.SampleDesc.Quality = 0;
        swap_chain_description.Windowed = TRUE;
        swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL d3d_feature_lvl;

        const D3D_FEATURE_LEVEL d3d_feature_array[1] = { D3D_FEATURE_LEVEL_11_0 };

        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            d3d_feature_array,
            1,
            D3D11_SDK_VERSION,
            &swap_chain_description,
            &d3d_swap_chain,
            &d3d_device,
            &d3d_feature_lvl,
            &d3d_device_ctx);

        ID3D11Texture2D* pBackBuffer;

        d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

        d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

        pBackBuffer->Release();

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        io.IniFilename = NULL;

        ImFontConfig font_config;
        font_config.PixelSnapH = false;
        font_config.OversampleH = 5;
        font_config.OversampleV = 5;
        font_config.RasterizerMultiply = 1.2f;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };

        font_config.GlyphRanges = ranges;

        fonts::medium = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 15.0f, &font_config, ranges);
        fonts::semibold = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 17.0f, &font_config, ranges);

        LogoFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 18.f);

        (void)io;
        D3DX11_IMAGE_LOAD_INFO info1;
        ID3DX11ThreadPump* pump{ nullptr };

        ImGui_ImplWin32_Init(window_handle);

        ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

        d3d_device->Release();

        return true;
    }

    auto Menu() -> VOID
    {
        static heads head_selected = HEAD_1;
        static bool checkbox[1]{};
        static bool visuals_check[1]{};
        static bool misc_check[1]{};

        static int combobox, sliderscalar = 0;
        const char* combobox_items[3] = { "Head", "Neck", "Chest" };

        ImGui::SetNextWindowSize({ 500, 370 });

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("Bypassed.AC", nullptr, ImGuiWindowFlags_NoDecoration);
        {
            auto draw = ImGui::GetWindowDrawList();

            auto pos = ImGui::GetWindowPos();
            auto size = ImGui::GetWindowSize();

            draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 51), ImColor(24, 24, 24), 9.0f, ImDrawFlags_RoundCornersTop);
            draw->AddRectFilledMultiColorRounded(pos, ImVec2(pos.x + 55, pos.y + 51), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.05f), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.05f), 9.0f, ImDrawFlags_RoundCornersTopLeft);

            draw->AddText(fonts::semibold, 17.0f, ImVec2(pos.x + 25, pos.y + 18), ImColor(192, 203, 229), "Bypassed.AC");

            ImGui::SetCursorPos({ 175, 19 });
            ImGui::BeginGroup(); {
                if (elements::tab("Aimbot", head_selected == HEAD_1)) head_selected = HEAD_1;
                ImGui::SameLine();
                if (elements::tab("Visuals", head_selected == HEAD_2)) head_selected = HEAD_2;
                ImGui::SameLine();
                if (elements::tab("Misc", head_selected == HEAD_3)) head_selected = HEAD_3;
            }
            ImGui::EndGroup();

            switch (head_selected) {
            case HEAD_1:
                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 25, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Aimbot");

                ImGui::SetCursorPos({ 25, 85 });
                ImGui::BeginChild("##container", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Checkbox("Memory Aim", &checkbox[0]);
                    ImGui::Checkbox("Draw Fov", &globals->DrawFov);

                    ImGui::Combo("Aimbone", &combobox, combobox_items, IM_ARRAYSIZE(combobox_items));

                    ImGui::SliderFloat("FOV", &globals->field_of_view, 0.f, 500.0f);
                }
                ImGui::EndChild();

                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 285, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Aim Settings");

                ImGui::SetCursorPos({ 285, 85 });
                ImGui::BeginChild("##container1", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Text("");
                }
                ImGui::EndChild();
                break;
            case HEAD_2:
                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 25, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Visuals");

                ImGui::SetCursorPos({ 25, 85 });
                ImGui::BeginChild("##container", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Checkbox("2D Box", &settings->BoxEsp);
                    ImGui::Checkbox("3D Box", &visuals_check[1]);
                    ImGui::Checkbox("Skeletons", &settings->Skeleton);
                    ImGui::Checkbox("Username Esp", &settings->usernameEsp);
                    ImGui::Checkbox("Distance Esp", &settings->DistanceEsp);
                    ImGui::Checkbox("Chams", &settings->Chams);
                }
                ImGui::EndChild();

                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 285, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "World Visuals");

                ImGui::SetCursorPos({ 285, 85 });
                ImGui::BeginChild("##container2", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Text("");
                }
                ImGui::EndChild();
                break;
            case HEAD_3:
                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 25, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Misc");

                ImGui::SetCursorPos({ 25, 85 });
                ImGui::BeginChild("##container", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Checkbox("No Recoil", &settings->NoRecoil);

                    ImGui::Checkbox("Super Melee", &settings->SuperBow);

                    ImGui::Checkbox("Admin Flags", &settings->AdminFlags);
                    
                    ImGui::Checkbox("Camera Field Of View", &settings->DoFov);

                    ImGui::Checkbox("Zoom (N)", &settings->Zoom);

                    ImGui::SliderFloat("FOV", &settings->PlayerFieldOfView, 0.0f, 180.0f);
                }
                ImGui::EndChild();

                draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 285, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Misc 2");

                ImGui::SetCursorPos({ 285, 85 });
                ImGui::BeginChild("##container2", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
                    ImGui::Checkbox("Ambient Color", &settings->AmbientColor);
                }
                ImGui::EndChild();
                break;
            }
        }
        ImGui::End();

        ImGui::PopStyleVar();
    }

    auto StartRender() -> VOID 
    {
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Entity_Loop();

        if (GetAsyncKeyState(VK_INSERT) & 1)
            globals->render_menu = !globals->render_menu;

        if (globals->DrawFov)
        {
            ImGui::GetForegroundDrawList()->AddCircle(ImVec2(globals->width / 2, globals->height / 2), globals->field_of_view, ImColor(0, 0, 0, 255), 100, 3.f);
            ImGui::GetForegroundDrawList()->AddCircle(ImVec2(globals->width / 2, globals->height / 2), globals->field_of_view, ImColor(255, 255, 225, 255), 100, 2.f);
        }

        if (globals->crosshair)
        {
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(globals->width / 2 - 10, globals->height / 2), ImVec2(globals->width / 2 + 10, globals->height / 2), ImColor(0, 0, 0, 255), 2.f);
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(globals->width / 2, globals->height / 2 - 10), ImVec2(globals->width / 2, globals->height / 2 + 10), ImColor(0, 0, 0, 255), 2.f);
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(globals->width / 2 - 10, globals->height / 2), ImVec2(globals->width / 2 + 10, globals->height / 2), ImColor(255, 255, 255, 255), 1.f);
            ImGui::GetForegroundDrawList()->AddLine(ImVec2(globals->width / 2, globals->height / 2 - 10), ImVec2(globals->width / 2, globals->height / 2 + 10), ImColor(255, 255, 255, 255), 1.f);
        }

        ImGui::PushFont(LogoFont);
        ImGui::GetForegroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 15, 15), "Y O D I E.AC");
        ImGui::PopFont();

        ImGui::PushFont(LogoFont);
        ImGui::GetForegroundDrawList()->AddText(ImVec2(globals->width - 60, 5), ImColor(255, 255, 255), std::to_string(static_cast<int>(ImGui::GetIO().Framerate)).c_str());
        ImGui::PopFont();

        if (globals->render_menu)
        {
            Menu();
        }

        if (GetAsyncKeyState(VK_DELETE))
        {
            exit(0);
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
        d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        d3d_swap_chain->Present(0, 0);
    }

    auto Draw() -> bool
    {
        static RECT rect_og;
        MSG msg = { NULL };
        ZeroMemory(&msg, sizeof(MSG));

        while (msg.message != WM_QUIT)
        {
            UpdateWindow(window_handle);
            ShowWindow(window_handle, SW_SHOW);

            if ((PeekMessageA)(&msg, window_handle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            ImGuiIO& io = ImGui::GetIO();
            io.DeltaTime = 1.0f / 60.0f;

            POINT p_cursor;
            GetCursorPos(&p_cursor);
            io.MousePos.x = p_cursor.x;
            io.MousePos.y = p_cursor.y;

            if (GetAsyncKeyState(VK_LBUTTON)) {
                io.MouseDown[0] = true;
                io.MouseClicked[0] = true;
                io.MouseClickedPos[0].x = io.MousePos.x;
                io.MouseClickedPos[0].x = io.MousePos.y;
            }
            else
                io.MouseDown[0] = false;

            StartRender();
        }
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        (DestroyWindow)(window_handle);

        return true;
    }
}