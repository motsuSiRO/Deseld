#pragma once
#include "Effekseer.h"
#include "EffekseerRendererDX11.h"
#include "EffekseerSoundXAudio2.h"

#include <map>

namespace Mo2Efk
{
	Effekseer::Manager* manager;
	EffekseerRendererDX11::Renderer* renderer;
	std::map<int, Effekseer::Effect*> efk_list;

	//static std::list<int> 
}