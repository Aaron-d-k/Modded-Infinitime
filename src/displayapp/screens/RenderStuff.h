#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include "systemtask/WakeLock.h"
#include "InfiniPaint.h"


#include <array>

#include "fastrng.h"

struct cell_line
{
    static constexpr size_t N = 240;
    static constexpr size_t BackingArrSize = (N-1)/32+1;
    std::array<uint32_t,BackingArrSize> data;

    cell_line operator ~() const
    {
        cell_line outp;
        for (size_t i = 0; i < data.size(); i++)
        {
            outp.data[i] = ~data[i];
        }
        return outp;
    }

    cell_line operator |(const cell_line& oth) const
    {
        cell_line outp;
        for (size_t i = 0; i < data.size(); i++)
        {
            outp.data[i] = data[i]|oth.data[i];
        }
        return outp;
    }

    cell_line operator &(const cell_line& oth) const
    {
        cell_line outp;
        for (size_t i = 0; i < data.size(); i++)
        {
            outp.data[i] = data[i]&oth.data[i];
        }
        return outp;
    }

    cell_line operator ^(const cell_line& oth) const
    {
        cell_line outp;
        for (size_t i = 0; i < data.size(); i++)
        {
            outp.data[i] = data[i]^oth.data[i];
        }
        return outp;
    }

    void operator |=(const cell_line& oth)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            data[i]|=oth.data[i];
        }
    }

    void operator &=(const cell_line& oth)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            data[i]&=oth.data[i];
        }
    }

    void operator ^=(const cell_line& oth)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            data[i]^=oth.data[i];
        }
    }

    cell_line operator >>(int s) const
    {
        cell_line outp;
        for (size_t i = 0; i < data.size()-1; i++)
        {
            outp.data[i] = (data[i]>>s)|(data[i+1]<<(32-s));
        }
        outp.data.back() = (data.back()>>s);
        return outp;
    }

    cell_line operator <<(int s) const
    {
        cell_line outp;
        outp.data[0] = (data[0]<<s);
        for (size_t i = 1; i < data.size(); i++)
        {
            outp.data[i] = (data[i]<<s)|(data[i-1]>>(32-s));
        }
        outp.cleanup();
        return outp;
    }

    void cleanup()
    {
        data.back() &= (1<<(32-data.size()*32+N)) - 1;
    }
};

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class RenderStuff : public Screen {
      public:
        RenderStuff(Pinetime::Components::LittleVgl* lvgl, System::SystemTask* systemTask);
        ~RenderStuff() override;


        void Refresh() override;

        static constexpr size_t scanlineH = 4;
        static constexpr size_t screenS = 240;
        static constexpr cell_line ZERO = {};

        enum class Rule {
            CGOL,
            ANNEAL,
            VOTE,
        } rule = Rule::VOTE;

        bool OnTouchEvent(TouchEvents event) override;
        
      private:
        Pinetime::System::WakeLock wakeLock;
        lv_task_t* taskRefresh;
        lv_obj_t* title;
        Pinetime::Components::LittleVgl* lvgl;
        fastRNG rng;
        
        std::array<lv_color_t, screenS*scanlineH> drawbuffer;

        std::array<cell_line, screenS> cgolbuffer;
        cell_line prevLine;
        size_t nextscanline;
        void perform_scan();
        
        std::array<cell_line,3> sumlo;
        std::array<cell_line,3> sumhi;
        std::array<cell_line,4> accum;
        std::array<cell_line,3> accum2;
        void IterateState(std::array<const cell_line*, 3> state, cell_line& o);

        

        size_t currchunk;

        static constexpr size_t mem_usage = sizeof(drawbuffer)+sizeof(cgolbuffer);
        static_assert(mem_usage < 10000);//10KB pls pls pls


      };
    }
    
    template <>
    struct AppTraits<Apps::RenderStuff> {
      static constexpr Apps app = Apps::RenderStuff;
      static constexpr const char* icon = "R";
      static Screens::Screen* Create(AppControllers& controllers) {
        auto newscreen = new (std::nothrow) Screens::RenderStuff(&controllers.lvgl, controllers.systemTask);
        if (newscreen!=nullptr) return newscreen;
        else return new Screens::InfiniPaint(controllers.lvgl, controllers.motorController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      };
    };
  }
}