//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#ifdef DX11
#ifndef SSH_MOUNT
#define SSH_MOUNT

#include "shadow.h"
#include "ShaderSettingsSunShafts.txt"
#ifdef SSH_STREAKS
#include "halo.h"
#endif

#ifdef USE_LFX
#include "lensflare2.h"
#endif


#define fx .5/1024.f
#define fy .5/768.f
#define SSH_SUNSAMPLES 20
#define SSH_SKYSAMPLES 40
#define SSH_SUNGRID int(1)
#define SSH_DX float(SSH_SUNSAMPLES * fx)
#define SSH_DY float(SSH_SUNSAMPLES * fy)
#define SSH_SUNGRIDSQUARE int(pow(SSH_SUNGRID*2+1,2))
#define sunsamples 100
	
float4 testsun(float2 center)
{
 //float sunsamples = e_weights.z*1000;
 float dx = sunsamples * fx;
 float dy = sunsamples * fy; 
  return tex2Dlod( s_image, float4(center,0,0) ); 
}



float4 screen_sunpos(float3 sundir_w)
{	
	// Transform sun position to screen space
	float4 sunpos = 0; //float4(L_sun_dir_e,0);
	//float sunheightdiff = (abs(sundir_w.y)-SSH_POSHEIGHT);	
	
	
	// OGSE Sun Pos calculation (thanks)	
	// dist to the sun
	float sun_dist = SSH_FARPLANE / (sqrt(1 - sundir_w.y * sundir_w.y));
	// sun pos
	float3 sun_pos_world = sun_dist*sundir_w + eye_position;
	float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1));
	
	// hack for sun vs. artificial light shafts
	if (sundir_w.x == L_sun_dir_w.x && sundir_w.y == L_sun_dir_w.y && sundir_w.z == L_sun_dir_w.z)
		sunpos = convert_to_screen_space(sun_pos_projected)/-abs(sun_pos_projected.w);	
	else
		sunpos = convert_to_screen_space(sun_pos_projected)/abs(sun_pos_projected.w);	
	
	return sunpos;
}

float4 screen_sunpos_diff(float3 sundir_w, float4 sunpos, bool useCorr)
{	
	float xdiff = abs(sunpos.x-0.5);	
	
	float dot_prod_x  = dot(normalize(eye_direction.xz), normalize(sundir_w.xz));
	float dot_angle_x = degrees(acos(dot_prod_x));
	
	float dot_prod_y  = dot(normalize(float2(eye_direction.y,0)), normalize(float2(sundir_w.y,0)));
	float dot_angle_y = degrees(acos(dot_prod_y));	
		
	float xcorr = 1.f;
	float3 testcol = 1;
	
	//if (useCorr)
	if (true)
	{
		if (xdiff > 0.40)
		{
			xcorr = 1.75;
			testcol = float3(0,1,0);
		}
		else if (xdiff > 0.36)
		{
			xcorr = 1.53;
			testcol = float3(0,1,1);
		}	
		else if (xdiff > 0.33)
		{
			xcorr = 1.42;
			testcol = float3(0.5,1,0.5);
		}
		else if (xdiff > 0.30)
		{
			xcorr = 1.33;
			testcol = float3(1,1,0);
		}
		else if (xdiff > 0.24)
		{
			xcorr = 1.22;
			testcol = float3(0,1,1);
		}
		else if (xdiff > 0.18)
		{
			xcorr = 1.115;
			testcol = float3(0,1,0);
		}		
		else if (xdiff > 0.12)
		{
			xcorr = 1.07;
			testcol = float3(1,0,0.5);
		}
	}
		
	if (!useCorr)
	{		
		
		//float b = 180; //e_barrier.z*1000; // 159; //e_barrier.z*1000; // MAX  160 / 170
		//float c = 125; //e_barrier.y*1000; // MAX  120 / 130
		
		float b = 180;
		
		//float c = e_barrier.y*1000 + xoff * e_barrier.z*1000; ; 125;		110 ?
		
		float c = 100 + cos(dot_prod_x) * 15; // e_barrier.z*1000;
		
		if (xdiff >= 0.25)
		{
			float rel_xdiff = 1-saturate(3.333f*(0.4-xdiff)/0.4f); // is between 0 and 1
			c = 100 + cos(dot_prod_x) * (15+rel_xdiff*10);
		}		
					
		float view_min_angle = 70;
			
		if (dot_angle_x < view_min_angle)
		{
			sunpos.a = 0;
		}
		else if (dot_angle_x < c)
		{	
			sunpos.a = 1-saturate((dot_angle_x-c)/(view_min_angle-c));			
		}
	}	
	
	return sunpos;
}

#ifdef SSH
float4 render_shafts(float2 texCoord, float4 sunpos, float3 sundir_w, float factor) : COLOR0  
{  			
	// Compute sun intensity
	float sunColLum = (L_sun_color.r+L_sun_color.g+L_sun_color.b)/3;
	
	// Store initial sample.  
	float4 origColor = tex2Dlod(s_image, float4(texCoord,0,0));  
	float3 color = origColor.rgb;
	
	bool forced = false; //e_barrier.z == 0.1111;
	bool artificial = false; //e_barrier.z == 0.1112;
	bool disabled = false; //e_barrier.z == 0.1113;	
		
	if (!disabled && (forced || artificial || sunColLum > 0) ) // ONLY RENDER SUN SHAFTS WHEN SUN IS THERE
	{		
		float2 uv = texCoord;
		
		float xcorr = sunpos.a;		
		
		
		float fadeX = 1-saturate((abs(sunpos.x-0.5)-SSH_SUNFADEMINX) / (SSH_SUNFADEMAXX-SSH_SUNFADEMINX));			
	
		
		float2 deltaSun      = (texCoord - sunpos.xy);
		float2 deltaTexCoord = deltaSun;
		
		float sundist = abs(distance(texCoord,sunpos.xy));		
				
		float fadeNear =   saturate((sundist-SSH_SUNNEARMIN) / (SSH_SUNNEARMAX-SSH_SUNNEARMIN));
		float fadeDist = 1-saturate((sundist-SSH_SUNDISTMIN) / (SSH_SUNDISTMAX-SSH_SUNDISTMIN));		
		float fadeExpo = 1-saturate((abs(sunpos.x-0.5)-SSH_SUNEXPOMIN) / (SSH_SUNEXPOMAX-SSH_SUNEXPOMIN));					
		
		float SSH_EXPOSURE = lerp(SSH_EXPOSURES.y, SSH_EXPOSURES.x, fadeExpo);
		float SSH_WEIGHT   = lerp(SSH_WEIGHTS.y, SSH_WEIGHTS.x, fadeExpo);
		float SSH_DENSITY  = lerp(SSH_DENSITYS.y, SSH_DENSITYS.x, fadeExpo);
		//float SSH_DECAY    = lerp(SSH_DECAYS.x, SSH_DECAYS.y, fadeDist);			
		
		// alpha correction		
		#ifdef SSH_ALPHA				
		bool is_origColor_particle = 	(
								(  abs(origColor.a-round(origColor.a*100.f)/100.f-0.00001f) < 0.00001f)   ||
								(!(abs(origColor.a-round(origColor.a*100.f)/100.f-0.00001f) < 0.000011f) )
							)
							//&&
							//origColor.a < SSH_MAXALPHARAY
							||
							origColor.a == 1;
		
		float alphaCorr = (origColor.a > SSH_ALPHA && !is_origColor_particle) ? SSH_ALPHACORR : 1;
		#else
		float alphaCorr = 1;
		#endif
		
		// Calculate exposure according to filters
		float exposure = SSH_EXPOSURE * pow(xcorr, 1) * fadeX * fadeNear * fadeDist * alphaCorr;
				
		// Only proceed with drawing sun shafts when exposure higher than zero (save FPS).
		if (exposure > 0.f)
		{			
			// Divide by number of samples and scale by control factor.  	
			deltaTexCoord *= 1.0f / SSH_NUM_SAMPLES * SSH_DENSITY;  	

			// Set up illumination decay factor.  
			float illuminationDecay = 1.0f;  
			
			float test = 0.f;
			float accum = 0.f;
			float last_accum = 0.f;
			float depth = 0.f;
			//bool has_particle = false;
			float occ = 0.f;
			
			float SSH_OCCMIN = 1;
			float SSH_OCCMAX = 20;
			
			//float blur = e_barrier.z * 100;
			//float dx = blur * fx;
			//float dy = blur * fy;
						
			#ifdef SSH_INDOORDETECTION2
			#define SSH_SAMPLE4
			#endif

			// Evaluate summation in SSH_NUM_SAMPLES iterations.  
			#ifdef DX11
			//[loop]
			//[unroll(SSH_NUM_SAMPLES)]
			#endif
			for (int i = 0; i < SSH_NUM_SAMPLES; i++)  
			{  
				// Step sample location along ray.  		
				float2 delta = deltaTexCoord;
				texCoord -= delta; 					
				
				// Retrieve sample at new location.  	
				
				#ifdef SSH_BLUR				// use blur
					const float bdelta = SSH_BLUR * sundist * (.5f/1024.f);	  					
					#ifdef SSH_NATURAL		// use "natural" sampling					
						#ifdef SSH_SAMPLE4			
						float4 sample =   (tex2Dlod(s_image, float4(texCoord + bdelta,0,0))  +
							tex2Dlod(s_image, float4(texCoord - bdelta,0,0))) * (1.f/2.f);
						#else
						float3 sample =   (tex2Dlod(s_image, float4(texCoord + bdelta,0,0)).rgb  +
							tex2Dlod(s_image, float4(texCoord - bdelta,0,0)).rgb) * (1.f/2.f);
						#endif					
					#else					// use "technical" sampling					
						float4 n_sample =   (tex2Dlod(s_normal, float4(texCoord + bdelta,0,0))  +	tex2Dlod(s_normal, float4(texCoord - bdelta,0,0))) * (1.f/2.f);
						#ifdef SSH_SAMPLE4
						float4 image =     tex2Dlod(s_image, float4(texCoord,0,0));
						float4 sample = float4(n_sample.a, n_sample.a, n_sample.a, image.a);					
						#else
						float3 sample = float3(n_sample.a, n_sample.a, n_sample.a);
						#endif					
					#endif					
				#else						// use no blur
					#ifdef SSH_SAMPLE4
					float4 sample = tex2Dlod(s_image, float4(texCoord,0,0));
					#else
					float3 sample = tex2Dlod(s_image, float4(texCoord,0,0)).rgb;
					#endif
				#endif				

				// Adopt sun color					
				sample.rgb = lerp(sample.rgb, sample.rgb * L_sun_color, SSH_SUNCOL);
				
				// Apply sample attenuation scale/decay factors - Accumulate combined color.  
				#ifdef SSH_SAMPLE4
				sample.rgb *= illuminationDecay * SSH_WEIGHT;  		
				color += sample.rgb;  
				#else
				sample *= illuminationDecay * SSH_WEIGHT;  	
				color += sample; 
				#endif

				// Update exponential decay factor.  
				illuminationDecay *= SSH_DECAYS.x; //(1+e_barrier.x); // 				
				
				#ifdef SSH_INDOORDETECTION2
				occ += (sample.a > SSH_MINALPHA2 && sample.a < SSH_MAXALPHA2) ? 1 : 0;
				#else
				occ++;
				#endif			
				
				test += 1;		
				
			}
			
			if (forced || artificial || occ > SSH_OCCMIN)
			{
				float4 final = float4(0,0,0,0);
				
				float rel_occ = saturate((occ-SSH_OCCMIN)/(SSH_OCCMAX-SSH_OCCMIN));							

				//test /= SSH_NUM_SAMPLES;

				// Adjust color/brightness according to sun light					
				float4 sunlight = testsun(sunpos.xy);  
				float lum = (sunlight.r*0.36+sunlight.g*0.63+sunlight.b*0.17)/3;

				if (sunpos.x >= 0 && sunpos.x <= 1 && sunpos.y >= 0 && sunpos.y <= 1)		// sun is in visible view range (i.e. visible on screen if not occluded)
				{				
				}
				
				float lum_factor = (0.5+pow(lum,0.5)/5);
				if (!artificial) lum_factor *= pow(rel_occ,0.5);
				if (forced) lum_factor = 1;
				color *= lum_factor;									
				
				bool is_indoor = (mcls(tex2Dlod(s_position, float4(uv.xy,0,0)).a) == 4) ? true : true;; // false;
			
				// adjust color according to indoor environment				
				bool sun_solid = false;				
				
				// check sun height (world absolute position)	
				float sun_fact = 0;
				float sunheight = abs(sundir_w.y);	
				
				if (sunheight >= SSH_SUNHEIGHTMAX && sunheight <= SSH_SUNHEIGHTHI)
					sun_fact = 1-saturate( (sunheight-SSH_SUNHEIGHTMAX) / (SSH_SUNHEIGHTHI-SSH_SUNHEIGHTMAX) );
				else if (sunheight < SSH_SUNHEIGHTMAX && sunheight >= SSH_SUNHEIGHTLO)
					sun_fact = 1-saturate( (SSH_SUNHEIGHTMAX-sunheight) / (SSH_SUNHEIGHTMAX-SSH_SUNHEIGHTLO) );
					
				// compress/limt exposure
				//float SSH_COMPHI = e_barrier.z*10;
				//float SSH_COMPHISOFT = e_barrier.y; // 0=no compression, 1=limiting		
				#ifdef SSH_COMPHI		
				float lumdiff = max((color.r+color.g+color.b)/3.f - (origColor.r+origColor.g+origColor.b)/3.f,0);
				if (lumdiff > SSH_COMPHI)			
				{
					float comp_cof = lumdiff/SSH_COMPHI;
					exposure /= 1+(comp_cof-1)*SSH_COMPHISOFT;  // min(exposure * lumdiff * e_barrier.z * 1000, exposure);
				}
				#endif
				//float SSH_COMPLO = e_barrier.z*10;
				#ifdef SSH_COMPLO						
				float lumdiff2 = max((color.r+color.g+color.b)/3.f - (origColor.r+origColor.g+origColor.b)/3.f,0);
				if (lumdiff2 < SSH_COMPLO && lumdiff2 > 0)
				{
					float comp_cof = lumdiff2/SSH_COMPLO;
					exposure /= 1+(comp_cof-1)*SSH_COMPLOSOFT;  
				}			
				#endif
				
				if (forced || artificial) sun_fact = 1;
				color *= exposure * sun_fact;				
					
							
				float old_lum = (origColor.r*0.36+origColor.g*0.63+origColor.b*0.17)/3;
				float new_lum = (color.r*0.36+color.g*0.63+color.b*0.17)/3;							
				
				float4 final1 = 0;
				float4 final2 = 0;				

				if (forced || new_lum-old_lum >= SSH_MINLUMDIFF)			/// TODO: FIX FIX FIX
				{											
						final1 = float4(color, 1);
				}
				else					
					final1 = origColor;
					
				final2 = origColor+(float4(color, 1));

				final = lerp(final2,final1,SSH_MINLUMCOF);
					
					
					
					
				/* NEW: ADD HALO (sun only, no artificial light sources) */
				#ifdef SSH_STREAKS
				if (!artificial && origColor.a > SSH_STREAKSMINALPHA)
				{									
					final += lum_factor * render_halo(uv, sunpos.xy);
				}
				#endif		

				//if (abs(uv.y-sunpos.x-0.5) < 0.02)
				//	final += float4(2,0,0,1);					
				
				return final;
				
			}
			else // sun is occluded - no shafts
				return origColor;
		}
		else // exposure is zero - no shafts
			return origColor;
	}
	else // no sun - no shafts
		return origColor;
}  

float4 shaft(float2 texCoord, float4 sunpos, float3 sundir_w, float factor) : COLOR0  
{
	float4 final = render_shafts(texCoord, sunpos, sundir_w, factor);	
		return final;
}

float4 shaftx(float2 center, float4 final, float3 sundir_w, float factor) : COLOR0  
{
	float3 sun_pos_world = sundir_w;
	float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1));	
	float4 sunpos = convert_to_screen_space(sun_pos_projected)/abs(sun_pos_projected.w);		
			
	// DEBUG SUN POS
	//if (abs(sunpos.x-center.x)<0.05 && abs(sunpos.y-center.y)<0.05)
	//	final += float4(1,0.5,0,1);
	
	/*
	float dot_prod_x  = dot(normalize(eye_direction.xz), normalize(sun_pos_projected.xz));
	float dot_angle_x = (degrees(acos(dot_prod_x)));
	if (dot_angle_x > e_weights.z * 1000)
	{
		sunpos.a = 0;
	}
	*/
	float4 shafts = float4(0,0,0,0);
	shafts = shaft(center, sunpos, sundir_w, factor);
	final = (final + shafts) / 2.f;	
	
	return final;
}
#endif

#ifdef USE_LFX
float4 lensfx(float2 center, float4 final, float3 sundir_w, float factor)  
{
	float3 sun_pos_world = sundir_w;
	float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1));	
	float4 sunpos = convert_to_screen_space(sun_pos_projected)/abs(sun_pos_projected.w);	

	float dist_amount =  1.5 - (distance(float2(0.5,0.5), sunpos.xy) + 0.0001);					// amount based on 2D-distance of light source to screen center
	
	float z_diff = abs(tex2Dlod(s_position, float4(sunpos.xy,0,0)).z - sun_pos_projected.z);	// occlusion check: depth pitch between light pos and texel pos
		
	float z_dist = 1-pow(saturate(sun_pos_projected.z/LFX_MAXDIST),LFX_DISTPOW);								// amount based on 3D-distance of light source
	
	// check if within or near screen boundaries
	if (z_diff < 3.0f && sunpos.x > -1 && sunpos.x < 2 && sunpos.y > -1 && sunpos.y < 2)
	{
		final.rgb += render_lensflare(center.xy, sunpos.xy, dist_amount) * factor * dist_amount * z_dist;	
	}
	
	return final;
}
#endif

#endif			// SSH_MOUNT
#endif