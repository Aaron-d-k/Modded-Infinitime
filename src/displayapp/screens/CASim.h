#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include "systemtask/WakeLock.h"

#include <array>
#include <string>

struct fastRNG
{
    uint32_t x;

    fastRNG(uint32_t rng_init)
    {
        x=rng_init;
    }

    void next_state()
    {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
    }
    
    uint32_t operator()()
    {
        next_state();
        return x;
    }
};

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CASim : public Screen {
      public:
        CASim(System::SystemTask* systemTask);
        ~CASim() override;

        void Refresh() override;

        static constexpr std::string_view text = R"( I must not fear. Fear is the mind-killer. Fear is the little-death that brings total obliteration. I will face my fear. I will permit it to pass over me and through me. And when it has gone past I will turn the inner eye to see its path. Where the fear has gone there will be nothing. Only I will remain.)";
        

      private:
        lv_task_t* taskRefresh;
        lv_obj_t* title;
        lv_obj_t* canvas;
        static constexpr size_t Wcanvas = 192;
        static constexpr size_t Hcanvas = 16;
        static constexpr size_t buffsize = LV_CANVAS_BUF_SIZE_INDEXED_2BIT(Wcanvas,Hcanvas);
        std::array<uint32_t, (buffsize-1)/4+1> drawbuff;

        std::array<char, 24> currword;
        size_t idx = 0;
        int tickrest = 0;
        Pinetime::System::WakeLock wakeLock;
        fastRNG rng = {100};
        
        struct {
            uint8_t bigmask;
            uint16_t mask1;
            uint16_t mask2;
            uint16_t colca;
        } CAstate;
      };
    }
    
    template <>
    struct AppTraits<Apps::CASim> {
      static constexpr Apps app = Apps::CASim;
      static constexpr const char* icon = "F";
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::CASim(controllers.systemTask);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      };
    };
  }
}