#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include "Math/Vector.h"

namespace RcEngine
{
	namespace Math
	{
		enum ContainmentType 
		{
			CT_Disjoint,
			CT_Contains,
			CT_Intersects
		};

		template<typename Real>
		class BoundingSphere
		{
		public:
			BoundingSphere(){ }

			BoundingSphere(const Vector<Real, 3>& center, Real radius)
				: Center(center), Radius(radius) {}

			BoundingSphere(const BoundingSphere<Real>& rhs)
				: Center(rhs.Center), Radius(rhs.Radius) {}

			BoundingSphere& operator = (const BoundingSphere<Real>& rhs)
			{
				if (this != &rhs)
				{
					Center = rhs.Center;
					Radius = rhs.Radius;
				}
				return *this;
			}

			bool operator == ( const BoundingSphere<Real>& rhs)
			{
				return (Center == rhs.Center) && (Radius == rhs.Radius);
			}

		public:
			Vector<Real, 3> Center;
			Real Radius;
		};

		typedef BoundingSphere<float> BoundingSpheref;
		typedef BoundingSphere<double> BoundingSphered;

		template<typename Real>
		class BoundingBox
		{
		public:
			BoundingBox();

			BoundingBox(const Vector<Real,3>& min, const Vector<Real,3>& max)
				: Min(min), Max(max){ }

			BoundingBox(const BoundingBox<Real>& rhs)
				: Min(rhs.Min), Max(rhs.Max) { }

			BoundingBox& operator = (const BoundingBox<Real>& rhs)
			{
				if (this != &rhs)
				{
					Max = rhs.Max;
					Min = rhs.Min;
				}
				return *this;
			}

			bool operator == ( const BoundingBox<Real>& rhs)
			{
				return (Min == rhs.Min) && (Max == rhs.Max);
			}

		public:
			Vector<Real,3> Max;
			Vector<Real,3> Min;
		};

		typedef BoundingBox<float> BoundingBoxf;
		typedef BoundingBox<double> BoundingBoxd;
	}
}

#endif // BoundingSphere_h__
