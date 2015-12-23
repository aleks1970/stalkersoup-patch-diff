//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"
#ifndef HMODEL_H
#define HMODEL_H

#ifdef DX11
	uniform samplerCUBE env_s0;
	uniform samplerCUBE env_s1;
	uniform samplerCUBE sky_s0;
	uniform samplerCUBE sky_s1;
	uniform float4 env_color;
	uniform float3x4 m_v2w;
	#ifndef USE_F32
		void hmodel (out float3 hdiffuse,out float3 hspecular,float m,float h,float s,float3 pnt,float3 normal)
		{
		float3 nw=mul(m_v2w,normal);
		float hscale=h;
		#ifdef USE_GAMMA_22
			hscale=(hscale*hscale);
		#endif
		float3 v2pntL=normalize(pnt);
		float3 v2pnt=mul(m_v2w,v2pntL);
		float3 vreflect=reflect(v2pnt,nw);
		float hspec=.5h+.5h*dot(vreflect,v2pnt);
		float4 light=tex3D(s_material,float3(hscale,hspec,m));
		float3 e0d=texCUBE(env_s0,nw).xyz;
		float3 e1d=texCUBE(env_s1,nw).xyz;
		float3 env_d=env_color.xyz*lerp(e0d,e1d,env_color.w);
		env_d*=env_d;
		hdiffuse=env_d*light.xyz+L_ambient.rgb;
		vreflect.y=vreflect.y*2-1;
		float3 e0s=texCUBE(env_s0,vreflect).xyz;
		float3 e1s=texCUBE(env_s1,vreflect).xyz;
		float3 env_s=env_color.xyz*lerp(e0s,e1s,env_color.w);
		env_s*=env_s;
		hspecular=env_s*light.w*s;
		}
	#else
		void hmodel (out float3 hdiffuse,out float3 hspecular,float m,float h,float s,float3 pnt,float3 normal)
		{
		float3 nw=mul(m_v2w,normal);
		float hscale=h;
		float3 N=normalize(normal);
		float3 V=normalize(pnt);
		float3 L=-Ldynamic_dir;
		float3 Ha=normalize(L+V);
		float4 LV=float4((saturate(dot(nw,Ha))),(saturate(dot(V,Ha))),
		(saturate(dot(nw,V))),(saturate(dot(N,L))));
		float NH2=pow(LV.x,2);
		const float Pi=3.1415926535;
		float m2=m*2;
		float Bd=(m/m2*NH2*NH2)*(exp(-((m-NH2)/(m2*NH2))));
		float SS=min(1,min((2*LV.x*LV.w)/LV.y,(2*LV.x*LV.z)/LV.y));
		float Si=(Bd*SS);
		float rets=(1.5f*((L_sun_color)*LV.w)*saturate(LV.w*(1*((LV.w*Si*m2))))*nw);
		float3 e0d=texCUBE(env_s0,nw);
		float3 e1d=texCUBE(env_s1,nw);
		float3 vP=mul (m_v2w,V);
		float3 REFL=reflect(vP,nw);
		float3 e0s=texCUBE(env_s0,REFL);
		float3 e1s=texCUBE(env_s1,REFL);
		float3 env_s=env_color.xyz*lerp(e0s,e1s,env_color.w);
		float3 env_d=env_color.xyz*((lerp(e0d,e1d,env_color.w)));
		float4 light=tex3D(s_material,float3(hscale,rets,m2));
		env_d*=env_d;
		env_s*=env_s;
		float hsp=dot(REFL,vP);
		float4 ambient=float4(0.008f,0.008f,0.008f,(hscale));
		float4 ls=tex3D (s_material,float3(hscale,hsp,m));
		hdiffuse=(env_d)*(light.xyz)+((L_ambient.rgb));
		if (m>=0.1f) 
		hspecular=env_s*light.w*s;
		else
		hspecular=env_s*ls.w*s;
		}
	#endif
	void hmodel_table (out float3 hdiffuse,out float3 hspecular,float m,float h,float s,float3 pnt,float3 normal)
	{
	float hscale=h;
	float3 v2pnt=normalize(pnt);
	float3 vreflect=reflect(v2pnt,normal);
	float hspec=.5h+.5h*dot(vreflect,v2pnt);
	float4 light=tex3D(s_material,float3(hscale,hspec,m));
	float3 env_d=texCUBE(env_s0,normal).xyz;
	float3 env_s=texCUBE(env_s0,vreflect).xyz;
	hdiffuse=env_d*light.xyz+L_ambient.rgb;
	hspecular=env_s*light.w*s;
	}
	//#endif

#else
	
	uniform samplerCUBE env_s0;
	uniform samplerCUBE env_s1;
	uniform samplerCUBE sky_s0;
	uniform samplerCUBE sky_s1;
	uniform half4 env_color;
	uniform half3x4 m_v2w;
	#ifndef USE_F32
		void hmodel (out half3 hdiffuse,out half3 hspecular,half m,half h,half s,float3 point,half3 normal)
		{
		half3 nw=mul(m_v2w,normal);
		half hscale=h;
		#ifdef USE_GAMMA_22
			hscale=(hscale*hscale);
		#endif
		float3 v2pointL=normalize(point);
		half3 v2point=mul(m_v2w,v2pointL);
		half3 vreflect=reflect(v2point,nw);
		half hspec=.5h+.5h*dot(vreflect,v2point);
		half4 light=tex3D(s_material,half3(hscale,hspec,m));
		half3 e0d=texCUBE(env_s0,nw);
		half3 e1d=texCUBE(env_s1,nw);
		half3 env_d=env_color.xyz*lerp(e0d,e1d,env_color.w);
		env_d*=env_d;
		hdiffuse=env_d*light.xyz+L_ambient.rgb;
		vreflect.y=vreflect.y*2-1;
		half3 e0s=texCUBE(env_s0,vreflect);
		half3 e1s=texCUBE(env_s1,vreflect);
		half3 env_s=env_color.xyz*lerp(e0s,e1s,env_color.w);
		env_s*=env_s;
		hspecular=env_s*light.w*s;
		}
	#else
		void hmodel (out half3 hdiffuse,out half3 hspecular,half m,half h,half s,float3 point,half3 normal)
		{
		half3 nw=mul(m_v2w,normal);
		half hscale=h;
		half3 N=normalize(normal);
		half3 V=normalize(point);
		half3 L=-Ldynamic_dir;
		float3 Ha=normalize(L+V);
		float4 LV=float4((saturate(dot(nw,Ha))),(saturate(dot(V,Ha))),
		(saturate(dot(nw,V))),(saturate(dot(N,L))));
		float NH2=pow(LV.x,2);
		const float Pi=3.1415926535;
		half m2=m*2;
		float Bd=(m/m2*NH2*NH2)*(exp(-((m-NH2)/(m2*NH2))));
		float SS=min(1,min((2*LV.x*LV.w)/LV.y,(2*LV.x*LV.z)/LV.y));
		float Si=(Bd*SS);
		float rets=(1.5f*((L_sun_color)*LV.w)*saturate(LV.w*(1*((LV.w*Si*m2))))*nw);
		half3 e0d=texCUBE(env_s0,nw);
		half3 e1d=texCUBE(env_s1,nw);
		half3 vP=mul (m_v2w,V);
		half3 REFL=reflect(vP,nw);
		half3 e0s=texCUBE(env_s0,REFL);
		half3 e1s=texCUBE(env_s1,REFL);
		half3 env_s=env_color.xyz*lerp(e0s,e1s,env_color.w);
		half3 env_d=env_color.xyz*((lerp(e0d,e1d,env_color.w)));
		float4 light=tex3D(s_material,half3(hscale,rets,m2));
		env_d*=env_d;
		env_s*=env_s;
		half hsp=dot(REFL,vP);
		float4 ambient=float4(0.008f,0.008f,0.008f,(hscale));
		half4 ls=tex3D (s_material,half3(hscale,hsp,m));
		hdiffuse=(env_d)*(light.xyz)+((L_ambient.rgb));
		if (m>=0.1f) 
		hspecular=env_s*light.w*s;
		else
		hspecular=env_s*ls.w*s;
		}
	#endif
	void hmodel_table (out half3 hdiffuse,out half3 hspecular,half m,half h,half s,half3 point,half3 normal)
	{
	half hscale=h;
	half3 v2point=normalize(point);
	half3 vreflect=reflect(v2point,normal);
	half hspec=.5h+.5h*dot(vreflect,v2point);
	half4 light=tex3D(s_material,half3(hscale,hspec,m));
	half3 env_d=texCUBE(env_s0,normal);
	half3 env_s=texCUBE(env_s0,vreflect);
	hdiffuse=env_d*light.xyz+L_ambient.rgb;
	hspecular=env_s*light.w*s;
	}

#endif

#endif