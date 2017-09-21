#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <memory>
#include <agile.h>
#include <concrt.h>

#define USE_EDGEMODE_JSRT
#include <jsrt.h>
#include "Content\jsrt-wrappers.h"

//standard template library
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <unordered_map>
#include <functional>

//Header files of DirectX Tool Kit 
#include <CommonStates.h>
#include <Audio.h>
#include <Model.h>
#include <effects.h>
#include <PrimitiveBatch.h>
#include <GeometricPrimitive.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <VertexTypes.h>
#include <Camera.h>
#include <Keyboard.h>
#include <DDSTextureLoader.h>
