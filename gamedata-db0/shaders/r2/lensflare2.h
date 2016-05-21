//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders, line 18 textur
#ifndef LENS2
#define LENS2

#define LFX_SCREENRATIO float(3/4.f) //  float(10/16.f)			// screen ratio for lens FX 
#define LFX_FLARES 10 // 8										// number of flares to be rendered
#define LFX_MAXDIST float(20.0f)								// Maximum distance to light source for flares to show up
#define LFX_DISTPOW int(2)										// Impact of distance to light source on flares intensity 

#define LFX_HEXA												// Use hexagon instead of pentagon as polygon shape
#define LFX_POLY 8												// Maximum number of polygon edges
//#define LFX_HEXA												// If enabled, use hexagon instead of pentagon as polygon shape
#define LFX_ROTATION											// Use rotation. Experimental. Eats some FPS. Makes some other shader features unusable.
#define LFX_POLYROT 0 //  // 11									// Rotation offset angle, if shape rotation used
#define LFX_AMSIZE float(0.004)  // 0.0025 // 0.006				// Density of anamorphic lens flares.


#define  HS0 float4( 1.0,         0.0,         0.0,  1.0)
#define  HS1 float4( 0.309016994, 0.951056516, 0.0,  1.0)
#define  HS2 float4(-0.809016994, 0.587785252, 0.0,  1.0)
#define  HS3 float4(-0.809016994,-0.587785252, 0.0,  1.0)
#define  HS4 float4( 0.309016994,-0.951056516, 0.0,  1.0)
#define  HS5 float4( 0.0        ,0.0         , 1.0,  1.0)

#define  AM0 float4( 1.0,         0.0,         0.0,  1.0)
#define  AM1 float4( 0.2,         0.0025,         0.0,  1.0)
#define  AM2 float4( 0.0,         0.0045,        0.0,  1.0)
#define  AM3 float4(-0.2,         0.0025,         0.0,  1.0)
#define  AM4 float4(-1.0,         0.0,         0.0,  1.0)
#define  AM5 float4(-0.2,        -0.0025,         0.0,  1.0)
#define  AM6 float4( 0.0,        -0.0045,        0.0,  1.0)
#define  AM7 float4( 0.2,        -0.0025,         0.0,  1.0)
float2 wh_(float2 f,float2 x,float T){float2 L=-x;float A=radians(T),i=sin(A),y=cos(A);float2x2 r=float2x2(y,-i,i,y);f=f+L;f=mul(f,r);f=f-L;return f;}int w_h(int r,float L[LFX_POLY],float f[LFX_POLY],float x,float T){int i,Y,y=0;for(i=0,Y=r-1;i<r;Y=i++){if(f[i]>T!=f[Y]>T&&x<(L[Y]-L[i])*(T-f[i])/(f[Y]-f[i])+L[i])y=!y;}return y;}void whs(out float f[LFX_POLY]){float r[LFX_POLY];for(int i=0;i<LFX_POLY;i++)r[i]=0;f=r;}float sh2(float2 f,float2 L,float O,int i,float r){float y=0;if(r!=0){y=degrees(acos(dot(normalize(float2(0,-.5)),normalize(L-.5))))/2.f;if(L.x>.5)y*=-1;y+=r;}if(i==1){float2 s=wh_(HS0,float2(0,0),y+LFX_POLYROT),Y=wh_(HS1,float2(0,0),y+LFX_POLYROT),d=wh_(HS2,float2(0,0),y+LFX_POLYROT),a=wh_(HS3,float2(0,0),y+LFX_POLYROT),e=wh_(HS4,float2(0,0),y+LFX_POLYROT);float T[LFX_POLY];whs(T);T[0]=s.x*O+L.x;T[1]=Y.x*O+L.x;T[2]=d.x*O+L.x;T[3]=a.x*O+L.x;T[4]=e.x*O+L.x;float A[LFX_POLY];whs(A);A[0]=s.y*O/LFX_SCREENRATIO+L.y;A[1]=Y.y*O/LFX_SCREENRATIO+L.y;A[2]=d.y*O/LFX_SCREENRATIO+L.y;A[3]=a.y*O/LFX_SCREENRATIO+L.y;A[4]=e.y*O/LFX_SCREENRATIO+L.y;return w_h(5,T,A,f.x,f.y);}else if(i==2){float2 s=wh_(AM0,float2(0,0),y+LFX_POLYROT),Y=wh_(AM1,float2(0,0),y+LFX_POLYROT),d=wh_(AM2,float2(0,0),y+LFX_POLYROT),a=wh_(AM3,float2(0,0),y+LFX_POLYROT),e=wh_(AM4,float2(0,0),y+LFX_POLYROT),A=wh_(AM5,float2(0,0),y+LFX_POLYROT),n=wh_(AM6,float2(0,0),y+LFX_POLYROT),T=wh_(AM7,float2(0,0),y+LFX_POLYROT);float x[LFX_POLY];whs(x);float H[LFX_POLY];whs(H);float p[LFX_POLY];whs(p);x[0]=s.x*O+L.x;x[1]=Y.x*O+L.x;x[2]=d.x*O+L.x;x[3]=a.x*O+L.x;x[4]=e.x*O+L.x;x[5]=A.x*O+L.x;x[6]=n.x*O+L.x;x[7]=T.x*O+L.x;H[0]=x[0];H[1]=x[1];H[2]=x[2];H[3]=x[3];H[4]=x[4];H[5]=x[5];H[6]=x[6];H[7]=x[7];p[0]=x[0];p[1]=x[1];p[2]=x[2];p[3]=x[3];p[4]=x[4];p[5]=x[5];p[6]=x[6];p[7]=x[7];float w[LFX_POLY];whs(w);float t[LFX_POLY];whs(t);float o[LFX_POLY];whs(o);w[0]=s.y*O/LFX_SCREENRATIO+L.y;w[1]=Y.y*O/LFX_SCREENRATIO+L.y;w[2]=d.y*O/LFX_SCREENRATIO+L.y;w[3]=a.y*O/LFX_SCREENRATIO+L.y;w[4]=e.y*O/LFX_SCREENRATIO+L.y;w[5]=A.y*O/LFX_SCREENRATIO+L.y;w[6]=n.y*O/LFX_SCREENRATIO+L.y;w[7]=T.y*O/LFX_SCREENRATIO+L.y;t[0]=w[0]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[1]=w[1]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[2]=w[2]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[3]=w[3]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[4]=w[4]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[5]=w[5]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[6]=w[6]+LFX_AMSIZE*O/LFX_SCREENRATIO;t[7]=w[7]+LFX_AMSIZE*O/LFX_SCREENRATIO;o[0]=w[0]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[1]=w[1]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[2]=w[2]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[3]=w[3]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[4]=w[4]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[5]=w[5]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[6]=w[6]-LFX_AMSIZE*O/LFX_SCREENRATIO;o[7]=w[7]-LFX_AMSIZE*O/LFX_SCREENRATIO;return w_h(8,x,w,f.x,f.y)+.3*w_h(8,H,t,f.x,f.y)+.3*w_h(8,p,o,f.x,f.y);}else return 1;}float dist(float2 L,float2 T){return sqrt(pow(L.x-T.x,2)+pow((L.y-T.y)*LFX_SCREENRATIO,2));}float3 fhs1(float2 f,float2 y,float L,float3 O,float x,float E,float T,int i,float r,float2 p){float3 t=0;float A=dist(f,y+p);if(A<L){if(L==0)L=.0001;float a=saturate(A/L+.01),Y=smoothstep(0,E*4,a)*smoothstep(0,saturate(T)+.0001,1-a);if(i>0)Y*=sh2(f,y+p,L,i,r);t+=float3(Y,Y,Y)*O;}return t*x;}float2 fh2(float2 L,float O){return float2(-(L.x-.5)*O+.5,-(L.y-.5)*O+.5);}struct flare{float dist;float radius;float amount;float3 filter;float body;float edge;int shape;float radiusPow;float amountPow;float bodyPow;float edgePow;float rotation;float2 offset;};float4 render_lensflare(float2 x,float2 f,float T){
#include"ShaderSettingsLensFlare.txt"
float2 r=x;float4 L=0;float i=.7,y=.1;float3 a=float3(1,1.1,1.5);for(int O=0;O<LFX_FLARES;O++)L.xyz+=fhs1(x,fh2(f,flares[O].dist),flares[O].radius*pow(T,flares[O].radiusPow),flares[O].filter,flares[O].amount*pow(T,flares[O].amountPow),flares[O].body/pow(T,flares[O].bodyPow),flares[O].edge/pow(T,flares[O].edgePow),flares[O].shape,flares[O].rotation,flares[O].offset);return L;}
#endif