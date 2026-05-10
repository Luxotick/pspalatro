#include "global.h"

SceCtrlData g_pad, g_last_pad;

void input_init()
{
    sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

void input_update()
{
    memcpy(&g_last_pad, &g_pad, sizeof(SceCtrlData));
    sceCtrlReadBufferPositive(&g_pad, 1);
}

bool input_is_button_down(unsigned int button)
{
    return g_pad.Buttons & button;
}

bool input_was_button_pressed(unsigned int button)
{
    return !(g_last_pad.Buttons & button) && (g_pad.Buttons & button);
}

bool input_was_button_released(unsigned int button)
{
    return (g_last_pad.Buttons & button) && !(g_pad.Buttons & button);
}
