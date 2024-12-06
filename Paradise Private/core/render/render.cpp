#include "render.hpp"

#include <Windows.h>
#include <iostream>
#include <string>
#include <d3d9.h>
#include <d3d11.h>
#include <dwmapi.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <dependencies/framework/imgui.h>
#include <dependencies/framework/backends/imgui_impl_dx11.h>
#include <dependencies/framework/backends/imgui_impl_win32.h>
#include <dependencies/framework/imgui_internal.h>
#include <core/game/sdk.hpp>
#include <utilities/obfuscation/xor.hpp>
#include <core/game/features/visuals/visuals.hpp>
#include <dependencies/random.h>
#include <dependencies/font.h>
#include <dependencies/framework/icon.h>
#include <dependencies/menufont.h>
#include <d3d11.h>
#include <stdexcept>
#include <d3d11.h>
#include <dependencies/framework/libraries/ParadiseLogo.h>

#define STB_IMAGE_IMPLEMENTATION
#include <dependencies/framework/libraries/stb.h>

ID3D11ShaderResourceView* ParadiseLogo = NULL;

ID3D11Device* D3DDevice;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* D3DDeviceContext;
ID3D11ShaderResourceView* g_pTextureView = nullptr;
IDXGISwapChain* D3DSwapChain;
ID3D11RenderTargetView* D3DRenderTarget;
D3DPRESENT_PARAMETERS D3DPresentParams;
ImFont* IconsFont;
ImFontConfig cfg;

HWND hWindowHandle = 0;
RECT GameRect = { NULL };
int Width;
int Height;
bool bShowMenu = true;
int MenuTab;

DWORD ScreenCenterX;
DWORD ScreenCenterY;
MARGINS Margin = { -1 };


inline bool GetImage(const wchar_t* filePath, const BYTE* data, DWORD dataSize)
{
    HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {

        return false;
    }

    DWORD bytesWritten = 0;
    BOOL result = WriteFile(hFile, data, dataSize, &bytesWritten, NULL);
    if (!result || bytesWritten != dataSize)
    {

        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    D3DDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    D3DDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

void jrt::render_c::SetWindowToTarget()
{
    while (true)
    {
        SetWindowPos(hwnd, hWindowHandle, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

auto jrt::render_c::Initialize() -> bool
{
    std::thread([]() { jrt::render.SetWindowToTarget(); }).detach();

    WNDCLASS windowClass = { 0 };
    windowClass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.lpfnWndProc = DefWindowProcA;
    std::wstring randomClassName = RandomString(16);
    windowClass.lpszClassName = randomClassName.c_str();
    windowClass.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&windowClass))
    {
        std::cout << (_("\n\n [-] ").decrypt()) << WStringToString(randomClassName) << (_("Overlay Failed").decrypt());
    }

    hWindowHandle = CreateWindow(randomClassName.c_str(),
        NULL,
        WS_POPUP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        windowClass.hInstance,
        NULL);


    ShowWindow(hWindowHandle, SW_SHOW);
    DwmExtendFrameIntoClientArea(hWindowHandle, &Margin);
    SetWindowLong(hWindowHandle, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
    UpdateWindow(hWindowHandle);

    DXGI_SWAP_CHAIN_DESC SwapChainDescription;
    ZeroMemory(&SwapChainDescription, sizeof(SwapChainDescription));
    SwapChainDescription.BufferCount = 2;
    SwapChainDescription.BufferDesc.Width = 0;
    SwapChainDescription.BufferDesc.Height = 0;
    SwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDescription.OutputWindow = hWindowHandle;
    SwapChainDescription.SampleDesc.Count = 1;
    SwapChainDescription.SampleDesc.Quality = 0;
    SwapChainDescription.Windowed = 1;
    SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    static const ImWchar IconRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    static const ImWchar IconRangesBrand[] = { ICON_MIN_FAB, ICON_MAX_16_FAB, 0 };

    D3D_FEATURE_LEVEL d3d_feature_lvl;
    const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, d3d_feature_array, 2, D3D11_SDK_VERSION,
        &SwapChainDescription, &D3DSwapChain, &D3DDevice, &d3d_feature_lvl, &D3DDeviceContext);

    if (FAILED(hr))
        return false;

    ID3D11Texture2D* pBackBuffer;
    hr = D3DSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr))
        return false;

    hr = D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &D3DRenderTarget);
    pBackBuffer->Release();
    if (FAILED(hr))
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.Fonts->AddFontFromMemoryTTF(&Font, sizeof(Font), 18.0f);
    jrt::fonts.Menu = io.Fonts->AddFontFromMemoryTTF(&MenuFont, sizeof(MenuFont), 15.0f);
    jrt::fonts.Icons = io.Fonts->AddFontFromMemoryCompressedTTF(SolidData, SolidSize, 18, &cfg, IconRange);
    GetImage(_(L"C:\\ParadiseLogo.jpg").decrypt(), Paradise, sizeof(Paradise));

    if (!ImGui_ImplWin32_Init(hWindowHandle) || !ImGui_ImplDX11_Init(D3DDevice, D3DDeviceContext))
        return false;

    return true;
}

enum eMenuPage_ : int
{
    // Player
    MenuPage_Aimbot,
    MenuPage_Visuals,
    MenuPage_Misc,
    MenuPage_Exploits,
    MenuPage_Settings,
    MenuPage_COUNT
};

void LoadStyles()
{
    // Custom styles
    ImGuiStyle& style = ImGui::GetStyle();

    // Text
    ImGui::StyleColorsDark();

    style.WindowRounding = 6;
    style.ChildRounding = 6;
    style.FrameRounding = 3;
    style.GrabRounding = 3;
    style.PopupRounding = 3;
    style.TabRounding = 3;
    style.ScrollbarRounding = 1;

    style.ButtonTextAlign = { 0.5f, 0.5f };
    style.WindowTitleAlign = { 0.5f, 0.5f };
    style.FramePadding = { 10.0f, 10.0f };
    style.WindowPadding = { 10.0f, 10.0f };
    style.ItemSpacing = style.WindowPadding;
    style.ItemInnerSpacing = { style.WindowPadding.x, 4 };

    style.WindowBorderSize = 1;
    style.FrameBorderSize = 1;

    style.ScrollbarSize = 12.f;
    style.GrabMinSize = style.FrameRounding;

    // Colors
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.031f, 0.039f, 0.047f, 1.0f); // Hex: 0x080A0C
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.031f, 0.039f, 0.047f, 1.0f); // Hex: 0x080A0C
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.039f, 0.047f, 0.055f, 1.0f); // Hex: 0x0A0C0E
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.039f, 0.047f, 0.055f, 1.0f); // Hex: 0x0A0C0E

    style.Colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.0f); // Hex: 0xFFFFFF
    style.Colors[ImGuiCol_CheckMark] = style.Colors[ImGuiCol_Text];
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.475f, 0.475f, 0.475f, 1.0f); // Hex: 0x797979

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.510f, 0.282f, 1.000f, 1.0f); // Hex: 0x8248FF
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.275f, 0.141f, 0.502f, 1.0f); // Hex: 0x462480

    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);       // Transparent
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315

    style.Colors[ImGuiCol_Border] = ImVec4(0.063f, 0.071f, 0.082f, 1.0f); // Hex: 0x101215
    style.Colors[ImGuiCol_Separator] = ImVec4(0.510f, 0.282f, 1.000f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.067f, 0.075f, 0.082f, 0.7f); // Hex: 0x111315 with alpha 0.7
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.067f, 0.075f, 0.082f, 0.5f); // Hex: 0x111315 with alpha 0.5

    style.Colors[ImGuiCol_Header] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.067f, 0.075f, 0.082f, 0.7f); // Hex: 0x111315 with alpha 0.7
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.067f, 0.075f, 0.082f, 0.5f); // Hex: 0x111315 with alpha 0.5

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.067f, 0.075f, 0.082f, 1.0f); // Hex: 0x111315
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.067f, 0.075f, 0.082f, 0.7f); // Hex: 0x111315 with alpha 0.7
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.067f, 0.075f, 0.082f, 0.5f); // Hex: 0x111315 with alpha 0.5
}

void PlayerCounter(int PlayerCount, int VisibleCount)
{
    float ScreenWidth = ImGui::GetIO().DisplaySize.x;

    std::string PlayerCounter = _s("[") + std::to_string(PlayerCount) + _s(" Rendered]  [") + std::to_string(VisibleCount) + _s(" Visible]");

    ImVec2 Size = ImGui::CalcTextSize(PlayerCounter.c_str());

    ImVec2 PlayerCounterSize = ImVec2((ScreenWidth - Size.x) / 2, 10);

    ImGui::GetBackgroundDrawList()->AddText(PlayerCounterSize, ImColor(255, 255, 255), PlayerCounter.c_str(), 0, true);
}

void jrt::render_c::RenderMenu()
{
    if (SPOOF_CALL(GetAsyncKeyState)(VK_INSERT) & 1)
        bShowMenu = !bShowMenu;

    int FPS = ImGui::GetIO().Framerate;

    int LogoWidth = 50;
    int LogoHeight = 50;

    PlayerCounter(jrt::settings.iPlayerCount, jrt::settings.iVisiblePlayers);

    ImGui::GetBackgroundDrawList()->AddText(ImVec2(8, 10), ImColor(255, 255, 255), _("Paradise"), 0, true);
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(65, 10), ImColor(255, 255, 255), _("Fortnite"), 0, true);

    jrt::settings.bMenuOpen = bShowMenu;
    if (!bShowMenu) return;

    LoadStyles();

    //LoadTextureFromFile(_("C:\\ParadiseLogo.jpg"), &ParadiseLogo, &LogoWidth, &LogoHeight);


    static ImVec2 MenuSize = ImVec2(600, 550);
    ImGui::SetNextWindowSize(MenuSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize / 2 - MenuSize / 2, ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::PushFont(jrt::fonts.Menu);

    ImGui::Begin(_("Paradise Recoded"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::PopStyleVar(2);

    ImGuiStyle& style = ImGui::GetStyle();
    const float HeaderHeight = 82.0f;
    float FooterHeight = ImGui::GetFrameHeight();
    ImVec2 Pos = ImGui::GetWindowPos();
    ImVec2 Size = ImGui::GetWindowSize();

    ImGui::GetWindowDrawList()->AddRectFilled(Pos, Pos + ImVec2(Size.x, HeaderHeight), ImGui::GetColorU32(ImGuiCol_ChildBg), style.WindowRounding, ImDrawFlags_RoundCornersTop);
    ImGui::GetWindowDrawList()->AddRectFilled(Pos + ImVec2(0, HeaderHeight), Pos + ImVec2(Size.x, Size.y - FooterHeight), ImGui::GetColorU32(ImGuiCol_WindowBg), style.WindowRounding, ImDrawFlags_RoundCornersNone);
    ImGui::GetWindowDrawList()->AddRectFilled(Pos + ImVec2(0, Size.y - FooterHeight), Pos + Size, ImGui::GetColorU32(ImGuiCol_ChildBg), style.WindowRounding, ImDrawFlags_RoundCornersBottom);

    if (style.WindowBorderSize > 0)
    {
        ImGui::GetWindowDrawList()->AddLine(Pos + ImVec2(0, HeaderHeight), Pos + ImVec2(Size.x, HeaderHeight), ImGui::GetColorU32(ImGuiCol_Border), style.WindowBorderSize);
        ImGui::GetWindowDrawList()->AddLine(Pos + ImVec2(0, Size.y - FooterHeight - style.WindowBorderSize), Pos + ImVec2(Size.x, Size.y - FooterHeight - style.WindowBorderSize), ImGui::GetColorU32(ImGuiCol_Border), style.WindowBorderSize);
    }

    ImGui::BeginChild(_("Tabs"), ImVec2(0, Size.y - FooterHeight), true);
    {
        ImGui::BeginGroup();
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::RadioFrameIcon(jrt::fonts.Icons, ICON_FA_CROSSHAIRS, &MenuTab, MenuPage_Aimbot, ImVec2(50, 50));
            ImGui::RadioFrameIcon(jrt::fonts.Icons, ICON_FA_EYE, &MenuTab, MenuPage_Visuals, ImVec2(50, 50));
            ImGui::RadioFrameIcon(jrt::fonts.Icons, ICON_FA_GEAR, &MenuTab, MenuPage_Misc, ImVec2(50, 50));
            ImGui::RadioFrameIcon(jrt::fonts.Icons, ICON_FA_BOMB, &MenuTab, MenuPage_Exploits, ImVec2(50, 50));

        }

        ImGui::EndGroup();

        ImGui::SameLine();

        if (MenuTab == 0)
        {
            ImGui::BeginChild(_("Aimbot"), ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
            {

                ImGui::Text(_("Category:"));
                ImGui::SameLine();
                ImGui::TextColored(ImColor(103, 0, 207), _("Aimbot"));
                ImGui::Separator();
                ImGui::Spacing();
           
                ImGui::Checkbox(_("Aimbot"), &jrt::settings.bAimbot);

                ImGui::Checkbox(_("Prediction"), &jrt::settings.bPrediction);

                ImGui::Checkbox(_("Visible Check"), &jrt::settings.bVisibleCheck);

                ImGui::Checkbox(_("Ignore Downed"), &jrt::settings.IgnoreDowned);

                ImGui::Checkbox(_("Ignore Dying"), &jrt::settings.bIgnoreDying);

                ImGui::Checkbox(_("Draw FOV"), &jrt::settings.bRenderFOV);

                ImGui::Spacing();

                ImGui::SliderFloat(_("FOV"), &jrt::settings.iFovRadius, 0, 30);

                ImGui::SliderFloat(_("Smooth"), &jrt::settings.fSmooth, 1, 15);

                ImGui::Spacing();

                ImGui::Combo(_("Hitbox"), &jrt::settings.iAimbone, AimBones, sizeof(AimBones) / sizeof(*AimBones));

                ImGui::Combo(_("Key"), &jrt::settings.iAimKeyPos, Keys, sizeof(Keys) / sizeof(*Keys));
            }
            ImGui::EndChild();
        }

        if (MenuTab == 1)
        {
            ImGui::BeginChild(_("Visuals"), ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
            {
                ImGui::Text(_("Category:"));
                ImGui::SameLine();
                ImGui::TextColored(ImColor(103, 0, 207), _("Visuals"));
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox(_("Enable"), &jrt::settings.bEnable);
                ImGui::Checkbox(_("Draw Box"), &jrt::settings.bBox);
                ImGui::Combo(_("Box Type"), &jrt::settings.iBoxType, "2D Box\0Corner 2D\0");
                ImGui::Checkbox(_("Filled"), &jrt::settings.bFilledBox);
                ImGui::Spacing();
                ImGui::Checkbox(_("Skeleton"), &jrt::settings.bSkeletons);
                ImGui::Checkbox(_("Weapon"), &jrt::settings.bHeldWeapon);
                ImGui::Checkbox(_("Username"), &jrt::settings.bUsername);
                ImGui::Checkbox(_("Platform"), &jrt::settings.bPlatform);
                ImGui::Checkbox(_("Rank"), &jrt::settings.bRanked);
                ImGui::Checkbox(_("Distance"), &jrt::settings.bDistance);
                ImGui::Spacing();
                
                ImGui::SliderFloat(_("Skeleton Thickness"), &jrt::settings.iSkeletonThickness, 0, 5);
                ImGui::SliderFloat(_("Box Thickness"), &jrt::settings.iBoxThickness, 0, 10);
            }
            ImGui::EndChild();
        }

        if (MenuTab == 2)
        {
            ImGui::BeginChild(_("Misc"), ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
            {
                ImGui::Text(_("Category:"));
                ImGui::SameLine();
                ImGui::TextColored(ImColor(103, 0, 207), _("Misc"));
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Checkbox(_("Outlined Skeleton"), &jrt::settings.bOutlinedSkeleton);

                ImGui::Text(_("Triggerbot:"));

                ImGui::Checkbox(_("Enable Triggerbot"), &jrt::settings.bTriggerbot);
                ImGui::Checkbox(_("Shotgun Only"), &jrt::settings.bShotgunOnly);

                ImGui::Combo(_("Key"), &jrt::settings.iTriggerKeyPos, Keys, sizeof(Keys) / sizeof(*Keys));

                ImGui::Spacing();

                ImGui::Text(_("Other:"));

                ImGui::Checkbox(_("Enable FOV Indicators"), &jrt::settings.bIndicators);
            }
            ImGui::EndChild();
        }

        if (MenuTab == 3)
        {
            ImGui::BeginChild(_("Exploits"), ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
            {
                ImGui::Text(_("Category:"));
                ImGui::SameLine();
                ImGui::TextColored(ImColor(103, 0, 207), _("Exploits"));
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Checkbox(_("Magic Bullet"), &jrt::settings.bMagicBullet);
                ImGui::Spacing();
                ImGui::SliderFloat(_("Magic Bullet Size"), &jrt::settings.fMagicBulletSize, 1, 4);

              
            }
            ImGui::EndChild();
        }
    }

    ImGui::EndChild();

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - FooterHeight);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);
    ImGui::BeginChild(_("Footer"), ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
    ImGui::PopStyleVar();
    {
        ImGui::SetCursorPosY(style.FramePadding.y - style.WindowBorderSize);
        ImGui::DoubleText(style.Colors[ImGuiCol_SliderGrab], style.Colors[ImGuiCol_SliderGrabActive], _("Paradise Fortnite"), _("Version 1.5"));
    }
    ImGui::EndChild();

    ImGui::End();
    ImGui::PushFont(nullptr);
    ImGui::EndFrame();
}

auto jrt::render_c::RenderThread( ) -> void
{
    ImVec4 vClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    const float fClearColor[ 4 ] = { vClearColor.x * vClearColor.w, vClearColor.y * vClearColor.w, vClearColor.z * vClearColor.w, vClearColor.w };
    auto& io = ImGui::GetIO( );

    SetThreadPriority( GetCurrentThread( ), THREAD_PRIORITY_HIGHEST );

    for ( ;; )
    {
        MSG msg;
        while ( PeekMessage( &msg, hWindowHandle, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );

            if (msg.message == WM_QUIT)
                return;
        }

        io.DeltaTime = 1.0f / 165.f;
        io.ImeWindowHandle = hWindowHandle;

        POINT p_cursor;
        GetCursorPos( &p_cursor );
        io.MousePos.x = p_cursor.x;
        io.MousePos.y = p_cursor.y;

        io.MouseDown[ 0 ] = ( SPOOF_CALL(GetAsyncKeyState)( VK_LBUTTON ) & 0x8000 ) != 0;
        io.MouseClicked[ 0 ] = io.MouseDown[ 0 ];

        io.WantCaptureMouse = io.WantCaptureKeyboard = io.WantCaptureMouse || io.WantCaptureKeyboard;

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );
        {
            jrt::visuals.ActorLoop( );

            if (jrt::settings.bRenderFOV)
            {
                ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(jrt::screen.fWidth / 2, jrt::screen.fHeight / 2), jrt::settings.iFovRadius * 10 + 1.0f, ImColor(0, 0, 0), 64, 1.3f);
                ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(jrt::screen.fWidth / 2, jrt::screen.fHeight / 2), jrt::settings.iFovRadius * 10, ImColor(255, 255, 255), 64, 1.0f);
            }
             
            this->RenderMenu( );
        }

        ImGui::Render( );
        D3DDeviceContext->OMSetRenderTargets( 1, &D3DRenderTarget, nullptr );
        D3DDeviceContext->ClearRenderTargetView( D3DRenderTarget, fClearColor );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        D3DSwapChain->Present( 1, 0 );
    }

    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    if ( D3DRenderTarget )
        D3DRenderTarget->Release( );

    if ( D3DSwapChain )
        D3DSwapChain->Release( );

    if ( D3DDeviceContext )
        D3DDeviceContext->Release( );

    DestroyWindow( hWindowHandle );
}