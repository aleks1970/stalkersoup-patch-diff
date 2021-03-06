#ifdef DX11

#ifndef _CLOUDCONFIG_H
#define _CLOUDCONFIG_H
// note: timers has resolution (sec), where x=1, y=10, z=1/10,
#define CLOUD_TILE0        (0.7f)
#define CLOUD_SPEED0 (0.066)
#define CLOUD_TILE1 (2.8)
#define CLOUD_SPEED1 (0.033)
#define CLOUD_FADE         (0.5)
#endif

#else
	
#ifndef _CLOUDCONFIG_H
#define _CLOUDCONFIG_H
#define CLOUD_TILE0 (0.7f)
#define CLOUD_SPEED0 (0.066)
#define CLOUD_TILE1 (2.8)
#define CLOUD_SPEED1 (0.033)
#define CLOUD_FADE (0.5)
#endif

#endif