//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#ifndef ANTIALIASING
#define ANTIALIASING
float3 aa(v2p I)
{
float3 result;
if (true) {
//if (e_kernel.x > 0.4) {
float3 nc=tex2D(s_normal,I.tc0.xy).rgb;
float4 nd;
nd.x=dot(nc,(float3)tex2D(s_normal,I.tc1.xy).xyz);
nd.y=dot(nc,(float3)tex2D(s_normal,I.tc2.xy).xyz);
nd.z=dot(nc,(float3)tex2D(s_normal,I.tc3.xy).xyz);
nd.w=dot(nc,(float3)tex2D(s_normal,I.tc4.xy).xyz);
nd-=AA_BARRIER_X;
nd=step(0,nd);
float ne=saturate(dot(nd,AA_WEIGHTS_X));
float4 tc5r=I.tc5.wzyx;
float4 tc6r=I.tc6.wzyx;
float4 dc=tex2D(s_position,I.tc0.xy);
float4 dd;
dd.x=(float)tex2D(s_position,I.tc1.xy).z+(float)tex2D(s_position,I.tc2.xy).z;
dd.y=(float)tex2D(s_position,I.tc3.xy).z+(float)tex2D(s_position,I.tc4.xy).z;
dd.z=(float)tex2D(s_position,I.tc5.xy).z+(float)tex2D(s_position,tc5r.xy).z;
dd.w=(float)tex2D(s_position,I.tc6.xy).z+(float)tex2D(s_position,tc6r.xy).z;
dd=abs(2*dc.z-dd)-AA_BARRIER_Y;
dd=step(dd,0);
float de=saturate(dot(dd,AA_WEIGHTS_Y));
float w=(1-de*ne)*AA_KERNEL;
#ifdef USE_DISTORT
float4 distort2=tex2D(s_distort,I.tc0.xy);
float2 doffs=(distort2.xy-.5h)*def_distort;
float2 center=I.tc0.xy+doffs;
#else
float2 center=I.tc0.xy;
#endif
float2 offset2=center*(1-w);
float4 s0=tex2D(s_image,offset2+I.tc1.xy*w);
float4 s1=tex2D(s_image,offset2+I.tc2.xy*w);
float4 s2=tex2D(s_image,offset2+I.tc3.xy*w);
float4 s3=tex2D(s_image,offset2+I.tc4.xy*w);
result = ((s0+s1+s2+s3)/4.h).xyz;
//float3 final=mblur(center,dc, result);
//return combine_bloom(final,tex2D(s_bloom,I.tc0));
}
else result = tex2D(s_image,I.tc0.xy).xyz;
return result;
}
#endif