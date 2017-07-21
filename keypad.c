//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// - Example 11 (LINUX):
//     Demonstrate graph control with scrollbar
//
#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <unistd.h>   // For usleep() //xxx

#include "codes.h"
#include "leds.h"

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_1, E_ELEM_BTN_2, E_ELEM_BTN_3,
      E_ELEM_BTN_4, E_ELEM_BTN_5, E_ELEM_BTN_6,
      E_ELEM_BTN_7, E_ELEM_BTN_8, E_ELEM_BTN_9,
      E_ELEM_BTN_0, E_ELEM_BTN_ENTER, E_ELEM_BTN_RST,
      E_ELEM_TXT_PIN};
enum {E_FONT_BTN, E_FONT_TXT};

bool      m_bQuit = false;

#define MAX_PIN 6
int m_pin[MAX_PIN];
size_t m_pin_idx = 0;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            2
#define MAX_ELEM_PG_MAIN    13

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

void setPinText();

// Configure environment variables suitable for display
// - These may need modification to match your system
//   environment and display type
// - Defaults for GSLC_DEV_FB and GSLC_DEV_TOUCH are in GUIslice_config.h
// - Note that the environment variable settings can
//   also be set directly within the shell via export
//   (or init script).
//   - eg. export TSLIB_FBDEVICE=/dev/fb1
void UserInitEnv()
{
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)
  setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);
  setenv((char*)"SDL_FBDEV",GSLC_DEV_FB,1);
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
#endif  

#if defined(DRV_TOUCH_TSLIB)
  //setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  //setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1); 
  //setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  //setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  //setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);  
#endif
   
}

// Define debug message function
static int16_t DebugOut(char ch)
{
    fputc(ch,stderr);
    return 0;
}

// Enter button callback
bool CbBtnEnter(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN)
  {
    size_t idx;
    if ((idx = codecmp(m_pin, m_pin_idx)) == MAX_ENT + 1)
    {
        setRed();
    }
    else
    {
        gslc_tsElem* pElemTmp = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_PIN);      
        gslc_ElemSetTxtStr(pElemTmp, pinEntries[idx].name);
        m_bQuit = true;
        setGreen();
    }
  }
  return true;
}

void setPinText()
{
    char            acTxt[MAX_PIN+1];
    char* pTmp = acTxt;
    gslc_tsElem*    pElemTmp = NULL;

    for (size_t i = 0; i < m_pin_idx; ++i)
    {
        pTmp += snprintf(pTmp,2,"%d",m_pin[i]);
    }
    pTmp[0] = 0;
    pElemTmp = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_PIN);      
    gslc_ElemSetTxtStr(pElemTmp,acTxt);
}

bool CbBtnReset(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
    if (eTouch == GSLC_TOUCH_UP_IN)
    {
        m_pin_idx = 0;
        setPinText();
        clearLeds();
    }
    return true;
}

bool CbBtnNumeric(void* pvGui, void *pvElem, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
    if (eTouch == GSLC_TOUCH_UP_IN)
    {
        if (m_pin_idx >= MAX_PIN)
        {
            return true;
        }
        int num = 0;
        switch (((gslc_tsElem*)pvElem)->nId)
        {
        case E_ELEM_BTN_9:
            ++num;
        case E_ELEM_BTN_8:
            ++num;
        case E_ELEM_BTN_7:
            ++num;
        case E_ELEM_BTN_6:
            ++num;
        case E_ELEM_BTN_5:
            ++num;
        case E_ELEM_BTN_4:
            ++num;
        case E_ELEM_BTN_3:
            ++num;
        case E_ELEM_BTN_2:
            ++num;
        case E_ELEM_BTN_1:
            ++num;
        case E_ELEM_BTN_0:
            {
                m_pin[m_pin_idx++] = num;
                setPinText();
            }
        }
    }
    return true;
}

// Create page elements
bool InitOverlays()
{
  gslc_tsElem*  pElem = NULL;

  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN, m_asPageElemRef, MAX_ELEM_PG_MAIN);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Number buttons
  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_1, E_PG_MAIN,
      (gslc_tsRect){15,20,90,90}, "1", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_2, E_PG_MAIN,
      (gslc_tsRect){115,20,90,90}, "2", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_3, E_PG_MAIN,
      (gslc_tsRect){215,20,90,90}, "3", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_4, E_PG_MAIN,
      (gslc_tsRect){15,120,90,90}, "4", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_5, E_PG_MAIN,
      (gslc_tsRect){115,120,90,90}, "5", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_6, E_PG_MAIN,
      (gslc_tsRect){215,120,90,90}, "6", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_7, E_PG_MAIN,
      (gslc_tsRect){15,220,90,90}, "7", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_8, E_PG_MAIN,
      (gslc_tsRect){115,220,90,90}, "8", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_9, E_PG_MAIN,
      (gslc_tsRect){215,220,90,90}, "9", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_0, E_PG_MAIN,
      (gslc_tsRect){115,320,90,90}, "0", 0, E_FONT_BTN, &CbBtnNumeric);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);

  // Reset button
  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_RST, E_PG_MAIN,
       (gslc_tsRect){15,320,90,90}, "RST", 0, E_FONT_BTN, &CbBtnReset);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);  

  // Quit button
  pElem = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_ENTER, E_PG_MAIN,
       (gslc_tsRect){215,320,90,90}, "ENTER", 0, E_FONT_BTN, &CbBtnEnter);
  gslc_ElemSetCol(pElem, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem, GSLC_COL_WHITE);  

  // Pin text
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_PIN,E_PG_MAIN,
      (gslc_tsRect){10,430,300,30}, "",0,E_FONT_TXT); 

  return true;
}


int main(int argc, char* args[])
{
  bool                bOk = true;
  
  // -----------------------------------
  // Initialize
#ifdef DEBUG
  gslc_InitDebug(&DebugOut);
#endif
  loadDb();
  initLeds();
  UserInitEnv();
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT))
  {
      exit(1);
  }  
  
  // Load Fonts
  bOk = gslc_FontAdd(&m_gui, E_FONT_BTN, FONT_DROID_SANS, 14);
  if (!bOk)
  {
      fprintf(stderr,"ERROR: FontAdd failed\n");
      exit(1);
  }
  bOk = gslc_FontAdd(&m_gui, E_FONT_TXT, FONT_DROID_SANS, 20);
  if (!bOk)
  {
      fprintf(stderr,"ERROR: FontAdd failed\n");
      exit(1);
  }

  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui, E_PG_MAIN);
  
  // -----------------------------------
  // Main event loop

  while (!m_bQuit)
  {
    gslc_Update(&m_gui);
    usleep(30000);
  }
  usleep(1000000);

  // -----------------------------------
  // Close down display
  gslc_Quit(&m_gui);
  cleanupLeds();

  return 0;
}
