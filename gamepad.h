#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_



// We need the Windows Header and the XInput Header
#include "stdafx.h"
#include <XInput.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING
// A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

// XBOX Controller Class Definition
class CXBOXController
{
private:
    XINPUT_STATE _controllerState;
    int _controllerNum;
public:
    CXBOXController(int playerNumber);
    XINPUT_STATE GetState();
    bool IsConnected();
    bool checkButtonPress(WORD press);
    void Vibrate(int leftVal = 0, int rightVal = 0);
};

#endif