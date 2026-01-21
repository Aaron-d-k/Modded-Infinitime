#include "RenderStuff.h"
#include "displayapp/LittleVgl.h"
#include <climits>


using namespace Pinetime::Applications::Screens;

RenderStuff::RenderStuff(Pinetime::Components::LittleVgl* lvgl, System::SystemTask* systemTask) : lvgl(lvgl), rng(xTaskGetTickCount()), wakeLock(*systemTask) {
    title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(title, "Hello!");
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(title, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

    drawbuffer.fill(LV_COLOR_YELLOW);

    taskRefresh = lv_task_create(RefreshTaskCallback, 10, LV_TASK_PRIO_LOW, this);

    for (auto& i : cgolbuffer)
    {
        for (auto& j : i.data)
        {
            j = rng();
        }
        i.cleanup();
    }

    nextscanline = 0;
    currchunk = 0;
    wakeLock.Lock(); //released automatically in destructor.
}

RenderStuff::~RenderStuff() {
  lv_obj_clean(lv_scr_act());
  lv_task_del(taskRefresh);
}

void FullAdder(const cell_line& b1, const cell_line& b2, const cell_line& b3, cell_line& oh, cell_line& ol)
{
    cell_line tmp = (b1^b2);
    ol = tmp^b3;
    oh = (b1&b2)|(tmp&b3);
}

void HalfAdder(const cell_line& b1, const cell_line& b2, cell_line& oh, cell_line& ol)
{
    ol = b1^b2;
    oh = b1&b2;
}

void RenderStuff::IterateState(std::array<const cell_line*,3> state, cell_line& o)
{

	
    FullAdder((*state[0])<<1,(*state[0])>>1,*state[0], sumhi[0], sumlo[0]);
    HalfAdder((*state[1])<<1,(*state[1])>>1,           sumhi[1], sumlo[1]);
    FullAdder((*state[2])<<1,(*state[2])>>1,*state[2], sumhi[2], sumlo[2]);
    

    {
        cell_line carry;
        cell_line carry2;
        HalfAdder(sumlo[0],  sumlo[1],            carry, accum2[0]);
        FullAdder(sumhi[0],  sumhi[1], carry, accum2[2], accum2[1]);

        HalfAdder(accum2[0], sumlo[2],            carry,  accum[0]);
        FullAdder(accum2[1], sumhi[2], carry,    carry2,  accum[1]);
        HalfAdder(accum2[2], carry2,           accum[3],  accum[2]);
    }
    
    // ty apg for optimal5
    //CODE AUTOMATICALLY GENERATED
    if (rule==Rule::CGOL)
    {
        cell_line x6 = accum[0] | *state[1];
        x6 &= ~accum[3];
        x6 &= accum[1];
        o = x6 &~ accum[2];
    }
    else if (rule==Rule::VOTE)
    {  
        cell_line x6 = *state[1] & accum[2];
        x6 |= accum[1];
        x6 |= accum[0];
        cell_line x9 = x6 ^ accum[2];
        cell_line x10 = accum[2] ^ accum[3];
        o = x10 &~ x9;

    }
    else if (rule==Rule::ANNEAL)
    {
        cell_line x6 = accum[0] ^ *state[1];
        x6 |= accum[3];
        x6 ^= accum[1];
        cell_line x9 = accum[0] | accum[2];
        x9 &= x6;
        cell_line x11 = accum[2] ^ accum[1];
        x11 |= accum[3];
        x11 &= ~x9;
        o = x11 ^ accum[1];
    }
    else o = *state[0];
}


void Pinetime::Applications::Screens::RenderStuff::perform_scan() 
{
    int currscanline = nextscanline;

    nextscanline++;
    nextscanline %= screenS;

    cell_line outpLine;
    IterateState({(currscanline==0)?&ZERO:&prevLine, &cgolbuffer[currscanline], (nextscanline==0)?&ZERO:&cgolbuffer[nextscanline]}, outpLine);
    prevLine = cgolbuffer[currscanline];
    cgolbuffer[currscanline] = outpLine;
}



void Pinetime::Applications::Screens::RenderStuff::Refresh() 
{
    if (lvgl->IsScrolling()) {
        return;
    }

    for (int i=0; i < 32; i++) 
    {
        perform_scan();
    }


    for (size_t y = currchunk*scanlineH; y < currchunk*scanlineH + scanlineH; y++)
    {
        for (size_t i = 0; i < cell_line::BackingArrSize-1; i++)
        {
            uint32_t n = cgolbuffer[y].data[i];

            for (size_t p = 0; p < 32; p++)
            {
                drawbuffer[(y-currchunk*scanlineH)*screenS+i*32+p] = ((n>>p)&1)?LV_COLOR_WHITE:LV_COLOR_BLACK;
            }
        }
        
        {
            uint32_t n = cgolbuffer[y].data[cell_line::BackingArrSize-1];

            for (size_t p = 0; p < screenS-(cell_line::BackingArrSize-1)*32; p++)
            {
                drawbuffer[(y-currchunk*scanlineH)*screenS+(cell_line::BackingArrSize-1)*32+p] = ((n>>p)&1)?LV_COLOR_WHITE:LV_COLOR_BLACK;
            }
        }
    }
    

    lv_area_t area;
    area.x1 = 0;
    area.x2 = screenS-1;
    area.y1 = currchunk*scanlineH;
    area.y2 = currchunk*scanlineH + scanlineH - 1;
    lvgl->FlushDisplay(&area, drawbuffer.data(), false);

    currchunk++;
    currchunk %= screenS/scanlineH;    
}

bool Pinetime::Applications::Screens::RenderStuff::OnTouchEvent(TouchEvents event) {
  if (event==TouchEvents::SwipeLeft)
  {
    rule = Rule::ANNEAL;
    return true;
  }
  if (event==TouchEvents::SwipeRight)
  {
    rule = Rule::VOTE;
    return true;
  }
  if (event==TouchEvents::SwipeUp)
  {
    rule = Rule::CGOL;
    return true;
  }
  else return false;
}
