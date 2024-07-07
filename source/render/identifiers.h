#pragma once

namespace Lotus
{

/*
  UBOs binding points
*/
constexpr unsigned int CameraBufferBindingPoint     = 0;
constexpr unsigned int LightsBufferBindingPoint     = 1;
constexpr unsigned int ProceduralBufferBindingPoint = 2;

/*
  SSBOs binding points
*/
constexpr unsigned int ObjectBufferBindingPoint       = 0;
constexpr unsigned int ObjectHandleBufferBindingPoint = 1;
constexpr unsigned int MaterialBufferBindingPoint     = 2;

}