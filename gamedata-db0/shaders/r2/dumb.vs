//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"

#ifdef DX11
struct v2p {
float4 hpos:POSITION;
};
v2p main(float4 P:POSITION) {
v2p O;
O.hpos=mul(m_WVP,P);
return O;
}
FXVS;

#else

struct v2p {
float4 hpos:POSITION;
};
v2p main(float4 P:POSITION) {
v2p O;
O.hpos=mul(m_WVP,P);
return O;
}
FXVS;

#endif