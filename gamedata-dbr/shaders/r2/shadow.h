//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"

#ifndef SHADOW_H
#define SHADOW_H

#ifdef DX11
	//#define KERNEL .6f
	uniform sampler s_smap:register(ps,s0);
	uniform sampler2D jitter0;
	uniform sampler2D jitter1;
	float sample_sw(float2 tc,float2 shift,float depth_cmp) 
	{
	static const float ts=KERNEL/float(SMAP_size);
	tc+=shift*ts;
	float texsize=SMAP_size;
	float offset=0.5f/texsize;
	float2 Tex00=tc+float2(-offset,-offset);
	float2 Tex01=tc+float2(-offset,offset);
	float2 Tex10=tc+float2(offset,-offset);
	float2 Tex11=tc+float2(offset,offset);
	float4 depth=float4(
	depth_cmp-tex2D(s_smap,Tex00).x,
	depth_cmp-tex2D(s_smap,Tex01).x,
	depth_cmp-tex2D(s_smap,Tex10).x,
	depth_cmp-tex2D(s_smap,Tex11).x);
	float4 compare=step(depth,0);
	float2 fr=frac(Tex00*texsize);
	float2 ifr=float2(1,1)-fr;
	float4 fr4=float4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
	return dot (compare,fr4);
	}
	float shadow_sw(float4 tc) 
	{ 
		float2 tc_dw=tc.xy/tc.w;
		float4 s;
		s.x=sample_sw(tc_dw,float2(-1,-1),tc.z);
		s.y=sample_sw(tc_dw,float2(+1,-1),tc.z);
		s.z=sample_sw(tc_dw,float2(-1,+1),tc.z);
		s.w=sample_sw(tc_dw,float2(+1,+1),tc.z);
		return dot(s,1.h/4.h);
	}
	#ifndef USE_F32
		float sample_hw_pcf (float4 tc,float4 shift)
		{
			static const float ts=KERNEL/float(SMAP_size);
			return tex2Dproj(s_smap, tc + tc.w * shift * ts).x;			
		}
		float ss_sample_hw_pcf (float4 tc)
		{
			//static const float ts=KERNEL/float(SMAP_size);
			//return tex2Dproj(s_smap, tc + tc.w * ts).x;			
			return tex2Dproj(s_smap, tc).x;		
		}
		float4 test(float4 tc,float2 offset) 
		{			
			return tex2Dproj(s_smap, float4(tc.xy + tc.w * offset, tc.zw)); //float4 tcx=float4(tc.xy+tc.w*offset,tc.zw);		
		}
		float test2(float4 tc, float2 shift) 
		{			
			return tex2Dproj(s_smap, float4(tc.xy + tc.w * shift, tc.zw)).x;		
		}
		float shadow_hw(float4 tc)
		{	
			#ifdef USE_SHADOW_16
				return (sample_hw_pcf(tc, float4( 1,  1, 0, 0)) + sample_hw_pcf(tc, float4( -1, -1, 0, 0)) +
						sample_hw_pcf(tc, float4( 1, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1,  1, 0, 0)) + 							
						sample_hw_pcf(tc, float4( 0,  1, 0, 0)) + sample_hw_pcf(tc, float4(  1,  0, 0, 0)) + 
						sample_hw_pcf(tc, float4( 0, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1,  0, 0, 0)) + 						
						sample_hw_pcf(tc, float4( 0,  2, 0, 0)) + sample_hw_pcf(tc, float4(  2,  0, 0, 0)) + 	
						sample_hw_pcf(tc, float4( 0, -2, 0, 0)) + sample_hw_pcf(tc, float4( -2,  0, 0, 0)) +								
						sample_hw_pcf(tc, float4( 2,  2, 0, 0)) + sample_hw_pcf(tc, float4( -2, -2, 0, 0)) +
						sample_hw_pcf(tc, float4( 2, -2, 0, 0)) + sample_hw_pcf(tc, float4( -2,  2, 0, 0))) * (1.h/16.h); // /16.h
			#else
				return (sample_hw_pcf(tc, float4( 0,  1, 0, 0)) + sample_hw_pcf(tc, float4(  1, 0, 0, 0)) + 
						sample_hw_pcf(tc, float4( 0, -1, 0, 0)) + sample_hw_pcf(tc, float4( -1, 0, 0, 0))) * (1.h/4.h); // /4.h
			#endif			
		} 
		float sample_hw_f4(float4 tc,float4 shift)
		{
			static const float ts=KERNEL/float(SMAP_size);
			float4 T4=tc/tc.w;
			T4.xy+=shift.xy*ts;
			float4 D4=tex2D(s_smap,T4.xy);
			float4 compare=T4.z<D4;
			float texsize=SMAP_size;
			float2 fr=frac(T4.xy*texsize);
			float2 ifr=float2(1,1)-fr;
			float4 fr4=float4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
			float4 fr4s=fr4.zywx;
			return dot(compare,fr4s);
		}
		float shadow_hw_f4(float4 tc) 
		{
			float s0=sample_hw_f4(tc,float4(-1,-1,0,0));
			float s1=sample_hw_f4(tc,float4(+1,-1,0,0));
			float s2=sample_hw_f4(tc,float4(-1,+1,0,0));
			float s3=sample_hw_f4(tc,float4(+1,+1,0,0));
			return (s0+s1+s2+s3)/4.h;
		}
		#ifdef USE_HWSMAP_PCF
			float shadow(float4 tc) { return shadow_hw (tc);}
		#else
			#ifdef USE_FETCH4
				float shadow(float4 tc) { return shadow_hw_f4(tc);}
			#else
				float shadow(float4 tc) { return shadow_sw (tc);}
			#endif
		#endif
		float shadowtest(float4 tc,float4 tcJ) 
		{
			#ifdef SHADOWTEST_LEGACY
				const float scale=(KERNEL/float(SMAP_size));
				float4 J0=tex2Dproj(jitter0,tcJ) * scale;
				float4 J1=tex2Dproj(jitter1,tcJ) * scale;
				return (test(tc,J0.xy).x + test(tc,J0.wz).y + test(tc,J1.xy).z + test(tc,J1.wz).x) * (1.h/4.h);
			#else
				const float4 jitter = tex2Dproj(jitter0, tcJ) * tc.w *(KERNEL/float(SMAP_size));			
				return (tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x  + 
					    tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x  +					
					    tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x  +
					    tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x) * (1/4.h);
			#endif
		}
		#ifdef HIGH_QUALITY_JITTER			
			float shadowtest_sun(float4 tc, float4 tcJ) 
			{						
				#ifdef HIGH_QUALITY_JITTER_LEGACY						
					const float offset = -1.h - (2 / float(JITTER_SAMPLES));
					float shadow = 0.h;					
					for (int i = 0; i < (JITTER_SAMPLES / 2); ++i) 
					{										
						float4 jitter = tex2D(jitter0, tcJ) * (KERNEL/float(SMAP_size));					
						shadow += test2(tc, jitter.xy);
						shadow += test2(tc, jitter.zw);
						tcJ *= offset;								
					}				
					return shadow * (1 / float(JITTER_SAMPLES));	
				#else																
					const float offset = 1.h + (8 / float(JITTER_SAMPLES));
					float shadow = 0;					
					for (int i = JITTER_SAMPLES / 8; i > 0; --i) 
					{										
						float4 jitter = tex2D(jitter0, tcJ.xy) * tc.w * (KERNEL/float(SMAP_size)) ;
						shadow += tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x +
								  tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x +				
								  tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x +
								  tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x +
								  tex2Dproj(s_smap, float4(tc.xy + jitter.yx, tc.zw)).x +
								  tex2Dproj(s_smap, float4(tc.xy + jitter.wz, tc.zw)).x +					
								  tex2Dproj(s_smap, float4(tc.xy - jitter.yx, tc.zw)).x +
								  tex2Dproj(s_smap, float4(tc.xy - jitter.wz, tc.zw)).x;
						tcJ.xy *= offset;
					}				
					return shadow * (1 / float(JITTER_SAMPLES));		
				#endif
			}
		#else
			float shadowtest_sun(float4 tc, float4 tcJ) 
			{				
				#ifdef SHADOWTEST_SUN_LEGACY
					const float scale=(KERNEL/float(SMAP_size));
					float4 J0=tex2D(jitter0,tcJ) * scale;
					float4 J1=tex2D(jitter1,tcJ) * scale;
					float4 r;
					r.x=test(tc,J0.xy).x;
					r.y=test(tc,J0.wz).y;
					r.z=test(tc,J1.xy).z;
					r.w=test(tc,J1.wz).x;
					return dot(r,1.h/4.h);
				#else
					const float4 jitter = tex2D(jitter0, tcJ) * tc.w *(KERNEL/float(SMAP_size));			
					return (tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x  + 
							tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x  +					
							tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x  +
							tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x) * (1/4.h);
				#endif
			}
		#endif
		#ifdef USE_SUNMASK 
			uniform float3x4 m_sunmask;
			float sunmask(float4 P) 
			{
				const float2 tc=mul(m_sunmask,P).xy;
				return tex2D(s_lmap,tc).w;
			}
		#else
			float sunmask (float4 P) { return 1.h;}
		#endif
	#else
		#ifdef USE_HWSMAP_PCF
			float sample_hw_pcf (float4 tc,float4 shift)
			{
				static const float ts=KERNEL/float(SMAP_size);
				return tex2Dproj(s_smap,tc+tc.w*shift*ts).x;
			}
			float ss_sample_hw_pcf (float4 tc)
			{
				static const float ts=KERNEL/float(SMAP_size);
				return tex2Dproj(s_smap,tc+tc.w*ts).x;
			}
			float shadow_hw (float4 tc) 
			{
				float4 s0;
				s0.x=sample_hw_pcf(tc,float4(-1,-1,0,0));
				s0.y=sample_hw_pcf(tc,float4(+1,-1,0,0));
				s0.z=sample_hw_pcf(tc,float4(-1,+1,0,0));
				s0.w=sample_hw_pcf(tc,float4(+1,+1,0,0));
				float result=(s0.x+s0.y+s0.z+s0.w)/4.h;
				return result;
			}
			/*float sample_hw_pcf_far(float4 tc)
			{
				static const float ts=KERNEL/float(SMAP_size);
				float3 s0;
				s0.x=sample_hw_pcf(tc,float4(0.f,0.f,0,0));
				s0.y=sample_hw_pcf(tc,float4(1.f,0.f,0,0));
				s0.z=sample_hw_pcf(tc,float4(2.f,0.f,0,0));
				float3 s1;
				s1.x=sample_hw_pcf(tc,float4(0.f,1.f,0,0));
				s1.y=sample_hw_pcf(tc,float4(1.f,1.f,0,0));
				s1.z=sample_hw_pcf(tc,float4(2.f,1.f,0,0));
				float3 s2;
				s2.x=sample_hw_pcf(tc,float4(0.f,2.f,0,0));
				s2.y=sample_hw_pcf(tc,float4(1.f,2.f,0,0));
				s2.z=sample_hw_pcf(tc,float4(2.f,2.f,0,0));
				float2 weight=frac((tc.xy/tc.w)+ts);
				float3 result=lerp(lerp(s0,s1,weight.xxx),lerp(s0,s2,weight.xxx),weight.yyy);
				return result;
			}*/
		#endif
		float sample_hw_f4(float4 tc,float4 shift)
		{
			static const float ts=KERNEL/float(SMAP_size);
			float4 T4=tc/tc.w;
			T4.xy+=shift.xy*ts;
			float4 D4=tex2D(s_smap,T4);
			float4 compare=T4.z<D4;
			float texsize=SMAP_size;
			float2 fr=frac(T4.xy*texsize);
			float2 ifr=float2(1,1)-fr;
			float4 fr4=float4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
			float4 fr4s=fr4.zywx;
			return dot(compare,fr4s);
		}
		float shadow_hw_f4(float4 tc) 
		{
			float s0=sample_hw_f4(tc,float4(-1,-1,0,0));
			float s1=sample_hw_f4(tc,float4(+1,-1,0,0));
			float s2=sample_hw_f4(tc,float4(-1,+1,0,0));
			float s3=sample_hw_f4(tc,float4(+1,+1,0,0));
			return (s0+s1+s2+s3)/4.h;
		}
		float sample_hw_f4_far(float4 tc)
		{
			static const float ts=KERNEL/float(SMAP_size);
			float3 s0;
			s0.x=sample_hw_f4(tc,float4(0.f,0.f,0,0));
			s0.y=sample_hw_f4(tc,float4(1.f,0.f,0,0));
			s0.z=sample_hw_f4(tc,float4(2.f,0.f,0,0));
			float3 s1;
			s1.x=sample_hw_f4(tc,float4(0.f,1.f,0,0));
			s1.y=sample_hw_f4(tc,float4(1.f,1.f,0,0));
			s1.z=sample_hw_f4(tc,float4(2.f,1.f,0,0));
			float3 s2;
			s2.x=sample_hw_f4(tc,float4(0.f,2.f,0,0));
			s2.y=sample_hw_f4(tc,float4(1.f,2.f,0,0));
			s2.z=sample_hw_f4(tc,float4(2.f,2.f,0,0));
			float2 Weight=frac((tc.xy/tc.w)+ts);
			float Result=lerp(lerp(s0,s1,Weight.xxx),lerp(s0,s2,Weight.xxx),Weight.yyy);
			return (Result);
		}
		#ifdef USE_HWSMAP_PCF
			float shadow (float4 tc) { return shadow_hw (tc);}
		#else
			#ifdef USE_FETCH4
				float shadow (float4 tc) { return shadow_hw_f4 (tc);}
			#else
				float shadow (float4 tc) { return shadow_sw (tc);}
			#endif
		#endif
		float4 test(float4 tc,float2 offset) 
		{
			float4 tcx=float4(tc.xy+tc.w*offset,tc.zw);
			return tex2Dproj(s_smap,tcx);
		}
		float shadowtest(float4 tc,float4 tcJ) 
		{
			float4 r;
			const float scale=(2.7f/float(SMAP_size));
			float4 J0=tex2Dproj(jitter0,tcJ)*scale;
			float4 J1=tex2Dproj(jitter1,tcJ)*scale;
			r.x=test(tc,J0.xy).x;
			r.y=test(tc,J0.wz).y;
			r.z=test(tc,J1.xy).z;
			r.w=test(tc,J1.wz).x;
			return dot(r,1.h/4.h);
		}
		float shadowtest_sun(float4 tc,float4 tcJ) 
		{
			float4 r;
			const float scale=(2.0f/float(SMAP_size));
			float4 J0=tex2D(jitter0,tcJ)*scale;
			float4 J1=tex2D(jitter1,tcJ)*scale;
			r.x=test(tc,J0.xy).x;
			r.y=test(tc,J0.wz).y;
			r.z=test(tc,J1.xy).z;
			r.w=test(tc,J1.wz).x;
			return dot(r,1.h/4.h);
		}
		uniform float3x4 m_sunmask;
		float sunmask(float4 P) 
		{
			float2 tc=mul(m_sunmask,P);
			return tex2D(s_lmap,tc).w;
		}
	#endif
	uniform float4x4 m_shadow;

#else		//dx9
	
	uniform sampler s_smap:register(ps,s0);
	#ifndef KERNEL
		#define KERNEL .6f
	#endif
	half sample_sw(float2 tc,float2 shift,float depth_cmp) {
	static const float ts=KERNEL/float(SMAP_size);
	tc+=shift*ts;
	float texsize=SMAP_size;
	float offset=0.5f/texsize;
	float2 Tex00=tc+float2(-offset,-offset);
	float2 Tex01=tc+float2(-offset,offset);
	float2 Tex10=tc+float2(offset,-offset);
	float2 Tex11=tc+float2(offset,offset);
	float4 depth=float4(
	depth_cmp-tex2D(s_smap,Tex00).x,
	depth_cmp-tex2D(s_smap,Tex01).x,
	depth_cmp-tex2D(s_smap,Tex10).x,
	depth_cmp-tex2D(s_smap,Tex11).x);
	half4 compare=step(depth,0);
	float2 fr=frac(Tex00*texsize);
	half2 ifr=half2(1,1)-fr;
	half4 fr4=half4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
	return dot (compare,fr4);
	}
	half shadow_sw(float4 tc) { 
	float2 tc_dw=tc.xy/tc.w;
	half4 s;
	s.x=sample_sw(tc_dw,float2(-1,-1),tc.z);
	s.y=sample_sw(tc_dw,float2(+1,-1),tc.z);
	s.z=sample_sw(tc_dw,float2(-1,+1),tc.z);
	s.w=sample_sw(tc_dw,float2(+1,+1),tc.z);
	return dot(s,1.h/4.h);
	}
	#ifndef USE_F32
		half sample_hw_pcf(float4 tc,float4 shift){
		static const float ts=KERNEL/float(SMAP_size);
		return tex2Dproj(s_smap,tc+tc.w*shift*ts).x;
		}
		half shadow_hw(float4 tc){
		half s0=sample_hw_pcf(tc,float4(-1,-1,0,0));
		half s1=sample_hw_pcf(tc,float4(+1,-1,0,0));
		half s2=sample_hw_pcf(tc,float4(-1,+1,0,0));
		half s3=sample_hw_pcf(tc,float4(+1,+1,0,0));
		return (s0+s1+s2+s3)/4.h;
		}
		half sample_hw_f4(float4 tc,float4 shift){
		static const float ts=KERNEL/float(SMAP_size);
		float4 T4=tc/tc.w;
		T4.xy+=shift.xy*ts;
		float4 D4=tex2D(s_smap,T4);
		float4 compare=T4.z<D4;
		float texsize=SMAP_size;
		float2 fr=frac(T4.xy*texsize);
		half2 ifr=half2(1,1)-fr;
		half4 fr4=half4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
		half4 fr4s=fr4.zywx;
		return dot(compare,fr4s);
		}
		half shadow_hw_f4(float4 tc) {
		half s0=sample_hw_f4(tc,float4(-1,-1,0,0));
		half s1=sample_hw_f4(tc,float4(+1,-1,0,0));
		half s2=sample_hw_f4(tc,float4(-1,+1,0,0));
		half s3=sample_hw_f4(tc,float4(+1,+1,0,0));
		return (s0+s1+s2+s3)/4.h;
		}
		#ifdef USE_HWSMAP_PCF
			half shadow(float4 tc) { return shadow_hw (tc);}
		#else
			#ifdef USE_FETCH4
				half shadow(float4 tc) { return shadow_hw_f4(tc);}
			#else
				half shadow(float4 tc) { return shadow_sw (tc);}
			#endif
		#endif
		uniform sampler2D jitter0;
		uniform sampler2D jitter1;
		uniform sampler2D jitter2;
		uniform sampler2D jitter3;
		uniform half4 jitterS;
		half4 test(float4 tc,half2 offset) {
		float4 tcx=float4(tc.xy+tc.w*offset,tc.zw);
		return tex2Dproj(s_smap,tcx);
		}
		half shadowtest(float4 tc,float4 tcJ) {
		half4 r;
		const float scale=(2.7f/float(SMAP_size));
		half4 J0=tex2Dproj(jitter0,tcJ)*scale;
		half4 J1=tex2Dproj(jitter1,tcJ)*scale;
		r.x=test(tc,J0.xy).x;
		r.y=test(tc,J0.wz).y;
		r.z=test(tc,J1.xy).z;
		r.w=test(tc,J1.wz).x;
		return dot(r,1.h/4.h);
		}
		half shadowtest_sun(float4 tc,float4 tcJ) {
		half4 r;
		const float scale=(2.0f/float(SMAP_size));
		half4 J0=tex2D(jitter0,tcJ)*scale;
		half4 J1=tex2D(jitter1,tcJ)*scale;
		r.x=test(tc,J0.xy).x;
		r.y=test(tc,J0.wz).y;
		r.z=test(tc,J1.xy).z;
		r.w=test(tc,J1.wz).x;
		return dot(r,1.h/4.h);
		}
		#ifdef USE_SUNMASK 
			uniform float3x4 m_sunmask;
			half sunmask(float4 P) {
			float2 tc=mul(m_sunmask,P);
			return tex2D(s_lmap,tc).w;
			}
		#else
			half sunmask (float4 P) { return 1.h;}
		#endif
	#else
		#ifdef USE_HWSMAP_PCF
			half sample_hw_pcf(float4 tc,float4 shift){
			static const float ts=KERNEL/float(SMAP_size);
			return tex2Dproj(s_smap,tc+tc.w*shift*ts).x;
			}
			half shadow_hw (float4 tc) {
			float4 s0;
			s0.x=sample_hw_pcf(tc,float4(-1,-1,0,0));
			s0.y=sample_hw_pcf(tc,float4(+1,-1,0,0));
			s0.z=sample_hw_pcf(tc,float4(-1,+1,0,0));
			s0.w=sample_hw_pcf(tc,float4(+1,+1,0,0));
			float result=(s0.x+s0.y+s0.z+s0.w)/4.h;
			return result;
			}
			half sample_hw_pcf_far(float4 tc){
			static const float ts=KERNEL/float(SMAP_size);
			float3 s0;
			s0.x=sample_hw_pcf(tc,float4(0.f,0.f,0,0));
			s0.y=sample_hw_pcf(tc,float4(1.f,0.f,0,0));
			s0.z=sample_hw_pcf(tc,float4(2.f,0.f,0,0));
			float3 s1;
			s1.x=sample_hw_pcf(tc,float4(0.f,1.f,0,0));
			s1.y=sample_hw_pcf(tc,float4(1.f,1.f,0,0));
			s1.z=sample_hw_pcf(tc,float4(2.f,1.f,0,0));
			float3 s2;
			s2.x=sample_hw_pcf(tc,float4(0.f,2.f,0,0));
			s2.y=sample_hw_pcf(tc,float4(1.f,2.f,0,0));
			s2.z=sample_hw_pcf(tc,float4(2.f,2.f,0,0));
			float2 Weight=frac((tc.xy/tc.w)+ts);
			float3 Result=lerp(lerp(s0,s1,Weight.xxx),lerp(s0,s2,Weight.xxx),Weight.yyy);
			return (Result);
			}
		#endif
		half sample_hw_f4(float4 tc,float4 shift){
		static const float ts=KERNEL/float(SMAP_size);
		float4 T4=tc/tc.w;
		T4.xy+=shift.xy*ts;
		float4 D4=tex2D(s_smap,T4);
		float4 compare=T4.z<D4;
		float texsize=SMAP_size;
		float2 fr=frac(T4.xy*texsize);
		half2 ifr=half2(1,1)-fr;
		half4 fr4=half4(ifr.x*ifr.y,ifr.x*fr.y,fr.x*ifr.y,fr.x*fr.y);
		half4 fr4s=fr4.zywx;
		return dot(compare,fr4s);
		}
		half shadow_hw_f4(float4 tc) {
		half s0=sample_hw_f4(tc,float4(-1,-1,0,0));
		half s1=sample_hw_f4(tc,float4(+1,-1,0,0));
		half s2=sample_hw_f4(tc,float4(-1,+1,0,0));
		half s3=sample_hw_f4(tc,float4(+1,+1,0,0));
		return (s0+s1+s2+s3)/4.h;
		}
		half sample_hw_f4_far(float4 tc){
		static const float ts=KERNEL/float(SMAP_size);
		float3 s0;
		s0.x=sample_hw_f4(tc,float4(0.f,0.f,0,0));
		s0.y=sample_hw_f4(tc,float4(1.f,0.f,0,0));
		s0.z=sample_hw_f4(tc,float4(2.f,0.f,0,0));
		float3 s1;
		s1.x=sample_hw_f4(tc,float4(0.f,1.f,0,0));
		s1.y=sample_hw_f4(tc,float4(1.f,1.f,0,0));
		s1.z=sample_hw_f4(tc,float4(2.f,1.f,0,0));
		float3 s2;
		s2.x=sample_hw_f4(tc,float4(0.f,2.f,0,0));
		s2.y=sample_hw_f4(tc,float4(1.f,2.f,0,0));
		s2.z=sample_hw_f4(tc,float4(2.f,2.f,0,0));
		float2 Weight=frac((tc.xy/tc.w)+ts);
		float Result=lerp(lerp(s0,s1,Weight.xxx),lerp(s0,s2,Weight.xxx),Weight.yyy);
		return (Result);
		}
		#ifdef USE_HWSMAP_PCF
			half shadow (float4 tc) { return shadow_hw (tc);}
		#else
			#ifdef USE_FETCH4
				half shadow (float4 tc) { return shadow_hw_f4 (tc);}
			#else
				half shadow (float4 tc) { return shadow_sw (tc);}
			#endif
		#endif
		uniform sampler2D jitter0;
		uniform sampler2D jitter1;
		uniform sampler2D jitter2;
		uniform sampler2D jitter3;
		uniform half4 jitterS;
		half4 test(float4 tc,half2 offset) {
		float4 tcx=float4(tc.xy+tc.w*offset,tc.zw);
		return tex2Dproj(s_smap,tcx);
		}
		half shadowtest(float4 tc,float4 tcJ) {
		half4 r;
		const float scale=(2.7f/float(SMAP_size));
		half4 J0=tex2Dproj(jitter0,tcJ)*scale;
		half4 J1=tex2Dproj(jitter1,tcJ)*scale;
		r.x=test(tc,J0.xy).x;
		r.y=test(tc,J0.wz).y;
		r.z=test(tc,J1.xy).z;
		r.w=test(tc,J1.wz).x;
		return dot(r,1.h/4.h);
		}
		half shadowtest_sun(float4 tc,float4 tcJ) {
		half4 r;
		const float scale=(2.0f/float(SMAP_size));
		half4 J0=tex2D(jitter0,tcJ)*scale;
		half4 J1=tex2D(jitter1,tcJ)*scale;
		r.x=test(tc,J0.xy).x;
		r.y=test(tc,J0.wz).y;
		r.z=test(tc,J1.xy).z;
		r.w=test(tc,J1.wz).x;
		return dot(r,1.h/4.h);
		}
		uniform float3x4 m_sunmask;
		half sunmask(float4 P) {
		float2 tc=mul(m_sunmask,P);
		return tex2D(s_lmap,tc).w;
		}
	#endif
	uniform float4x4 m_shadow;
	
#endif		//dx11
#endif		//shadow_h