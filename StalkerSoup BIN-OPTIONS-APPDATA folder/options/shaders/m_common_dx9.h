//TB3D 109995 Adapted from Meltac 2.0 DX11 shaders, #elif fixed
//dx9

#include "shared\common.h"
#ifndef SSM_Mount
#include "ShaderSettings.txt"
#endif

#ifndef COMMON_H
	#define COMMON_H

	#pragma warning(disable: 3206)

	// #define USE_SUPER_SPECULAR			//109992: moved to shadersettings.txt
		
		#ifndef SMAP_size
			#ifdef CUSTOM_SHADOW_RESOLUTION
				#define SMAP_size CUSTOM_SHADOW_RESOLUTION
			#else
				#define SMAP_size 1024
			#endif
		#endif
		#define PARALLAX_H 0.02
		#define parallax float2(PARALLAX_H,-PARALLAX_H/2)
		#ifdef USE_R2_STATIC_SUN
		#define xmaterial half(1.0h/4.h)
		#else
		#define xmaterial half(L_material.w)
		#endif
		uniform half4 L_material;
		uniform half4 Ldynamic_color;
		uniform half4 Ldynamic_pos;
		uniform half4 Ldynamic_dir;
		uniform half4 J_direct [6];
		uniform half4 J_spot [6];
		#ifndef USE_F32_FOG
		half calc_fogging (half4 w_pos) { return dot(w_pos,fog_plane);}
		#else
		half calc_fogging (half4 w_pos) { return dot(w_pos,(fog_plane*1.125f));}
		#endif
		half2 calc_detail (half3 w_pos) {
		float dtl=distance (w_pos,eye_position)*dt_params.w;
		dtl=min (dtl*dtl,1);
		half dt_mul=1-dtl;
		half dt_add=.5*dtl;
		return half2 (dt_mul,dt_add);
		}
		half3 calc_reflection (half3 pos_w,half3 norm_w) {
		return reflect(normalize(pos_w-eye_position),norm_w);
		}
		half3 calc_sun_r1 (half3 norm_w) { return L_sun_color*saturate(dot((norm_w),-L_sun_dir_w));}
		half3 calc_model_hemi_r1 (half3 norm_w) { return max(0,norm_w.y)*L_hemi_color;}
		half3 calc_model_lq_lighting (half3 norm_w) { return L_material.x*calc_model_hemi_r1(norm_w)+L_ambient+L_material.y*calc_sun_r1(norm_w);}
		struct v_static {
		half4 P:POSITION;
		half4 Nh:NORMAL;
		half4 T:TANGENT;
		half4 B:BINORMAL;
		float2 tc:TEXCOORD0;
		float2 lmh:TEXCOORD1;
		half4 color:COLOR0;
		};
		struct v_tree {
		half4 P:POSITION;
		half4 Nh:NORMAL;
		half3 T:TANGENT;
		half3 B:BINORMAL;
		half4 tc:TEXCOORD0;
		};
		struct v_model {
		half4 P:POSITION;
		half3 N:NORMAL;
		half3 T:TANGENT;
		half3 B:BINORMAL;
		float2 tc:TEXCOORD0;
		};
		struct v_detail {
		half4 pos:POSITION;
		int4 misc:TEXCOORD0;
		};
		#ifdef USE_HWSMAP
		struct v_shadow_direct_aref {
		half4 hpos:POSITION;
		float2 tc0:TEXCOORD1;
		};
		struct v_shadow_direct {
		half4 hpos:POSITION;
		};
		#else
		struct v_shadow_direct_aref {
		half4 hpos:POSITION;
		float depth:TEXCOORD0;
		float2 tc0:TEXCOORD1;
		};
		struct v_shadow_direct {
		half4 hpos:POSITION;
		float depth:TEXCOORD0;
		};
		#endif
		struct p_bumped {
		half4 hpos:POSITION;
		#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
		half4 tcdh:TEXCOORD0;
		#else
		float2 tcdh:TEXCOORD0;
		#endif
		half4 position:TEXCOORD1;
		half3 M1:TEXCOORD2;
		half3 M2:TEXCOORD3;
		half3 M3:TEXCOORD4;
		#if defined(USE_PARALLAX) || defined(USE_Sky4CE_PARALLAXOCCLUSION)
		half3 eye:TEXCOORD5;
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
		half4 hpos:POSITION;
		#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
		half4 tcdh:TEXCOORD0;
		#else
		float2 tcdh:TEXCOORD0;
		#endif
		half4 position:TEXCOORD1;
		half3 N:TEXCOORD2;
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
		float get_val2(){return 0;}
		const half ePq=float(1);
		const float get_val() { float rPq;rPq=ePq;half fu=get_val2();return rPq;}
		struct f_deffer {
		half4 position:COLOR0;
		half4 Ne:COLOR1;
		half4 C:COLOR2;
		};
		struct p_screen {
		half4 hpos:POSITION;
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
		#define def_gloss half(2.f/255.f)
		#define def_aref half(200.f/255.f)
		#define def_dbumph half(0.333f)
		#define def_virtualh half(.05f)
		#define def_distort half(0.05f)
		#ifndef USE_F32
		#define def_hdr half(8.h)
		#else
		#define def_hdr half(HDR_int)
		#endif
		#define def_hdr_clip half(0.75h)
		#define LUMINANCE_VECTOR half3(0.3f,0.48f,0.22f)
		void tonemap (out half4 low,out half4 high,half3 rgb,half scale)
		{
		rgb=rgb*scale;
		#ifdef USE_BRANCHING
		low=rgb.xyzz;
		high=low/def_hdr;
		#else
		low=half4(rgb,0);
		high=half4(rgb/def_hdr,0);
		#endif
		}
		half4 combine_bloom (half3 low,half4 high) {
		return half4(low+high*high.a,1.f);
		}
		half3 v_hemi(half3 n) { return L_hemi_color*(.5f+.5f*n.y);}
		half3 v_hemi_wrap(half3 n,float w) { return L_hemi_color*(w+(1-w)*n.y);}
		half3 v_sun(half3 n) { return L_sun_color*dot(n,-L_sun_dir_w);}
		half3 v_sun_wrap(half3 n,float w) { return L_sun_color*(w+(1-w)*dot(n,-L_sun_dir_w));}
		half3 p_hemi(float2 tc) {
		half3 t_lmh=tex2D (s_hemi,tc);
		return dot(t_lmh,1.h/3.h);
		}
		float2 Parallax(half3 eye,float2 tc,sampler2D s_base) {
		half height=tex2D(s_base,tc).w*0.016-0.009;
		return tc+height*normalize(eye);
		}
		float2 Sky4CE_ParallaxOcclusion(half3 eye,float2 tc,sampler2D s_base) {
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
		double2 vTexCurrentOffset=tc;
		float fCurrHeight=0;
		float fCurrentBound=1.0;
		float Q1=CONTRAST_COEF_Q1;
		float Q2=CONTRAST_COEF_Q2;
		float Brightness=BRIGHTNESS_COEF;
		for(;fCurrHeight<fCurrentBound;fCurrentBound-=fStepSize)
		{
		vTexCurrentOffset.xy+=vTexOffsetPerStep;
		#ifndef USE_TEXTURE_PACK
		fCurrHeight=tex2Dlod(s_base,half4(vTexCurrentOffset.xy,0,0)).a+Brightness;
		if (fCurrHeight>Q2) fCurrHeight=1.0f;
		else if (fCurrHeight<Q1) fCurrHeight=0.0f;
		else fCurrHeight=(fCurrHeight-Q1)/(Q2-Q1);
		#else
		fCurrHeight=tex2Dlod(s_base,half4(vTexCurrentOffset.xy,0,0)).a;
		#endif
		}
		half4 offsetBest=half4(vTexCurrentOffset,0,0);
		vTexCurrentOffset.xy-=vTexOffsetPerStep;
		#ifndef USE_TEXTURE_PACK
		float fPrevHeight=tex2Dlod(s_base,half4(vTexCurrentOffset.xy,0,0)).a+Brightness;
		if (fPrevHeight>Q2) fPrevHeight=1.0f;
		else if (fPrevHeight<Q1) fPrevHeight=0.0f;
		else fPrevHeight=(fPrevHeight-Q1)/(Q2-Q1);
		#else
		float fPrevHeight=tex2Dlod(s_base,half4(vTexCurrentOffset.xy,0,0)).a;
		#endif
		float error=1.0;
		float t1=fCurrentBound;
		float t0=t1+fStepSize;
		float delta1=t1-fCurrHeight;
		float delta0=t0-fPrevHeight;
		half4 intersect=half4(vDelta,vDelta+tc);
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
		static const half2 poisson_disc12[12]=
		{
		half2(-0.326212f,-0.405810f),
		half2(-0.840144f,-0.073580f),
		half2(-0.695914f,0.457137f),
		half2(-0.203345f,0.620716f),
		half2(0.962340f,-0.194983f),
		half2(0.473434f,-0.480026f),
		half2(0.519456f,0.767022f),
		half2(0.185461f,-0.893124f),
		half2(0.507431f,0.064425f),
		half2(0.896420f,0.412458f),
		half2(-0.321940f,-0.932615f),
		half2(-0.791559f,-0.597710f)
		};
		half calc_ssao(half3 P,half3 N,half2 tc) {
		half2 scale=half2(.5f/1024.h,.5f/768.h)*150/max(P.z,1.3);
		half occ=0;
		half num_dir=0;
		float c=1;
		if(P.z<FADE_DIST) c=FADE_COEF+((1-FADE_COEF)*P.z)/FADE_DIST;
		for (int a=1;a<SSAO_QUALITY;++a)
		{
		half2 scale_tmp=scale*a;
		for (int i=0;i<12;i++)
		{
		float2 tap=tc+poisson_disc12[i]*scale_tmp;
		half3 tap_pos=tex2D(s_position,tap);
		half3 dir=tap_pos-P.xyz;
		half dist=length(dir);
		dir=normalize(dir);
		half infl=clamp(dot(dir,N.xyz),0,c);
		half occ_factor=saturate(dist);
		occ+=(infl+0.01)*lerp(1,occ_factor,infl)/(occ_factor+0.1);
		num_dir+=(infl+0.01)/(occ_factor+0.1);
		}
		}
		occ/=num_dir;
		return occ;
		} 
		#define FXPS technique _render{pass _code{PixelShader=compile ps_3_0 main();}}
		#define FXVS technique _render{pass _code{VertexShader=compile vs_3_0 main();}}


#endif