#include <Graphics/EffectParameter.h>

namespace RcEngine {

void EffectParameter::GetValue(bool& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<bool>& value)    { throw std::exception(); }
void EffectParameter::GetValue(float& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<float>& value)    { throw std::exception(); }
void EffectParameter::GetValue(int& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<int32_t>& value)    { throw std::exception(); }
void EffectParameter::GetValue(Matrix4f& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<Matrix4f>& value)    { throw std::exception(); }
void EffectParameter::GetValue(Vector2f& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<Vector2f>& value)    { throw std::exception(); }
void EffectParameter::GetValue(Vector3f& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<Vector3f>& value)    { throw std::exception(); }
void EffectParameter::GetValue(Vector4f& value)    { throw std::exception(); }
void EffectParameter::GetValue(vector<Vector4f>& value)    { throw std::exception(); }
void EffectParameter::GetValue( TextureLayer& textureLayer ) { throw std::exception(); }


void EffectParameter::SetValue(const bool& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<bool>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const float& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<float>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const int& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<int32_t>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const Matrix4f& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<Matrix4f>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const Vector2f& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<Vector2f>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const Vector3f& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<Vector3f>& value)    { throw std::exception(); }
void EffectParameter::SetValue(const Vector4f& value)    { throw std::exception(); }
void EffectParameter::SetValue(const vector<Vector4f>& value)    { throw std::exception(); }
void EffectParameter::SetValue( const TextureLayer& textureLayer ) { throw std::exception(); }

} // Namespace RcEngine