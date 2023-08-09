#pragma once

#ifdef VORTEX_EXPORTS
	#define VORTEX_API __declspec(dllexport)
#else
	#define VORTEX_API __declspec(dllimport)
#endif

#pragma warning(disable:4251)