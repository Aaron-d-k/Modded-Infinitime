#include "RenderStuff.h"
#include "displayapp/LittleVgl.h"

#include <climits>


using namespace Pinetime::Applications::Screens;

RenderStuff::RenderStuff(Pinetime::Components::LittleVgl& lvgl) : lvgl(lvgl), particlebuffer({}), drawbuffer({}) {
  title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Hello!");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

  drawbuffer.fill(LV_COLOR_BLUE);

  taskRefresh = lv_task_create(RefreshTaskCallback, 200, LV_TASK_PRIO_LOW, this);
  currscanline = 0;
}

RenderStuff::~RenderStuff() {
  lv_obj_clean(lv_scr_act());
  lv_task_del(taskRefresh);
}


void Pinetime::Applications::Screens::RenderStuff::Refresh() 
{
    if (lvgl.IsScrolling()) {
        return;
    }

    lv_area_t area;
    area.x1 = 0;
    area.x2 = screenS-1;
    area.y1 = currscanline*scanlineH;
    area.y1 = currscanline*scanlineH + scanlineH - 1;
    lvgl.FlushDisplay(&area, drawbuffer.data());

    currscanline++;
    currscanline %= screenS/scanlineH;    
}