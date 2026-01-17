#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CASim : public Screen {
      public:
        CASim();
        ~CASim() override;
      };
    }
    
    template <>
    struct AppTraits<Apps::CASim> {
      static constexpr Apps app = Apps::CASim;
      static constexpr const char* icon = Screens::Symbols::eye;
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::CASim();
      }
    };
  }
}