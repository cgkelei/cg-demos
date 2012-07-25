#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include "Math/Vector.h"

namespace RcEngine
{
	namespace Math
	{
		template<typename Real>
		class BoundingSphere
		{
		public:
			BoundingSphere();
			BoundingSphere(const Vector<Real, 3>& center, Real radius);

			inline Vector<Real, 3>  Center() const;
			inline Vector<Real, 3>& Center();

			inline Real  Radius() const;
			inline Real& Radius();

		public:
			Vector<Real, 3> mCenter;
			Real mRadius;
		};

		#include "BoundingSphere.inl"

		typedef BoundingSphere<float> BoundingSpheref;
		typedef BoundingSphere<double> BoundingSphered;

	}
}

#endif // BoundingSphere_h__
