#include <random>
#include <stringapiset.h>
#include <dependencies/framework/imgui.h>

inline std::wstring RandomString(size_t length)
{
    const wchar_t charset[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::default_random_engine engine(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(0, std::wcslen(charset) - 1);

    std::wstring randomString;
    for (size_t i = 0; i < length; ++i) {
        randomString += charset[dist(engine)];
    }
    return randomString;
}

inline std::string WStringToString(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

inline static const char* Keys[]
{
    "Left Mouse Button",
    "Right Mouse Button",
    "Middle Mouse Button",
    "Mouse Side 1",
    "Mouse Side 2",
    "Control-Break Processing",
    "Backspace",
    "Tab",
    "Clear",
    "Enter",
    "SHIFT",
    "CTRL",
    "ALT",
    "Caps Lock",
    "Esc",
    "Space",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply"
};


inline const char* AimBones[] =
{
    "Head",
    "Chest",
    "Neck",
    "Pelvis",
};

inline void GetAllKeys()
{
    if (jrt::settings.iAimKeyPos == 0)
    {
        jrt::settings.iAimbotKeybind = 0x01;
    }
    else if (jrt::settings.iAimKeyPos == 1)
    {
        jrt::settings.iAimbotKeybind = 0x02;
    }
    else if (jrt::settings.iAimKeyPos == 2)
    {
        jrt::settings.iAimbotKeybind = 0x04;
    }
    else if (jrt::settings.iAimKeyPos == 3)
    {
        jrt::settings.iAimbotKeybind = 0x05;
    }
    else if (jrt::settings.iAimKeyPos == 4)
    {
        jrt::settings.iAimbotKeybind = 0x06;
    }
    else if (jrt::settings.iAimKeyPos == 5)
    {
        jrt::settings.iAimbotKeybind = 0x03;
    }
    else if (jrt::settings.iAimKeyPos == 6)
    {
        jrt::settings.iAimbotKeybind = 0x08;
    }
    else if (jrt::settings.iAimKeyPos == 7)
    {
        jrt::settings.iAimbotKeybind = 0x09;
    }
    else if (jrt::settings.iAimKeyPos == 8)
    {
        jrt::settings.iAimbotKeybind = 0x0c;
    }
    else if (jrt::settings.iAimKeyPos == 9)
    {
        jrt::settings.iAimbotKeybind == 0x0D;
    }
    else if (jrt::settings.iAimKeyPos == 10)
    {
        jrt::settings.iAimbotKeybind = 0x10;
    }
    else if (jrt::settings.iAimKeyPos == 11)
    {
        jrt::settings.iAimbotKeybind = 0x11;
    }
    else if (jrt::settings.iAimKeyPos == 12)
    {
        jrt::settings.iAimbotKeybind == 0x12;
    }
    else if (jrt::settings.iAimKeyPos == 13)
    {
        jrt::settings.iAimbotKeybind == 0x14;
    }
    else if (jrt::settings.iAimKeyPos == 14)
    {
        jrt::settings.iAimbotKeybind == 0x1B;
    }
    else if (jrt::settings.iAimKeyPos == 15)
    {
        jrt::settings.iAimbotKeybind == 0x20;
    }
    else if (jrt::settings.iAimKeyPos == 16)
    {
        jrt::settings.iAimbotKeybind == 0x30;
    }
    else if (jrt::settings.iAimKeyPos == 17)
    {
        jrt::settings.iAimbotKeybind == 0x31;
    }
    else if (jrt::settings.iAimKeyPos == 18)
    {
        jrt::settings.iAimbotKeybind == 0x32;
    }
    else if (jrt::settings.iAimKeyPos == 19)
    {
        jrt::settings.iAimbotKeybind == 0x33;
    }
    else if (jrt::settings.iAimKeyPos == 20)
    {
        jrt::settings.iAimbotKeybind == 0x34;
    }
    else if (jrt::settings.iAimKeyPos == 21)
    {
        jrt::settings.iAimbotKeybind == 0x35;
    }
    else if (jrt::settings.iAimKeyPos == 22)
    {
        jrt::settings.iAimbotKeybind == 0x36;
    }
    else if (jrt::settings.iAimKeyPos == 23)
    {
        jrt::settings.iAimbotKeybind == 0x37;
    }
    else if (jrt::settings.iAimKeyPos == 24)
    {
        jrt::settings.iAimbotKeybind == 0x38;
    }
    else if (jrt::settings.iAimKeyPos == 25)
    {
        jrt::settings.iAimbotKeybind == 0x39;
    }
    else if (jrt::settings.iAimKeyPos == 26)
    {
        jrt::settings.iAimbotKeybind == 0x41;
    }
    else if (jrt::settings.iAimKeyPos == 27)
    {
        jrt::settings.iAimbotKeybind == 0x42;
    }
    else if (jrt::settings.iAimKeyPos == 28)
    {
        jrt::settings.iAimbotKeybind == 0x43;
    }
    else if (jrt::settings.iAimKeyPos == 29)
    {
        jrt::settings.iAimbotKeybind == 0x44;
    }
    else if (jrt::settings.iAimKeyPos == 30)
    {
        jrt::settings.iAimbotKeybind == 0x45;
    }
    else if (jrt::settings.iAimKeyPos == 31)
    {
        jrt::settings.iAimbotKeybind == 0x46;
    }
    else if (jrt::settings.iAimKeyPos == 32)
    {
        jrt::settings.iAimbotKeybind == 0x47;
    }
    else if (jrt::settings.iAimKeyPos == 33)
    {
        jrt::settings.iAimbotKeybind == 0x48;
    }
    else if (jrt::settings.iAimKeyPos == 34)
    {
        jrt::settings.iAimbotKeybind == 0x49;
    }
    else if (jrt::settings.iAimKeyPos == 35)
    {
        jrt::settings.iAimbotKeybind == 0x4A;
    }
    else if (jrt::settings.iAimKeyPos == 36)
    {
        jrt::settings.iAimbotKeybind == 0x4B;
    }
    else if (jrt::settings.iAimKeyPos == 37)
    {
        jrt::settings.iAimbotKeybind == 0x4C;
    }
    else if (jrt::settings.iAimKeyPos == 38)
    {
        jrt::settings.iAimbotKeybind == 0x4D;
    }
    else if (jrt::settings.iAimKeyPos == 39)
    {
        jrt::settings.iAimbotKeybind == 0x4E;
    }
    else if (jrt::settings.iAimKeyPos == 40)
    {
        jrt::settings.iAimbotKeybind == 0x4F;
    }
    else if (jrt::settings.iAimKeyPos == 41)
    {
        jrt::settings.iAimbotKeybind == 0x50;
    }
    else if (jrt::settings.iAimKeyPos == 42)
    {
        jrt::settings.iAimbotKeybind == 0x51;
    }
    else if (jrt::settings.iAimKeyPos == 43)
    {
        jrt::settings.iAimbotKeybind == 0x52;
    }
    else if (jrt::settings.iAimKeyPos == 44)
    {
        jrt::settings.iAimbotKeybind == 0x53;
    }
    else if (jrt::settings.iAimKeyPos == 45)
    {
        jrt::settings.iAimbotKeybind == 0x54;
    }
    else if (jrt::settings.iAimKeyPos == 46)
    {
        jrt::settings.iAimbotKeybind == 0x55;
    }
    else if (jrt::settings.iAimKeyPos == 47)
    {
        jrt::settings.iAimbotKeybind == 0x56;
    }
    else if (jrt::settings.iAimKeyPos == 48)
    {
        jrt::settings.iAimbotKeybind == 0x57;
    }
    else if (jrt::settings.iAimKeyPos == 49)
    {
        jrt::settings.iAimbotKeybind == 0x58;
    }
    else if (jrt::settings.iAimKeyPos == 50)
    {
        jrt::settings.iAimbotKeybind == 0x59;
    }
    else if (jrt::settings.iAimKeyPos == 51)
    {
        jrt::settings.iAimbotKeybind == 0x5A;
    }
    else if (jrt::settings.iAimKeyPos == 52)
    {
        jrt::settings.iAimbotKeybind == 0x60;
    }
    else if (jrt::settings.iAimKeyPos == 53)
    {
        jrt::settings.iAimbotKeybind == 0x61;
    }
    else if (jrt::settings.iAimKeyPos == 54)
    {
        jrt::settings.iAimbotKeybind == 0x62;
    }
    else if (jrt::settings.iAimKeyPos == 55)
    {
        jrt::settings.iAimbotKeybind == 0x63;
    }
    else if (jrt::settings.iAimKeyPos == 56)
    {
        jrt::settings.iAimbotKeybind == 0x64;
    }
    else if (jrt::settings.iAimKeyPos == 57)
    {
        jrt::settings.iAimbotKeybind == 0x65;
    }
    else if (jrt::settings.iAimKeyPos == 58)
    {
        jrt::settings.iAimbotKeybind == 0x66;
    }
    else if (jrt::settings.iAimKeyPos == 59)
    {
        jrt::settings.iAimbotKeybind == 0x67;
    }
    else if (jrt::settings.iAimKeyPos == 60)
    {
        jrt::settings.iAimbotKeybind == 0x68;
    }
    else if (jrt::settings.iAimKeyPos == 61)
    {
        jrt::settings.iAimbotKeybind == 0x69;
    }
    else if (jrt::settings.iAimKeyPos == 62)
    {
        jrt::settings.iAimbotKeybind == 0x6A;
    }

    if (jrt::settings.iTriggerKeyPos == 0)
    {
        jrt::settings.iTriggerbotKeybind = 0x01;
    }
    else if (jrt::settings.iTriggerKeyPos == 1)
    {
        jrt::settings.iTriggerbotKeybind = 0x02;
    }
    else if (jrt::settings.iTriggerKeyPos == 2)
    {
        jrt::settings.iTriggerbotKeybind = 0x04;
    }
    else if (jrt::settings.iTriggerKeyPos == 3)
    {
        jrt::settings.iTriggerbotKeybind = 0x05;
    }
    else if (jrt::settings.iTriggerKeyPos == 4)
    {
        jrt::settings.iTriggerbotKeybind = 0x06;
    }
    else if (jrt::settings.iTriggerKeyPos == 5)
    {
        jrt::settings.iTriggerbotKeybind = 0x03;
    }
    else if (jrt::settings.iTriggerKeyPos == 6)
    {
        jrt::settings.iTriggerbotKeybind = 0x08;
    }
    else if (jrt::settings.iTriggerKeyPos == 7)
    {
        jrt::settings.iTriggerbotKeybind = 0x09;
    }
    else if (jrt::settings.iTriggerKeyPos == 8)
    {
        jrt::settings.iTriggerbotKeybind = 0x0c;
    }
    else if (jrt::settings.iTriggerKeyPos == 9)
    {
        jrt::settings.iTriggerbotKeybind == 0x0D;
    }
    else if (jrt::settings.iTriggerKeyPos == 10)
    {
        jrt::settings.iTriggerbotKeybind = 0x10;
    }
    else if (jrt::settings.iTriggerKeyPos == 11)
    {
        jrt::settings.iTriggerbotKeybind = 0x11;
    }
    else if (jrt::settings.iTriggerKeyPos == 12)
    {
        jrt::settings.iTriggerbotKeybind == 0x12;
    }
    else if (jrt::settings.iTriggerKeyPos == 13)
    {
        jrt::settings.iTriggerbotKeybind == 0x14;
    }
    else if (jrt::settings.iTriggerKeyPos == 14)
    {
        jrt::settings.iTriggerbotKeybind == 0x1B;
    }
    else if (jrt::settings.iTriggerKeyPos == 15)
    {
        jrt::settings.iTriggerbotKeybind == 0x20;
    }
    else if (jrt::settings.iTriggerKeyPos == 16)
    {
        jrt::settings.iTriggerbotKeybind == 0x30;
    }
    else if (jrt::settings.iTriggerKeyPos == 17)
    {
        jrt::settings.iTriggerbotKeybind == 0x31;
    }
    else if (jrt::settings.iTriggerKeyPos == 18)
    {
        jrt::settings.iTriggerbotKeybind == 0x32;
    }
    else if (jrt::settings.iTriggerKeyPos == 19)
    {
        jrt::settings.iTriggerbotKeybind == 0x33;
    }
    else if (jrt::settings.iTriggerKeyPos == 20)
    {
        jrt::settings.iTriggerbotKeybind == 0x34;
    }
    else if (jrt::settings.iTriggerKeyPos == 21)
    {
        jrt::settings.iTriggerbotKeybind == 0x35;
    }
    else if (jrt::settings.iTriggerKeyPos == 22)
    {
        jrt::settings.iTriggerbotKeybind == 0x36;
    }
    else if (jrt::settings.iTriggerKeyPos == 23)
    {
        jrt::settings.iTriggerbotKeybind == 0x37;
    }
    else if (jrt::settings.iTriggerKeyPos == 24)
    {
        jrt::settings.iTriggerbotKeybind == 0x38;
    }
    else if (jrt::settings.iTriggerKeyPos == 25)
    {
        jrt::settings.iTriggerbotKeybind == 0x39;
    }
    else if (jrt::settings.iTriggerKeyPos == 26)
    {
        jrt::settings.iTriggerbotKeybind == 0x41;
    }
    else if (jrt::settings.iTriggerKeyPos == 27)
    {
        jrt::settings.iTriggerbotKeybind == 0x42;
    }
    else if (jrt::settings.iTriggerKeyPos == 28)
    {
        jrt::settings.iTriggerbotKeybind == 0x43;
    }
    else if (jrt::settings.iTriggerKeyPos == 29)
    {
        jrt::settings.iTriggerbotKeybind == 0x44;
    }
    else if (jrt::settings.iTriggerKeyPos == 30)
    {
        jrt::settings.iTriggerbotKeybind == 0x45;
    }
    else if (jrt::settings.iTriggerKeyPos == 31)
    {
        jrt::settings.iTriggerbotKeybind == 0x46;
    }
    else if (jrt::settings.iTriggerKeyPos == 32)
    {
        jrt::settings.iTriggerbotKeybind == 0x47;
    }
    else if (jrt::settings.iTriggerKeyPos == 33)
    {
        jrt::settings.iTriggerbotKeybind == 0x48;
    }
    else if (jrt::settings.iTriggerKeyPos == 34)
    {
        jrt::settings.iTriggerbotKeybind == 0x49;
    }
    else if (jrt::settings.iTriggerKeyPos == 35)
    {
        jrt::settings.iTriggerbotKeybind == 0x4A;
    }
    else if (jrt::settings.iTriggerKeyPos == 36)
    {
        jrt::settings.iTriggerbotKeybind == 0x4B;
    }
    else if (jrt::settings.iTriggerKeyPos == 37)
    {
        jrt::settings.iTriggerbotKeybind == 0x4C;
    }
    else if (jrt::settings.iTriggerKeyPos == 38)
    {
        jrt::settings.iTriggerbotKeybind == 0x4D;
    }
    else if (jrt::settings.iTriggerKeyPos == 39)
    {
        jrt::settings.iTriggerbotKeybind == 0x4E;
    }
    else if (jrt::settings.iTriggerKeyPos == 40)
    {
        jrt::settings.iTriggerbotKeybind == 0x4F;
    }
    else if (jrt::settings.iTriggerKeyPos == 41)
    {
        jrt::settings.iTriggerbotKeybind == 0x50;
    }
    else if (jrt::settings.iTriggerKeyPos == 42)
    {
        jrt::settings.iTriggerbotKeybind == 0x51;
    }
    else if (jrt::settings.iTriggerKeyPos == 43)
    {
        jrt::settings.iTriggerbotKeybind == 0x52;
    }
    else if (jrt::settings.iTriggerKeyPos == 44)
    {
        jrt::settings.iTriggerbotKeybind == 0x53;
    }
    else if (jrt::settings.iTriggerKeyPos == 45)
    {
        jrt::settings.iTriggerbotKeybind == 0x54;
    }
    else if (jrt::settings.iTriggerKeyPos == 46)
    {
        jrt::settings.iTriggerbotKeybind == 0x55;
    }
    else if (jrt::settings.iTriggerKeyPos == 47)
    {
        jrt::settings.iTriggerbotKeybind == 0x56;
    }
    else if (jrt::settings.iTriggerKeyPos == 48)
    {
        jrt::settings.iTriggerbotKeybind == 0x57;
    }
    else if (jrt::settings.iTriggerKeyPos == 49)
    {
        jrt::settings.iTriggerbotKeybind == 0x58;
    }
    else if (jrt::settings.iTriggerKeyPos == 50)
    {
        jrt::settings.iTriggerbotKeybind == 0x59;
    }
    else if (jrt::settings.iTriggerKeyPos == 51)
    {
        jrt::settings.iTriggerbotKeybind == 0x5A;
    }
    else if (jrt::settings.iTriggerKeyPos == 52)
    {
        jrt::settings.iTriggerbotKeybind == 0x60;
    }
    else if (jrt::settings.iTriggerKeyPos == 53)
    {
        jrt::settings.iTriggerbotKeybind == 0x61;
    }
    else if (jrt::settings.iTriggerKeyPos == 54)
    {
        jrt::settings.iTriggerbotKeybind == 0x62;
    }
    else if (jrt::settings.iTriggerKeyPos == 55)
    {
        jrt::settings.iTriggerbotKeybind == 0x63;
    }
    else if (jrt::settings.iTriggerKeyPos == 56)
    {
        jrt::settings.iTriggerbotKeybind == 0x64;
    }
    else if (jrt::settings.iTriggerKeyPos == 57)
    {
        jrt::settings.iTriggerbotKeybind == 0x65;
    }
    else if (jrt::settings.iTriggerKeyPos == 58)
    {
        jrt::settings.iTriggerbotKeybind == 0x66;
    }
    else if (jrt::settings.iTriggerKeyPos == 59)
    {
        jrt::settings.iTriggerbotKeybind == 0x67;
    }
    else if (jrt::settings.iTriggerKeyPos == 60)
    {
        jrt::settings.iTriggerbotKeybind == 0x68;
    }
    else if (jrt::settings.iTriggerKeyPos == 61)
    {
        jrt::settings.iTriggerbotKeybind == 0x69;
    }
    else if (jrt::settings.iTriggerKeyPos == 62)
    {
        jrt::settings.iTriggerbotKeybind == 0x6A;
    }
}