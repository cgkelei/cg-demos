#include <Graphics/EffectParameter.h>

namespace RcEngine {

void EffectParameter::GetValue(bool& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<bool>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float>& value)    { throw std::exception(); }
void EffectParameter::GetValue(int& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<int32_t>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float4x4& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float4x4>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float2& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float2>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float3& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float3>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float4& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float4>& value)    { throw std::exception(); }
void EffectParameter::GetValue( TextureLayer& textureLayer ) { throw std::exception(); }


void EffectParameter::SetValue(const bool& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<bool>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const int& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<int32_t>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float4x4& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float4x4>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float2& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float2>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float3& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float3>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float4& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float4>& value)    { throw std::exception(); }
void EffectParameter::SetValue( const TextureLayer& textureLayer ) { throw std::exception(); }

} // Namespace RcEngine