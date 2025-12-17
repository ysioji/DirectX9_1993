/*
	stdafx.h
*/
#pragma once

#include <stdio.h>
#include <windows.h>		// Win32 APIのためのヘッダ.
#include <tchar.h>
#include <time.h>
#include <assert.h>

#include <d3d9.h>			// Direct3D9 (DirectX9 Graphics)に必要.
#include <d3dx9core.h>		// DirectX スプライトに必要.
#include <dsound.h>			// DirectSound に必要.

#define DIRECTINPUT_VERSION 0x0800// これがいないとビルド時に警告がでる.
#include <dinput.h>			// DirectInput に必要.
#include <dinputd.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "myTexture.h"
#include "myInput.h"
#include "myFont.h"

#include "field.h"
#include "tankBase.h"
#include "bulletBase.h"
#include "pilotBase.h"
