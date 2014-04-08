#include <Graphics/SamplerState.h>

namespace RcEngine {

SamplerStateDesc::SamplerStateDesc()
	: AddressU(TAM_Clamp), 
	  AddressV(TAM_Clamp), 
	  AddressW(TAM_Clamp), 
	  Filter(TF_Min_Mag_Mip_Linear),
	  BorderColor(1, 1, 1, 1),
	  MipLODBias(0),
	  MinLOD(-FLT_MAX),
	  MaxLOD(FLT_MAX),
	  MaxAnisotropy(1),
	  ComparisonFunc(CF_Never),
	  BindStage(ST_Pixel),
	  CompareSampler(false)
{

}

//bool operator< (const SamplerStateDesc& lhs, const SamplerStateDesc& rhs)
//{
//	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
//}

}