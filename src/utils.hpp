#include <string>
#include <string_view>

#include <Geode/utils/cocos.hpp>
#include <Geode/loader/Mod.hpp>

#include <Geode/cocos/robtop/glfw/glfw3.h>

#ifdef GEODE_IS_WINDOWS
#include <WinUser.h> // virtual keys
#elif defined(GEODE_IS_MACOS)
#include <CoreGraphics/CoreGraphics.h>
enum {
  Return                    = 0x24,
  Tab                       = 0x30,
  Space                     = 0x31,
  Delete                    = 0x33,
  Escape                    = 0x35,
  Command                   = 0x37,
  Shift                     = 0x38,
  CapsLock                  = 0x39,
  Option                    = 0x3A,
  Control                   = 0x3B,
  RightCommand              = 0x36,
  RightShift                = 0x3C,
  RightOption               = 0x3D,
  RightControl              = 0x3E,
  Function                  = 0x3F,
  F17                       = 0x40,
  VolumeUp                  = 0x48,
  VolumeDown                = 0x49,
  Mute                      = 0x4A,
  F18                       = 0x4F,
  F19                       = 0x50,
  F20                       = 0x5A,
  F5                        = 0x60,
  F6                        = 0x61,
  F7                        = 0x62,
  F3                        = 0x63,
  F8                        = 0x64,
  F9                        = 0x65,
  F11                       = 0x67,
  F13                       = 0x69,
  F16                       = 0x6A,
  F14                       = 0x6B,
  F10                       = 0x6D,
  ContextualMenu            = 0x6E,
  F12                       = 0x6F,
  F15                       = 0x71,
  Help                      = 0x72,
  Home                      = 0x73,
  PageUp                    = 0x74,
  ForwardDelete             = 0x75,
  F4                        = 0x76,
  End                       = 0x77,
  F2                        = 0x78,
  PageDown                  = 0x79,
  F1                        = 0x7A,
  LeftArrow                 = 0x7B,
  RightArrow                = 0x7C,
  DownArrow                 = 0x7D,
  UpArrow                   = 0x7E
}; // we do this so that we don't have to include the entire carbon header, which causes a conflict with some enums (all the necessary keys are here anyways :D)
#endif

#include <Geode/cocos/cocoa/CCGeometry.h>

namespace BI
{
namespace utils
{
constexpr std::string_view SEPARATORS = " !@#%&*()-_{}[]\\/?.,:;\"";

inline const std::string &insertCharAtIndex(std::string &str, int pos, char toInsert)
{
    if (str.empty())
        pos = 0;
    else if (pos == -1)
        pos = str.size();

    std::string curStr{ std::move(str) };

    if (curStr.capacity() < curStr.size() + 1) {
        curStr.reserve(curStr.size() * 2);
        curStr.resize(curStr.size());
    }

    str = std::move(curStr);

    str.insert(pos, 1, toInsert);

    return str;
}

inline const std::string &insertStrAtIndex(std::string &str, int pos, const std::string_view toInsert)
{
    if (str.empty())
        pos = 0;
    else if (pos == -1)
        pos = str.size();

    std::string curStr{ std::move(str) };

    if (curStr.capacity() < curStr.size() + toInsert.size()) {
        str.reserve((curStr.size() + toInsert.size()) * 2);
        str.resize(curStr.size() + toInsert.size());
    }

    str = std::move(curStr);

    str.insert(pos, toInsert);

    return str;
}

inline int findNextSeparator(std::string_view str, int pos)
{
    if (pos == -1)
        pos = str.size();

    str = str.substr(pos, str.size());

    std::size_t sepPos = str.find_first_of(SEPARATORS);

    // | is the pos
    // example|.com -> example.|com
    if (pos + sepPos == pos)
        return pos + 1;

    return sepPos == std::string_view::npos ? -1 : (pos + sepPos);
}

inline int findPreviousSeparator(std::string_view str, int pos)
{
    if (pos == -1)
        pos = str.size();

    str = str.substr(0, pos);

    std::string reversedString{ str.rbegin(), str.rend() };

    std::size_t sepPos = reversedString.find_first_of(SEPARATORS);

    // | is the pos
    // example.|com -> example|.com
    if (str.size() - sepPos == pos)
        return pos - 1;

    return sepPos == std::string::npos ? 0 : (str.size() - sepPos);
}
} // namespace utils

namespace cocos
{
inline cocos2d::CCPoint getMousePosition()
{
#ifdef GEODE_IS_WINDOWS
    auto *director = cocos2d::CCDirector::sharedDirector();
    auto *gl       = director->getOpenGLView();
    auto winSize   = director->getWinSize();
    auto frameSize = gl->getFrameSize();
    auto mouse     = gl->getMousePosition() / frameSize;

    return cocos2d::CCPoint{ mouse.x, 1.f - mouse.y } * winSize;
#elif defined(GEODE_IS_MACOS)
    return geode::cocos::getMousePos();
#endif
}

inline bool isPositionInNode(cocos2d::CCNode *node, const cocos2d::CCPoint &pos)
{
    auto worldPos = node->getParent()->convertToWorldSpace(node->getPosition());
    auto size     = node->getScaledContentSize();

    auto rect = cocos2d::CCRect{ worldPos.x - size.width / 2, worldPos.y - size.height / 2, size.width, size.height };

    return rect.containsPoint(pos);
}
} // namespace cocos

enum class PlatformKey { LEFT_CONTROL, LEFT_SHIFT };
namespace platform
{
inline bool keyDown(PlatformKey key)
{
#ifdef GEODE_IS_WINDOWS
    switch (key) {
        case BI::PlatformKey::LEFT_CONTROL: return GetKeyState(VK_CONTROL) & 0x8000;
        case BI::PlatformKey::LEFT_SHIFT: return GetKeyState(VK_SHIFT) & 0x8000;
    }
#elif defined(GEODE_IS_MACOS)
    switch (key) {
        case BI::PlatformKey::LEFT_CONTROL: return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, Control);
        case BI::PlatformKey::LEFT_SHIFT: return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, Shift);
    }
#endif
}
} // namespace platform

namespace geode
{
template <typename T> T get(std::string_view value) { return ::geode::Mod::get()->getSettingValue<T>(value); }
} // namespace geode
} // namespace BI
