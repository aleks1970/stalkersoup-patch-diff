//TB3D 109995 Adapted from Meltac 2.0 DX11 shaders, #elif fixed
//dx11

#include "shared\common.h"
//#ifndef SSM_Mount
#include "ShaderSettings.txt"
//#endif

#ifndef COMMON_H
	#define COMMON_H

	#pragma warning(disable: 3206)

	// #define USE_SUPER_SPECULAR			//109992: moved to shadersettings.txt
		//////////////////////////////////////////////////////////////////////////////////////////
		// *** options

		// #define USE_GAMMA_22
		// #define USE_SJITTER
		// #define USE_SUNFILTER
		//
		// #define USE_MBLUR                	//- HW-options defined
		// #define USE_HWSMAP                	//- HW-options defined

		// #define USE_HWSMAP_PCF				//- nVidia GF3+, R600+

		// #define USE_BRANCHING        		//- HW-options defined
		// #define USE_VTF                		//- HW-options defined, VertexTextureFetch
		// #define FP16_FILTER                	//- HW-options defined
		// #define FP16_BLEND                	//- HW-options defined
		//
		// #define USE_PARALLAX                	//- shader defined
		// #define USE_TDETAIL                	//- shader defined
		// #define USE_LM_HEMI                	//- shader defined
		// #define USE_DISTORT                	//- shader defined
		#ifndef USE_F32
			#define USE_SUNMASK                		//- shader defined
		#endif
		//#define DBG_TMAPPING
		//////////////////////////////////////////////////////////////////////////////////////////
		#ifndef SMAP_size
			#ifdef CUSTOM_SHADOW_RESOLUTION
				#define SMAP_size CUSTOM_SHADOW_RESOLUTION
			#else
				#define SMAP_size 2048
			#endif
		#endif
		#define PARALLAX_H 0.025 // 0.02
		#define parallax float2(PARALLAX_H,-PARALLAX_H/2)
		#ifdef USE_R2_STATIC_SUN
			#define xmaterial float(1.0h/4.h)
		#else
			#define xmaterial float(L_material.w)
		#endif
		uniform float4 L_material;
		uniform float4 Ldynamic_color;
		uniform float4 Ldynamic_pos;
		uniform float4 Ldynamic_dir;
		uniform float4 J_direct [6];
		uniform float4 J_spot [6];
		#ifndef USE_F32_FOG
			float calc_fogging (float4 w_pos) { return dot(w_pos,fog_plane);}
		#else
			float calc_fogging (float4 w_pos) { return dot(w_pos,(fog_plane*1.125f));}
		#endif
		float2 calc_detail (float3 w_pos) {
		float dtl=distance (w_pos,eye_position)*dt_params.w;
		dtl=min (dtl*dtl,1);
		float dt_mul=1-dtl;
		float dt_add=.5*dtl;
		return float2 (dt_mul,dt_add);
		}
		float3 calc_reflection (float3 pos_w,float3 norm_w) {
		return reflect(normalize(pos_w-eye_position),norm_w);
		}
		float3 calc_sun_r1 (float3 norm_w) { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));}
		float3 calc_model_hemi_r1 (float3 norm_w) { return max(0,norm_w.y)*L_hemi_color.xyz;}
		float3 calc_model_lq_lighting (float3 norm_w) { return L_material.x*calc_model_hemi_r1(norm_w)+L_ambient.xyz+L_material.y*calc_sun_r1(norm_w);}
		struct v_static {
		float4 P:POSITION;
		float4 Nh:NORMAL;
		float4 T:TANGENT;
		float4 B:BINORMAL;
		float2 tc:TEXCOORD0;
		float2 lmh:TEXCOORD1;
		float4 color:COLOR0;
		};
		struct v_tree {
		float4 P:POSITION;
		float4 Nh:NORMAL;
		float3 T:TANGENT;
		float3 B:BINORMAL;
		float4 tc:TEXCOORD0;
		};
		struct v_model {
		float4 P:POSITION;
		float3 N:NORMAL;
		float3 T:TANGENT;
		float3 B:BINORMAL;
		float2 tc:TEXCOORD0;
		};
		struct v_detail {
		float4 pos:POSITION;
		int4 misc:TEXCOORD0;
		};
		#ifdef USE_HWSMAP
		struct v_shadow_direct_aref {
		float4 hpos:POSITION;
		float2 tc0:TEXCOORD1;
		};
		struct v_shadow_direct {
		float4 hpos:POSITION;
		};
		#else
		struct v_shadow_direct_aref {
		float4 hpos:POSITION;
		float depth:TEXCOORD0;
		float2 tc0:TEXCOORD1;
		};
		struct v_shadow_direct {
		float4 hpos:POSITION;
		float depth:TEXCOORD0;
		};
		#endif
		struct p_bumped {
		float4 hpos:POSITION;
		#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
			float4 tcdh:TEXCOORD0;
		#else
			float2 tcdh:TEXCOORD0;
		#endif
		float4 position:TEXCOORD1;
		float3 M1:TEXCOORD2;
		float3 M2:TEXCOORD3;
		float3 M3:TEXCOORD4;
		#if defined(USE_PARALLAX) || defined(USE_Sky4CE_PARALLAXOCCLUSION)
			float3 eye:TEXCOORD5;
			#ifdef USE_TDETAIL
				float2 tcdbump:TEXCOORD6;
				#ifdef USE_LM_HEMI
					float2 lmh:TEXCOORD7;
				#endif
			#else
				#ifdef USE_LM_HEMI
					float2 lmh:TEXCOORD6;
				#endif
			#endif
		#else
			#ifdef USE_TDETAIL
				float2 tcdbump:TEXCOORD5;
				#ifdef USE_LM_HEMI
					float2 lmh:TEXCOORD6;
				#endif
			#else
				#ifdef USE_LM_HEMI
					float2 lmh:TEXCOORD5;
				#endif
			#endif
		#endif
		};
		struct p_flat {
		float4 hpos:POSITION;
		#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
			float4 tcdh:TEXCOORD0;
		#else
			float2 tcdh:TEXCOORD0;
		#endif
		float4 position:TEXCOORD1;
		float3 N:TEXCOORD2;
		#ifdef USE_TDETAIL
			float2 tcdbump:TEXCOORD3;
			#ifdef USE_LM_HEMI
				float2 lmh:TEXCOORD4;
			#endif
		#else
			#ifdef USE_LM_HEMI
				float2 lmh:TEXCOORD3;
			#endif
		#endif
		};
		float get_val2(){
		return 0;}
		static const float4 ePq=float(1);
		static const float get_val() { float rPq;rPq=ePq.x;float fu=get_val2();return rPq;}
		struct f_deffer {
		float4 position:COLOR0;
		float4 Ne:COLOR1;
		float4 C:COLOR2;
		};
		struct p_screen {
		float4 hpos:POSITION;
		float2 tc0:TEXCOORD0;
		};
		uniform sampler2D s_base;
		uniform sampler2D s_bump;
		uniform sampler2D s_bumpX;
		uniform sampler2D s_detail;
		uniform sampler2D s_bumpD;
		uniform sampler2D s_hemi;
		uniform sampler2D s_mask;
		uniform sampler2D s_dt_r;
		uniform sampler2D s_dt_g;
		uniform sampler2D s_dt_b;
		uniform sampler2D s_dt_a;
		uniform sampler2D s_dn_r;
		uniform sampler2D s_dn_g;
		uniform sampler2D s_dn_b;
		uniform sampler2D s_dn_a;
		uniform sampler2D s_depth;
		uniform sampler2D s_position;
		uniform sampler2D s_normal;
		uniform sampler s_lmap;
		uniform sampler3D s_material;
		uniform sampler1D s_attenuate;
		uniform sampler2D s_diffuse;
		uniform sampler2D s_accumulator;
		uniform sampler2D s_generic;
		uniform sampler2D s_bloom;
		uniform sampler s_image;
		uniform sampler2D s_tonemap;
		#define def_gloss float(2.f/255.f)
		#define def_aref float(200.f/255.f)
		#define def_dbumph float(0.333f)
		#define def_virtualh float(.05f)
		#define def_distort float(0.05f)
		#ifndef USE_F32
			#define def_hdr float(8.h)
			#define def_hdr_mul float(.125h)
		#else
			#define def_hdr float(HDR_int)
		#endif
		#define def_hdr_clip float(0.75h)
		#define LUMINANCE_VECTOR float3(0.3f,0.48f,0.22f)
		void tonemap (out float4 low,out float4 high,float3 rgb,float scale)
		{
			#ifdef USE_BRANCHING
				#ifndef USE_F32
					low = (rgb * scale).xyzz;
					high = low*def_hdr_mul;
				#else				
					low	 = (rgb * scale).xyzz;
					high = low/def_hdr;
				#endif		
			#else
				rgb *= scale;
				low  = float4(rgb,0);
				high = float4(rgb/def_hdr,0);
			#endif
		}
		float4 combine_bloom (float3 low,float4 high) {
		return float4(low+high.xyz*high.a,1.f);
		}
		float3 v_hemi(float3 n) { return L_hemi_color.xyz*(.5f+.5f*n.y);}
		float3 v_hemi_wrap(float3 n,float w) { return L_hemi_color.xyz*(w+(1-w)*n.y);}
		float3 v_sun(float3 n) { return L_sun_color.xyz*dot(n,-L_sun_dir_w);}
		float3 v_sun_wrap(float3 n,float w) { return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));}

		float3 p_hemi(float2 tc) { return dot(tex2D(s_hemi,tc),1.h/3.h); } //float3 t_lmh=tex2D(s_hemi,tc);

		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//
		//--- © The below code is copyright of Sky4CE ---//
		//----------- mailto: Sky4CE@inbox.ru -----------//
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//

		float2 Parallax(float3 eye,float2 tc,sampler2D s_base) {
		float height=tex2D(s_base,tc.xy).w*0.016-0.009;
		return (tc.xy+height*normalize(eye).xyz).xy;
		}
		float2 Sky4CE_ParallaxOcclusion(float3 eye,float2 tc,sampler2D s_base) {
		const int maxSamples=MAX_SAMPLES;
		const int minSamples=MIN_SAMPLES;
		float fParallaxOffset=PARALLAX_OFFSET;
		int nNumSteps=lerp(maxSamples,minSamples,normalize(eye).z);
		float2 vDelta=-normalize(eye).xy*fParallaxOffset;
		#ifdef CORRECT_PERSPECTIVE
			vDelta/=normalize(eye).z;
		#endif
		float fStepSize=1.0/nNumSteps;
		float2 vTexOffsetPerStep=fStepSize*vDelta;
		float2 vTexCurrentOffset=tc;
		float fCurrHeight=0;
		float fCurrentBound=1.0;
		float Q1=CONTRAST_COEF_Q1;
		float Q2=CONTRAST_COEF_Q2;
		float Brightness=BRIGHTNESS_COEF;
		for(;fCurrHeight<fCurrentBound;fCurrentBound-=fStepSize)
		{
		vTexCurrentOffset.xy+=vTexOffsetPerStep;
		#ifndef USE_TEXTURE_PACK
			fCurrHeight=tex2Dlod(s_base,float4(vTexCurrentOffset.xy,0,0)).a+Brightness;
			if (fCurrHeight>Q2) fCurrHeight=1.0f;
			else if (fCurrHeight<Q1) fCurrHeight=0.0f;
			else fCurrHeight=(fCurrHeight-Q1)/(Q2-Q1);
		#else
			fCurrHeight=tex2Dlod(s_base,float4(vTexCurrentOffset.xy,0,0)).a;
		#endif
		}
		float4 offsetBest=float4(vTexCurrentOffset,0,0);
		vTexCurrentOffset.xy-=vTexOffsetPerStep;
		#ifndef USE_TEXTURE_PACK
			float fPrevHeight=tex2Dlod(s_base,float4(vTexCurrentOffset.xy,0,0)).a+Brightness;
			if (fPrevHeight>Q2) fPrevHeight=1.0f;
			else if (fPrevHeight<Q1) fPrevHeight=0.0f;
			else fPrevHeight=(fPrevHeight-Q1)/(Q2-Q1);
		#else
			float fPrevHeight=tex2Dlod(s_base,float4(vTexCurrentOffset.xy,0,0)).a;
		#endif
		float error=1.0;
		float t1=fCurrentBound;
		float t0=t1+fStepSize;
		float delta1=t1-fCurrHeight;
		float delta0=t0-fPrevHeight;
		float4 intersect=float4(vDelta,vDelta+tc);
		for (int i=0;i<FINAL_INTERSECTION_LOOPS && abs(error)>0.01;i++)
		{ 
		float denom=(delta1-delta0);
		float t=(t0*delta1-t1*delta0)/denom;
		offsetBest.xy=-t*intersect.xy+intersect.zw;
		#ifndef USE_TEXTURE_PACK
			float NB=tex2Dlod(s_base,offsetBest).a+Brightness;
			if (NB>Q2) NB=1.0f;
			else if (NB<Q1) NB=0.0f;
			else NB=(NB-Q1)/(Q2-Q1);
		#else
			float NB=tex2Dlod(s_base,offsetBest).a;
		#endif
		error=t-NB;
		if (error<0)
		{
		delta1=error;
		t1=t;
		}
		else
		{
		delta0=error;
		t0=t;
		}
		}
		return offsetBest.xy;
		}
		static const float2 poisson_disc12[12]=
		{
		float2(-0.326212f,-0.405810f),
		float2(-0.840144f,-0.073580f),
		float2(-0.695914f,0.457137f),
		float2(-0.203345f,0.620716f),
		float2(0.962340f,-0.194983f),
		float2(0.473434f,-0.480026f),
		float2(0.519456f,0.767022f),
		float2(0.185461f,-0.893124f),
		float2(0.507431f,0.064425f),
		float2(0.896420f,0.412458f),
		float2(-0.321940f,-0.932615f),
		float2(-0.791559f,-0.597710f)
		};


		#ifdef SSAO_LEGACY
			#ifdef SSAO_QUALITY_3
				static const float hbao_qlty = 3;
			#else
				#ifdef SSAO_QUALITY_2
					static const float hbao_qlty = 2;
				#else
					#ifdef SSAO_QUALITY_1
						static const float hbao_qlty = 1;
					#else
						static const float hbao_qlty = 0;
					#endif
				#endif
			#endif

			// Use Sky4ce's old SSAO algorithm
			float calc_ssao(float3 P,float3 N,float2 tc)
			{
				if (hbao_qlty == 0) return 1.0f;
				float2 scale=float2(.5f/1024.h,.5f/768.h)*150/max(P.z,1.3);
				float occ=0;
				float num_dir=0;
				float c=1;
				if(P.z<FADE_DIST) c=FADE_COEF+((1-FADE_COEF)*P.z)/FADE_DIST;
				for (int a=1;a<hbao_qlty;++a)
				{
					float2 scale_tmp=scale*a;
					for (int i=0;i<12;i++)
					{
						float2 tap=tc+poisson_disc12[i]*scale_tmp;
						float3 tap_pos=tex2D(s_position,tap).xyz;
						float3 dir=tap_pos-P.xyz;
						//float3 di2=rcp(dir);
						float dist=length(dir);
						dir=normalize(dir);
						float infl=clamp(dot(dir,N.xyz),0,c);
						float occ_factor=saturate(dist);
						//float sss = ddy_fine(occ_factor);
						occ+=(infl+0.01)*lerp(1,occ_factor,infl)/(occ_factor+0.1);
						num_dir+=(infl+0.01)/(occ_factor+0.1);
					}
				}
				occ/=num_dir;
				return occ;
			} 
		#else
			#ifdef SSAO_NEW		// Use new SSAO implementation
				static const float2 poisson_disc12_2[6] =
				{
					float2(-0.326212f , -0.405810f),
					float2(-0.695914f ,  0.457137f),
					float2( 0.962340f , -0.194983f),
					float2( 0.519456f ,  0.767022f),
					float2( 0.507431f ,  0.064425f),
					float2(-0.321940f , -0.932615f),
				};
				//    Screen space ambient occlusion
				//    P    screen space position of the original point
				//    N    screen space normal of the original point
				//    tc    G-buffer coordinates of the original point
				float    calc_ssao( float3 P, float3 N, float2 tc)
				{
					float point_depth = P.z;
					if (point_depth<0.01) point_depth = 100000.0h;    //    filter for the sky
					float2     scale     = float2    (.5f / 1024.h, .5f / 768.h)*150/max(point_depth,1.3);
					// sample
					float     occ    = 0.1h;   
					float num_dir    = 0.1h;
					for (int a=1; a<3; ++a)
					{
						float2    scale_tmp = scale*a;
						for (int i=0; i<6; i++)
						{
							float3     dir     = tex2D    (s_position,tc + poisson_disc12_2[i]*scale_tmp)-P.xyz;
							float     occ_factor = saturate(length(dir));
							float     infl     = saturate( dot( normalize( dir ), N.xyz ) );
							{
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
							}
						}
					}
					occ /= num_dir;
					return occ;
				}
			#else			// Use daemonjax' SSAO algorithm
				float calc_ssao (float3 P, float3 N, float2 tc)
				{
					#ifndef SSAO
						return 1.h;
					#else
						float2 scale = float2(.5f / SAO_DENSITY, .67f / SAO_DENSITY)*(150/max(P.z,1.3));
						float 	occ	 = 0.0h;	
						float num_dir = 0.0h;
						#ifndef SSAO_NOLOOP
							for (int a = 1; a < SSAO_PASSES; ++a)
						#else
							int a = 1;
						#endif
						//{
							float2	scale_tmp = scale*a;		
							float3 	dir 	= tex2D	(s_position,tc + float2(-0.416212f, -0.665810f)*scale_tmp).xyz-P.xyz;
							float 	occ_factor = saturate(length(dir));
							float 	infl 	= saturate(dot(normalize(dir), N.xyz));
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);		
							dir 	= tex2D	(s_position,tc +float2(0.432340f,-0.093580f)*scale_tmp).xyz-P.xyz;
							occ_factor = saturate(length(dir));
							infl 	= saturate(dot(normalize(dir), N.xyz));			
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);
							dir 	= tex2D	(s_position,tc +float2(-0.455914f,0.647137f)*scale_tmp).xyz-P.xyz;
							occ_factor = saturate(length(dir));
							infl 	= saturate(dot(normalize(dir), N.xyz));			
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);
							dir 	= tex2D	(s_position,tc +float2(0.479456f,0.627022f)*scale_tmp).xyz-P.xyz;
							occ_factor = saturate(length(dir));
							infl 	= saturate(dot(normalize(dir), N.xyz));			
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);
							dir 	= tex2D	(s_position,tc +float2(-0.492340f,0.090983f)*scale_tmp).xyz-P.xyz;
							occ_factor = saturate(length(dir));
							infl 	= saturate(dot(normalize(dir), N.xyz));			
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);
							dir 	= tex2D	(s_position,tc +float2(0.413434f,-0.680026f)*scale_tmp).xyz-P.xyz;
							occ_factor = saturate(length(dir));
							infl 	= saturate(dot(normalize(dir), N.xyz));			
							occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
							num_dir += (infl+0.01)/(occ_factor+0.1);
							#ifdef SSAO_HIGH_QUALITY
								dir 	= tex2D	(s_position,tc +float2(0.726212f, 0.305810f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
								dir 	= tex2D	(s_position,tc +float2(-0.742340f,0.173580f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
								dir 	= tex2D	(s_position,tc +float2(0.395914f,-0.757137f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
								dir 	= tex2D	(s_position,tc +float2(-0.719456f,-0.367022f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
								dir 	= tex2D	(s_position,tc +float2(0.772340f,-0.094983f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
								dir 	= tex2D	(s_position,tc +float2(-0.373434f,0.780026f)*scale_tmp).xyz-P.xyz;
								occ_factor = saturate(length(dir));
								infl 	= saturate(dot(normalize(dir), N.xyz));			
								occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
								num_dir += (infl+0.01)/(occ_factor+0.1);
							#endif
						//}
						return (occ / num_dir);
					#endif
				}
			#endif
			// end SSAO
		#endif

		int mcls(float mcol)
		{
			mcol = (mcol + 0.25) / 4.f;
			if      (mcol < 0.14)	// RGB 28 - Concrete walls, stone/brick buildings, NPCs, mutants, suits
				return 1;
			else if (mcol < 0.16)	// RGB 37 - Terrain, grass, water, metal cases, wood crates, signs, plastics, medkits, food, TVs, bulbs/lamps, some doors
				return 2;
			else if (mcol < 0.22)	// RGB 52 - Wood, bushes, trees, glass bottles
				return 3;
			else if (mcol < 0.29)	// RGB 70 - Weapons, binoc, ammo, ammo boxes
				return 4;
			else if (mcol < 0.33)	// RGB 82 - Vehicles, steel boxes, gas cans, fuel cans, barrels
				return 5;
			else if (mcol < 0.36)	// RGB 87 - Metallic structures/objects
				return 6;
			else
				return -1;
		}


		/* FROM OGSE - Thanks to OGSE team! */
		//	contrast function
		half Contrast(half Input, half ContrastPower)
		{
			 //piecewise contrast function
			 bool IsAboveHalf = Input > 0.5 ;
			 half ToRaise = saturate(2*(IsAboveHalf ? 1-Input : Input));
			 half Output = 0.5*pow(ToRaise, ContrastPower);
			 Output = IsAboveHalf ? 1-Output : Output;
			 return Output;
		}
		float4 convert_to_screen_space(float4 proj)
		{
			float4 screen;
			screen.x = (proj.x + proj.w)*0.5;
			screen.y = (proj.w - proj.y)*0.5;
			screen.z = proj.z;
			screen.w = proj.w;
			return screen;
		}
		/* END OGSE */
		float4 convert_to_world_space(float4 proj)
		{
			float4 world;
			world.x = (proj.x/0.5f)-proj.w; 
			world.y = proj.w-(proj.x/0.5f);
			world.z = proj.z;
			world.w = proj.w;
			return world;
		}

		// Checks whether b lies between a and c, regardless of a > c or a < c
		bool between(float b, float a, float c)
		{
			return b > (a < c ? a : c) && b < (a > c ? a : c);
		}
		bool between(float3 b, float3 a, float3 c)
		{
			return between(b.x, a.x, c.x) && between(b.y, a.y, c.y) && between(b.z, a.z, c.z);
		}

		// Wave warp
		float dist(float a, float b, float c, float d){
			return sqrt((a - c) * (a - c) + (b - d) * (b - d));
		}
		float2 wavewarp(float2 uv, float wsize, float wtime, float wval1, float wval2, float wval3, float wval4)
		{
			float f = sin(dist(uv.x + wtime, uv.y, wval1, wval1)*wsize)
						  + sin(dist(uv.x, uv.y, wval2, wval2)*wsize)
						  + sin(dist(uv.x, uv.y + wtime / wval4, wval3, wval2)*wsize);
			uv.xy = uv.xy+((f/wsize));
			return uv;
		}

		#define FXPS technique _render{pass _code{PixelShader=compile ps_5_0 main();}}
		#define FXVS technique _render{pass _code{VertexShader=compile vs_5_0 main();}}

#endif