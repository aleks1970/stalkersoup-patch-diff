//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
float4 wetreflect(float4 f,float2 W,float z,float t,bool L,bool x):COLOR{float4 w=f;
#ifdef WET_BUILDINGREF
x=true;
#endif
float4 s=tex2Dlod(s_image,float4(W.xy,0,0)),r=tex2Dlod(s_position,float4(W,0,0));int l=mcls(r.w);if(l==1&&x)z=WET_REF_BUILD;float i=0,e=0;if(WET_DEBUGVAL>0.||s.w<WET_MAXALPHA&&z>0){float y=saturate(1/r.z),p=1;if(WET_DEBUGVAL>.1||(l==2||l==5||l==6||l==1&&(L||x))){float R=W.y-normalize(eye_direction).y*WET_YCORR;float3 a=float3(W.x,R,r.z),m=tex2Dlod(s_normal,float4(W,0,0)).xyz,b=normalize(mul(m,(float3x3)m_WV));float d=pow((1-b.x)*(1-b.z)*b.y,2);float4 n=lerp(tex2Dlod(s_image,float4(W,0,0)),tex2Dlod(s_bloom,float4(W,0,0)),WET_BLOOM_MIX);float o=(s.x+s.y+s.z)/3,h=(n.x+n.y+n.z)/3,A=.08;e_barrier.z;float T=.35;e_barrier.y;float D=.35;e_weights.z;float N=.46;e_weights.y;float M=.01;if(L_sun_color.x<M&&L_sun_color.y<M&&L_sun_color.z<M)n=2*L_hemi_color*n*9;else{n=2*L_hemi_color*n*9/pow(float4(L_sun_color,1),.5);float E=(L_hemi_color.x+L_hemi_color.y+L_hemi_color.z)/3;if(E>WET_HEMILUM)N=.86;
#define WET_COLOR_SATURATION float(0.8)
float F=dot(n.xyz,float3(.3,.59,.11));n.xyz=lerp(float3(F,F,F),n.xyz,WET_COLOR_SATURATION+dot(Ldynamic_color.xyz,float3(1,1,1))/3*0);}n=normalize(normalize_blue(n))/2;float E=n.z/((n.x+n.y+n.z)/3),F=l==2?WET_MAXCOF:l==1?WET_MAXCOF_BUILD:l==5?WET_MAXCOF_VEHIC:WET_MAXCOF_METAL,X=1000*F*z;i=saturate((n.z-A+.01)*(T-n.x+.01)*(D-n.y+.01)*(E-N+.01)*X);float Q=n.z*E*(1-n.x)*(1-n.y),V=l==1?WET_NORMALIN:WET_NORMAL;
#ifdef WET_CONTRAST
if(d>V&&z>0){float3 P=f.xyz*f.xyz;f.xyz=lerp(f.xyz,P,WET_CONTRAST*F*z*pow(Q,WET_CONTRASTBLUE));}
#endif
float P=(L_sun_color.x+L_sun_color.y+L_sun_color.z)/3;bool O=false;
#ifdef WET_COLDETECT
O=true;
#endif
if(WET_DEBUGVAL>.2||(!O||P<WET_SUNLUM||n.z>A&&n.x<T&&n.y<D&&E>N&&r.z<WET_MAXDIST&&o>WET_MINLUM)){{if(e_kernel.x==.7)f=decolor(f);if(WET_DEBUGVAL>.3||d>V){float U=W.x,Y=t<WET_RAINTHR?-(a.y-.5)+.5+sin(a.y*100*p*WET_WAVE/pow(y,2))*.03:abs(-(a.y-.5)+.5+sin(a.y*100*p*WET_WAVE/pow(y,2))*.03);if(Y<0)Y=abs(Y+sin(a.y*100*pow(abs(Y*100),2)*WET_WAVENEG/pow(y,2))*.03),i-=pow(abs(Y),.5);if(Y>1)Y=2-Y;if(l==2){float B=t*WET_DISAMP,I=t*WET_DISFRQ,C=t*WET_DISPER;float2 g=xrefract(float2(U,Y),B,I,C,pow(y,.5),timers.x);U=g.x;Y=g.y;}float4 g=tex2Dlod(s_bloom,float4(U,Y,0,0)),B=0;if(l!=1||L)B=tex2Dlod(s_image,float4(U,Y,0,0)),B=lerp(g,B,saturate((1-WET_BLOOM)*z-WET_RAINBLOOM*t));else B=g;
#ifdef WET_REFBLUR
if(l!=1||L){float I=1.h,C=i*WET_REFBLUR,u=0.f,c=C/WET_REFBLURQUALITY;float3 G=0;for(int S=0;S<WET_REFBLURQUALITY;S++)G+=gaussblur(float2(U,Y),u).xyz,I++,u+=c;B.xyz+=G/I-B;}
#endif
float I=(B.x+B.y+B.z)*B.w/3,C=d;if(l!=1||L||x){float3 S=f.xyz*saturate(1-y*WET_BASE*2),u=i*S+(1-i)*f.xyz;f.xyz=u;if(z>WET_NORMALFLOOD&&C<z)C=z;}e=i*WET_REF*saturate(pow(WET_LUM*h,WET_LUMCOF))*C;B*=e*y;if(d>V)B=B+WET_BLOOM_COL*g+WET_BLOOM_REF*g;
#ifdef WET_COLFILTER
if(l==1&&L){float u=dot(f.xyz,float3(.3,.59,.11));f=1*float4(lerp(float3(u,u,u),f.xyz,.1)*float3(.1,.1,0),1)+B*WET_COLFILTER;}else f=1*f+B;
#else
f=1*f+B;
#endif

#ifdef WET_OBJREFLECT
f=objreflect(f,r,l,L,x,W,y,p,WET_OBJMAXCOL,e,t);
#endif
}
#ifdef WET_BLUR
float B=1.h,C=i*WET_BLUR,Y=0.f,u=C/WET_BLURQUALITY;float3 g=0;for(int S=0;S<WET_BLURQUALITY;S++)g+=gaussblur(W,Y).xyz,B++,Y+=u;f.xyz+=g/B-tex2Dlod(s_image,float4(W,0,0)).xyz;
#endif
f.w=.1;}}else f.w=1.;}else f.w=1.;}if(e==0){if(s.w<WET_MAXALPHA&&l==2){float n=s.z*2-s.x-s.y;if(n>WET_BLUEDIFF)f.z*=WET_BLUECORR;}}else{float Y=dot(f.xyz,float3(.3,.59,.11));f.xyz=lerp(float3(Y,Y,Y),f.xyz,WET_SAT);}return f;}