//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders, line 18 textur
#ifdef DX11
#ifndef NIGHTVISION
#define NIGHTVISION
#include "aa.h"
#include "blur.h"
#include "infrared.h"

float2 noise = float2(.5f/1024.f,.5f/768.f);

float  zoom(float input, float factor, float offset) { return (input-0.5f+offset) / factor+0.5f-offset; }
float2 zoom2(float2 input, float factor, float2 offsetXY) { return float2(zoom(input.x, factor, offsetXY.x), zoom(input.y, factor, offsetXY.y)); }
float4 zoom4(float4 input, float factor, float2 offsetXY) { float4 result; result.xy = zoom2(input.xy, factor, offsetXY); result.z=input.z; result.w=input.w; return result; }
//float2 nv_zoom4(float4 input) { return zoom4(input, NV_ZOOM); }
v2p zoom_v2p(v2p input, float factor, float2 offsetXY)
{
	v2p result;
	result.tc0 = zoom4(input.tc0, factor, offsetXY);
	result.tc1 = zoom4(input.tc1, factor, offsetXY);
	result.tc2 = zoom4(input.tc2, factor, offsetXY);
	result.tc3 = zoom4(input.tc3, factor, offsetXY);
	result.tc4 = zoom4(input.tc4, factor, offsetXY);
	result.tc5 = zoom4(input.tc5, factor, offsetXY);
	result.tc6 = zoom4(input.tc6, factor, offsetXY);
	return result;
}
/*
float3 zoom_rgb(v2p input, float factor)
{
	float3 c=tex2D(s_image,zoom2(input.tc0, factor)).rgb;
	for (int i=1; i < factor; i++)
	{
		c+=tex2D(s_image,zoom2(input.tc0, i+1)).rgb;
	}
	c/=factor;
	return c;
}
*/

float4 nv( v2p In, float4 color, float3 visionColor, bool entireScreen) : COLOR
{
  // Settings  
  //float luminanceThreshold = 1.0; // 0.2      
  float brightness = NV_BRIGHTNESS; //0.10f; //0.07; // 1.0; // 0.2;
  
  float2 measurePoints[9];
  measurePoints[0] = float2(0.50001f, 0.50001f);
  measurePoints[1] = float2(0.40001f, 0.50001f);
  measurePoints[2] = float2(0.60001f, 0.50001f);
  measurePoints[3] = float2(0.50001f, 0.40001f);
  measurePoints[4] = float2(0.50001f, 0.60001f);
  measurePoints[5] = float2(0.40001f, 0.40001f);
  measurePoints[6] = float2(0.40001f, 0.60001f);
  measurePoints[7] = float2(0.60001f, 0.40001f);
  measurePoints[8] = float2(0.60001f, 0.60001f);
	
  float4 finalColor;
  
  bool returnNow = false;
  
  float px=In.tc0.x-0.5f;
  float py=(In.tc0.y-0.5f) * NV_SCREENRATIO;	// correction for 4:3 / 16:10 ratio
  float center_dist = sqrt(pow(px, 2) + pow(py, 2));	// absolute distance from screen center	
  
  if (entireScreen || center_dist < NV_RADIUS)
  {	
	#ifdef INFRARED
	if (e_barrier.x == 0.69 || e_barrier.x == 0.67)
	{
		finalColor = infrared(In.tc0.xy, (e_barrier.x == 0.67));			// 0.67 = black/white IR, 0.69 = full-color IR
	}
	else
	{
	#endif
		// Use magnification
		#ifdef NV_ZOOM
		//float4 newpos = In.tc0;
		//newpos.x = (In.tc0.x-0.5f) / NV_ZOOM + 0.5f;
		//newpos.y = (In.tc0.y-0.5f) / NV_ZOOM + 0.5f;	
		
		//float aa_offset = (e_kernel.x - 0.3) * 10 / 4.h * 10;
		//float aa_offset = 0.1;	
		//float4 aa_pos = zoom4(In.tc0, NV_ZOOM, aa_offset);
		float bias = saturate(center_dist/NV_RADIUS);
		float zoom = NV_ZOOM+bias*NV_BIAS; // effective zoom level for current pixel
		float4 newpos = In.tc0;
		if (!entireScreen) newpos = In.tc0 = zoom4(In.tc0, zoom, 0);
		
		
		/*
		int2 screen_pos = newpos.xy * float2(1024.f, 768.f);
		float modulo = zoom;
		bool even = (screen_pos.x % modulo == 0) && (screen_pos.y % modulo == 0);	
		bool right = (screen_pos.x % modulo != 0) && (screen_pos.y % modulo == 0);
		bool bottom = (screen_pos.x % modulo == 0) && (screen_pos.y % modulo != 0);
		bool diagon = (screen_pos.x % modulo != 0) && (screen_pos.y % modulo != 0);
		if (px > 0)
		{
			if (right)
			{
				newpos = (In.tc0+zoom4(In.tc3, NV_ZOOM, 0)+zoom4(In.tc2, NV_ZOOM, 0))/3;
			}
			else if (bottom)
			{
				newpos = (In.tc0+zoom4(In.tc4, NV_ZOOM, 0)+zoom4(In.tc2, NV_ZOOM, 0))/3;
			}
			else if (diagon)
			{
				newpos = zoom4(In.tc2, NV_ZOOM, 0);
			}
		}
		*/
		//if (!even) newpos = aa_pos;
		//if (right) newpos = (In.tc0+zoom4(In.tc3, NV_ZOOM, 0)+zoom4(In.tc2, NV_ZOOM, 0))/3;
		//else if (bottom) newpos = (In.tc0+zoom4(In.tc4, NV_ZOOM, 0)+zoom4(In.tc2, NV_ZOOM, 0))/3;
		//else if (diagon) newpos = zoom4(In.tc2, NV_ZOOM, 0);
		#else
		float4 newpos = In.tc0;
		#endif
		
		
		float m = tex2D(s_image, newpos.xy).r;	
		float redFactor = (1-NV_RED + m * NV_RED);	
				
		//float3 c = color.rgb; //tex2D(s_image, In.tc0 + noise).rgb;
		float3 c=tex2D(s_image,newpos.xy).rgb;
		//float3 c=zoom_rgb(In, NV_ZOOM);
		//float3 c=(tex2D(s_image,newpos).rgb+tex2D(s_image,newpos+noise).rgb)/2;
		//float3 c=aa(zoom_v2p(In, NV_ZOOM)).rgb;
		//float myblur = (e_kernel.x - 0.3) * 10 / 4.h;
		//float3 c=simpleblur(zoom_v2p(In, NV_ZOOM), myblur * 100).rgb;
			
		// measure and compute center luminance
		float totalLum = 0;
		float pitch_diff = 4.4;  
		for (int i=0; i < 9; i++)
		{
			float3 pointRgb = tex2Dlod(s_image, float4(measurePoints[i],0,0)).rgb;		// color at measure point
			float pointLum = (pointRgb.x + pointRgb.y + pointRgb.z) / 3.f;	// luminance at measure point
			totalLum += pointLum;
			
			// Show measure points on screen		
			float pitch_x = abs(In.tc0.x - measurePoints[i].x);
			float pitch_y = abs(In.tc0.y - measurePoints[i].y);
			if (pitch_x < pitch_diff && pitch_y < pitch_y)
			{
				finalColor.rgb = float3(1.0, 0.0, 1.0);
				returnNow = true;
			}
		}	
		
		if (!returnNow)
		{	
			#ifdef NV_SATURATION
			// Apply (de-)saturation - only if NV color other than (1,1,1)
			if (visionColor.x+visionColor.y+visionColor.z != 3)
			{
				float gray=dot(c,float3(0.3,0.59,0.11));
				c=lerp(float3(gray,gray,gray),c,NV_SATURATION+(dot(Ldynamic_color.xyz,float3(1,1,1))/3)*NV_SAT_SUN_COEF);
			}
			#endif
					
		
			float avgLum = totalLum/9.f;										// average luminance				
			brightness /= avgLum;		
			if (brightness < NV_MINBRIGHT) brightness = NV_MINBRIGHT;		
			else if (brightness > NV_MAXBRIGHT) brightness = NV_MAXBRIGHT;
			//if (brightness > 2) brightness = 2;

			float lum = dot(float3(0.30, 0.59, 0.11), c);		
			c *= NV_AMP * (1.f-lum); // + (2-lum); 
			
			// Apply vision color and adjustments (only if not "white" which means no color)
			//if (visionColor.x+visionColor.y+visionColor.z > 0)
			if (visionColor.x+visionColor.y+visionColor.z != 3)
			{
				// set nightvision color
				finalColor.rgb = c * visionColor * redFactor;		
			
				// set contrast
				finalColor.rgb = (1 - NV_CONTRAST) + finalColor.rgb * NV_CONTRAST;
			
				// set brightness (limited!)
				float endLum = (finalColor.rgb.x + finalColor.rgb.y + finalColor.rgb.z) / 3.f;
				if (endLum * brightness > NV_MINBRIGHT)
					brightness = NV_MINBRIGHT / endLum;
				finalColor.rgb *=  brightness;
			}
			else		
				finalColor.rgb = tex2D(s_image,newpos.xy).rgb;
		}
	#ifdef INFRARED
	   }
	#endif
   }
   else
   {
		finalColor = color;
   }  
  
  finalColor.a = 1.0;
  return finalColor;
}
#endif
#endif