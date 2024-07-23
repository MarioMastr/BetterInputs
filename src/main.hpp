#include <string>
#include <string_view>

#include <Geode/utils/cocos.hpp>

#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/CCIMEDispatcher.hpp>

#include <Geode/binding/TextArea.hpp>
#include <Geode/binding/MultilineBitmapFont.hpp>

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/cocos/robtop/glfw/glfw3.h>
#elif defined(GEODE_IS_MACOS)
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>

#include "types/TouchMessageType.hpp"
#endif

#include <Geode/modify/CCTextFieldTTF.hpp>
#include <Geode/modify/CCScene.hpp>

#include "types/HighlightedString.hpp"
#include "types/CharNode.hpp"
#include "types/InputNodeTextAreaInfo.hpp"

#include "utils.hpp"

using namespace geode::prelude;

struct BetterTextInputNode : Modify<BetterTextInputNode, CCTextInputNode> {
    struct Fields {
        // the actual string, because we basically rework everything about the input text insertion is broken :D
        // (getString() normally returns the last character or is empty, we hook CCTextFieldTTF::getString to fix this)
        std::string m_string = "";

        // current cursor position
        int m_pos = -1;

        HighlightedString m_highlighted;
        bool m_is_adding_to_highlight = false;
        std::vector<cocos2d::extension::CCScale9Sprite *> m_highlights;

        bool m_use_update_blink_pos = false;
    };

    bool init(float p0, float p1, char const *p2, char const *p3, int p4, char const *p5);
    bool onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *tField);
    bool onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF *tField);
    void updateBlinkLabelToChar(int pos);
    void setString(gd::string str);
    void textAreaCursorFix(std::size_t pos);
    void onRightArrowKey(bool isCtrl, bool isShift);
    void onLeftArrowKey(bool isCtrl, bool isShift);
    void onHomeKey(bool isShift);
    void onEndKey(bool isShift);
    void onDelete(bool isCtrl, bool isDel);
    void onCopy();
    void onPaste();
    void onCut();
    void onStringEmpty();
    void useUpdateBlinkPos(bool toggle);
    void showTextOrPlaceholder(bool toggle);
    int getCursorPos();
    const HighlightedString &getHighlighted();
    void highlightFromToPos(int from, int to);
    void insertCharAtPos(int pos, char character);
    void insertStrAtPos(int pos, std::size_t len, const std::string &str);
    void deletePos(int pos, bool isDel);
    CharNode getCharNodePosInfo(std::size_t pos, bool isLeftAnchored);
    CharNode getCharNodePosInfoAtLine(std::size_t pos, std::size_t line, bool isLeftAnchored);
    InputNodeTextAreaInfo getTextLabelInfoFromPos(std::size_t pos);
    void setAndUpdateString(const std::string &str);
    void updateBlinkLabelToCharForced(int pos);
    void clearHighlight();
    void deselectInput();
    cocos2d::extension::CCScale9Sprite *appendHighlightNode();
    void removeLastHighlightNode();
    [[nodiscard]] int getAndSetNextPos();
    [[nodiscard]] int getAndSetPreviousPos();
};

struct BetterCCTextFieldTTF : Modify<BetterCCTextFieldTTF, CCTextFieldTTF> {
    const char *getString();
};

struct AlertLayerFix : Modify<AlertLayerFix, CCScene> {
    struct Fields {
        std::size_t m_previous_scene_children_count = 0;
        CCLayer *m_outermost_input_parent;
    };

    static CCScene *create();
    void onUpdateTick(float);
};

struct BetterCCIMEDispatcher : Modify<BetterCCIMEDispatcher, CCIMEDispatcher> {
    void dispatchDeleteBackward();
    void dispatchDeleteForward();
    void dispatchInsertText(const char *text, int len, cocos2d::enumKeyCodes keyCode);
};

#if defined(GEODE_IS_WINDOWS) // i need me some consistency

struct BetterCCEGLView : Modify<BetterCCEGLView, CCEGLView> {
    void onGLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void onGLFWMouseCallBack(GLFWwindow *window, int button, int action, int mods);
}

#elif defined(GEODE_IS_MACOS)

struct ModifierKeysState {
    bool ctrlDown;
    bool shiftDown;
};

static ModifierKeysState g_modifier_keys_state;

struct BetterKeyboardDispatcher : Modify<BetterKeyboardDispatcher, CCKeyboardDispatcher> {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isKeyDown, bool isKeyRepeat);
};

struct BetterTouchDispatcher : Modify<BetterTouchDispatcher, CCTouchDispatcher> {
    void touches(cocos2d::CCSet *touches, cocos2d::CCEvent *event, unsigned int type);
};

#endif