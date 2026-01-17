#include "displayapp/screens/CASim.h"
#include "CASim.h"
#include <climits>


using namespace Pinetime::Applications::Screens;

CASim::CASim(System::SystemTask* systemTask) : wakeLock(*systemTask) {
  title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Fear.");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);


  canvas = lv_canvas_create(lv_scr_act(), nullptr);
  drawbuff.fill(0);
  lv_canvas_set_buffer(canvas, drawbuff.data(), Wcanvas, Hcanvas, LV_IMG_CF_INDEXED_2BIT);
  lv_canvas_set_palette(canvas, 0, LV_COLOR_BLACK);
  lv_canvas_set_palette(canvas, 1, LV_COLOR_ORANGE);
  lv_canvas_set_palette(canvas, 2, LV_COLOR_ORANGE);
  lv_canvas_set_palette(canvas, 3, LV_COLOR_YELLOW);
  lv_obj_align(canvas, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -90);

  taskRefresh = lv_task_create(RefreshTaskCallback, 200, LV_TASK_PRIO_LOW, this);
  wakeLock.Lock(); //released automatically in destructor.
  tickrest=10;
  CAstate = {0x10, 0x100, 0x100, 0x100};
  rng.x = xTaskGetTickCount();
}

CASim::~CASim() {
  lv_obj_clean(lv_scr_act());
  lv_task_del(taskRefresh);
}

//v. slow
template<typename T>
T ECA(T board, int rule)
{
    T p = board>>1, q = board, r = board<<1, o = 0;
    for (size_t i = 0; i < sizeof(T)*CHAR_BIT; i++)
    {
        T neigh = (((p>>i)&1)<<2) + (((q>>i)&1)<<1) + ((r>>i)&1);
        o |= ((rule>>neigh)&1)<<i;
    }
    return o;
}

void Pinetime::Applications::Screens::CASim::Refresh() 
{
    uint32_t* pixdat = drawbuff.data() + 4; //apparently there is something in first 16 bytes... Couldn't find in docs. My suspicion is that it might be the colour palette?
    constexpr size_t row_stride = (Wcanvas*2)/32;
    constexpr int shiftsize = 4;
    constexpr int Hscalefactor = 2;

    if (drawbuff.size()<Hcanvas*row_stride+4) return;

    CAstate.bigmask = ECA<uint8_t>(CAstate.bigmask,54);
    CAstate.mask1 = ECA<uint16_t>(CAstate.mask1,30);
    CAstate.mask2 = ECA<uint16_t>(CAstate.mask2,110);
    CAstate.colca = ECA<uint16_t>(CAstate.mask2, 90);

    //lvgl uses bug-endian lol (cry)
    for (size_t r = 0; r < Hcanvas; r++)
    {
        for (size_t i = 0; i < row_stride-1; i++)
        {
            pixdat[row_stride*r + i] = __builtin_bswap32((__builtin_bswap32(pixdat[row_stride*r + i])<<shiftsize) | ((pixdat[row_stride*r + i + 1]>>(8-shiftsize))&((1<<shiftsize)-1)));
        }
        size_t ca_idx = r/Hscalefactor;
        uint32_t newstate = ((CAstate.bigmask>>ca_idx)&1)*( ((CAstate.mask1>>r)&1) + 4*((CAstate.mask2>>r)&1) + 10*((CAstate.colca>>r)&1));
        pixdat[row_stride*r + row_stride - 1] = __builtin_bswap32((__builtin_bswap32(pixdat[row_stride*r + row_stride - 1])<<shiftsize) | newstate);
    }

    CAstate.bigmask ^= uint8_t(((rng()&rng()&rng()&rng())>>8)&0xFF);  //and'ing to reduce density
    CAstate.mask1 ^= uint16_t(((rng()&rng()&rng()&rng())>>8)&0xFFFF); //low bits supposedly have lower randomness.
    CAstate.mask2 ^= uint16_t(((rng()&rng()&rng()&rng())>>8)&0xFFFF);
    CAstate.colca ^= uint16_t(((rng()&rng()&rng()&rng())>>8)&0xFFFF);
    
    lv_obj_invalidate(canvas);
    
    if (tickrest>0)
    {
        tickrest--;
        return;
    }

    idx++;
    if (idx>=text.size())
    {
        idx=1;
    }
    currword.fill(0);
    size_t wordstart = idx;
    while (idx < text.size() && ((idx-wordstart)<=1 || !isspace(text[idx])))
    {
        if (idx-wordstart+1<currword.size()) 
        {
            currword[idx-wordstart] = text[idx];
            if (text[idx]=='.')
            {
                tickrest += 5;
            }
            if (text[idx]==',')
            {
                tickrest += 1;
            }
        }
        idx++;
    }
    if (idx == text.size()) tickrest += 100;

    lv_label_set_text_static(title, currword.data());
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(title, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}