#pragma once



// ***************
//  STD Libraries
// ***************

#include <string>
#include <unordered_map>



// ***************
//  Common Header
// ***************

#include "common_header.h"



// **********************
//  Initialization Flags
// **********************

#define SWM_INIT_MINIMAL            0x00
#define SWM_INIT_CL                 0x01
#define SWM_INIT_RENDER             0x02
#define SWM_INIT_PHYSICS            0x04

#define SWM_INIT_ALL                0xFF



// ***********
//  API Begin
// ***********

namespace Swarm {

    namespace Core {

        //! Performs an initialization of the Swarm Engine
        /*!
         * Initializes the Swarm Engine. By default, all aspects and modules of the Engine are initialized when this
         * function is called. Optionally, specific flags can be used in order to initialize only certain modules.
         *
         * \param flags an optional bitwise collection of flags used to initialize only certain modules
         * \return true if the Engine was successfully initialized, false otherwise
         */
        bool init(size_t flags = SWM_INIT_ALL);

        //! Cleans up the Engine
        /*!
         * Peforms a full cleanup of the Engine's resources after it is done being used.
         */
        void cleanup();

        enum State {
            UNINITIALIZED,
            STOPPED,
            RUNNING
        };

        State state();

        typedef void (*GameCycleFunc)(double);

        void start(GameCycleFunc function);
        void stop();

    }

    namespace Config {

        //! A configuration group
        /*!
         * A RawConfigData object is a collection of data values representing configuration options.
         */
        class RawConfigData {
        public:
            RawConfigData(const char * filepath);
            ~RawConfigData();

            //! Writes this Raw Configuration Data to a file
            /*!
             * Writes the entire of this Raw Configuration Data to a file.
             * The format is simple, and consists of each key/value pair on a seperate line seperated by a single '='.
             * Writes to the specified filepath.
             */
            void writeConfigData(const char * filepath);

            //! Writes this Raw Configuration Data to a file
            /*!
             * Writes the entire of this Raw Configuration Data to a file.
             * The format is simple, and consists of each key/value pair on a seperate line seperated by a single '='.
             * Writes to the default filepath.
             */
            void writeConfigData() { writeConfigData(filepath); }


            //! Retrieves an Integer from the stored Configuration Data.
            /*!
             * Retrieves an Integer associated with the String key from the internal Integer map. If there is no mapping
             * related to the specified key, it creates a new mapping with the given default value and returns that value.
             */
            int getInteger(std::string key, int defaultValue = 0);
            //! Sets an Integer to the stored Configuration Data.
            /*!
             * Maps the specified value to the specified key in the internal Integer map.
             */
            void setInteger(std::string key, int value);

            //~ Retrieves a String from the stored Configuration Data.
            /*!
             * Retrives a String associated with the String key from the internal String map. If there is no mapping
             * related to the specified key, it creates a new mapping with the given default value and returns that value.
             */
            std::string getString(std::string key, std::string defaultValue = "");
            //! Sets a String to the stored Configuration Data.
            /*!
             * Maps the specified value to the specified key in the internal String map.
             */
            void setString(std::string key, std::string value);
        protected:
            const char * filepath;

            std::unordered_map<std::string, int> dataMap_int;
            std::unordered_map<std::string, std::string> dataMap_str;
        };

    }

    namespace Input {

        class KeyType {
        public:
            int keyCode() const { return _key; };
            operator int() const { return _key; };

            static KeyType K_SPACE;
            static KeyType K_APOSTROPHE;
            static KeyType K_COMMA;
            static KeyType K_MINUS;
            static KeyType K_PERIOD;
            static KeyType K_SLASH;
            static KeyType K_0;
            static KeyType K_1;
            static KeyType K_2;
            static KeyType K_3;
            static KeyType K_4;
            static KeyType K_5;
            static KeyType K_6;
            static KeyType K_7;
            static KeyType K_8;
            static KeyType K_9;
            static KeyType K_SEMICOLON;
            static KeyType K_EQUAL;
            static KeyType K_A;
            static KeyType K_B;
            static KeyType K_C;
            static KeyType K_D;
            static KeyType K_E;
            static KeyType K_F;
            static KeyType K_G;
            static KeyType K_H;
            static KeyType K_I;
            static KeyType K_J;
            static KeyType K_K;
            static KeyType K_L;
            static KeyType K_M;
            static KeyType K_N;
            static KeyType K_O;
            static KeyType K_P;
            static KeyType K_Q;
            static KeyType K_R;
            static KeyType K_S;
            static KeyType K_T;
            static KeyType K_U;
            static KeyType K_V;
            static KeyType K_W;
            static KeyType K_X;
            static KeyType K_Y;
            static KeyType K_Z;
            static KeyType K_LEFT_BRACKET;
            static KeyType K_BACKSLASH;
            static KeyType K_RIGHT_BRACKET;
            static KeyType K_GRAVE_ACCENT;
            static KeyType K_WORLD_1;
            static KeyType K_WORLD_2;
            static KeyType K_ESCAPE;
            static KeyType K_ENTER;
            static KeyType K_TAB;
            static KeyType K_BACKSPACE;
            static KeyType K_INSERT;
            static KeyType K_DELETE;
            static KeyType K_RIGHT;
            static KeyType K_LEFT;
            static KeyType K_DOWN;
            static KeyType K_UP;
            static KeyType K_PAGE_UP;
            static KeyType K_PAGE_DOWN;
            static KeyType K_HOME;
            static KeyType K_END;
            static KeyType K_CAPS_LOCK;
            static KeyType K_SCROLL_LOCK;
            static KeyType K_NUM_LOCK;
            static KeyType K_PRINT_SCREEN;
            static KeyType K_PAUSE;
            static KeyType K_F1;
            static KeyType K_F2;
            static KeyType K_F3;
            static KeyType K_F4;
            static KeyType K_F5;
            static KeyType K_F6;
            static KeyType K_F7;
            static KeyType K_F8;
            static KeyType K_F9;
            static KeyType K_F10;
            static KeyType K_F11;
            static KeyType K_F12;
            static KeyType K_F13;
            static KeyType K_F14;
            static KeyType K_F15;
            static KeyType K_F16;
            static KeyType K_F17;
            static KeyType K_F18;
            static KeyType K_F19;
            static KeyType K_F20;
            static KeyType K_F21;
            static KeyType K_F22;
            static KeyType K_F23;
            static KeyType K_F24;
            static KeyType K_F25;
            static KeyType K_KP_0;
            static KeyType K_KP_1;
            static KeyType K_KP_2;
            static KeyType K_KP_3;
            static KeyType K_KP_4;
            static KeyType K_KP_5;
            static KeyType K_KP_6;
            static KeyType K_KP_7;
            static KeyType K_KP_8;
            static KeyType K_KP_9;
            static KeyType K_KP_DECIMAL;
            static KeyType K_KP_DIVIDE;
            static KeyType K_KP_MULTIPLY;
            static KeyType K_KP_SUBTRACT;
            static KeyType K_KP_ADD;
            static KeyType K_KP_ENTER;
            static KeyType K_KP_EQUAL;
            static KeyType K_LEFT_SHIFT;
            static KeyType K_LEFT_CONTROL;
            static KeyType K_LEFT_ALT;
            static KeyType K_LEFT_SUPER;
            static KeyType K_RIGHT_SHIFT;
            static KeyType K_RIGHT_CONTROL;
            static KeyType K_RIGHT_ALT;
            static KeyType K_RIGHT_SUPER;
            static KeyType K_MENU;

        private:
            KeyType(int key) : _key(key) {}
            int _key;
        };

        class Keybinding {
        public:
            Keybinding();
            Keybinding(int key, const std::string &name = "Unnamed");
            Keybinding(int key, Config::RawConfigData &configData, const std::string &name = "Unnamed");
            ~Keybinding();

            //! Get the pressed status of this Keybinding.
            bool pressed() const;
            operator bool() const { return pressed(); }
            //! Set the pressed status of this Keybinding.
            void setPressed(bool pressed) const;

            //! Get this Keybinding's assigned name.
            std::string name() const { return _name; }
            //! Get this Keybinding's key code.
            int key() const { return _key; }
            std::string keyName() const;

        protected:
            int _key;
            std::string _name;
        };

        void setKeyPressed(int key, bool pressed);
        bool keyPressed(int key);

        typedef void (*KeypressFunc)(int key);

        size_t registerKeypressAction(int key, KeypressFunc function);
        void unregisterKeypressAction(size_t actionID);
        void unregisterAllKeypressActions();

    }

}