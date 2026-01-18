#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class RenderStuff : public Screen {
      public:
        RenderStuff(Pinetime::Components::LittleVgl& lvgl);
        ~RenderStuff() override;

        void Refresh() override;

        static constexpr size_t scanlineH = 4;
        static constexpr size_t screenS = 240;
        
      private:
        lv_task_t* taskRefresh;
        lv_obj_t* title;
        Pinetime::Components::LittleVgl& lvgl;
        
        std::array<lv_color_t, screenS*scanlineH> drawbuffer;
        std::array<std::array<uint8_t,16>,screenS/scanlineH> particlebuffer;

        size_t currscanline;

        static constexpr size_t mem_usage = sizeof(particlebuffer)+sizeof(drawbuffer);
        static_assert(mem_usage < 5000);


      };
    }
    
    template <>
    struct AppTraits<Apps::RenderStuff> {
      static constexpr Apps app = Apps::RenderStuff;
      static constexpr const char* icon = "R";
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::RenderStuff(controllers.lvgl);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      };
    };
  }
}