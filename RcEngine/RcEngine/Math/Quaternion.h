#ifndef Quaternion_h__
#define Quaternion_h__

namespace RcEngine{


template<typename Real>
class Quaternion
{
public:
	Quaternion();
	Quaternion(Real w, Real x, Real y, Real z);
	Quaternion(const Quaternion& rhs);

	void MakeIdentity();

	inline Real operator[] (int i) const;
	inline Real& operator[] (int i);
	inline Real W () const;
	inline Real& W ();
	inline Real X () const;
	inline Real& X ();
	inline Real Y () const;
	inline Real& Y ();
	inline Real Z () const;
	inline Real& Z ();

	// assignment operators
	Quaternion& operator += (const Quaternion& rhs);
	Quaternion& operator -= (const Quaternion& rhs);
	Quaternion& operator *= (const Quaternion& rhs);
	Quaternion& operator *= (float scalar);
	Quaternion& operator /= (float scalar);

	// binary operators
	Quaternion operator + (const Quaternion& rhs) const;
	Quaternion operator - (const Quaternion& rhs) const;
	Quaternion operator * (const Quaternion& rhs) const;
	Quaternion operator * (float scalar) const;
	Quaternion operator / (float scalar) const;

	bool operator == (const Quaternion& rhs) const;
	bool operator != (const Quaternion& rhs) const;

	inline static const Quaternion& Identity();


private:
	// support for comparisons
	int CompareArrays (const Quaternion& rhs) const;

	Real mTuple[4];
};

#include <Math/Quaternion.inl>

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;


} // Namespace RcEngine


#endif // Quaternion_h__