template<typename Real>
Rectangle<Real>::Rectangle( Real x, Real y, Real width, Real height )
	: X(x), Y(y), Width(width), Height(height)
{

}

template<typename Real>
Rectangle<Real>::Rectangle( const Rectangle<Real>& other )
	: X(other.X), Y(other.Y), Width(other.Width), Height(other.Height)
{

}

template<typename Real>
bool Rectangle<Real>::Intersects( const Rectangle<Real>& other ) const
{
	return Intersects(other.X, other.Y, other.Width, other.Height);
}

template<typename Real>
bool Rectangle<Real>::Intersects( Real x, Real y, Real width, Real height ) const
{
	Real t;
	if ((t = x - this->X) > this->Width || -t > width)
		return false;
	if ((t = y - this->Y) > this->Height || -t > height)
		return false;
	return true;
}

template<typename Real>
ContainmentType Rectangle<Real>::Contains( const Rectangle<Real>& other ) const
{
	if (Right() < other.Left() || Left() > other.Right())
		return CT_Disjoint;	

	if( Bottom() < other.Top() || Top() > other.Bottom() )
		return CT_Disjoint;

	if( Left() <= other.Left() && Right() >= other.Right() && 
		Top() <= other.Top() &&  Bottom() >= other.Bottom() )
		return CT_Contains;

	return CT_Intersects;
}

template<typename Real>
ContainmentType Rectangle<Real>::Contains( Real x, Real y, Real width, Real height ) const
{
	if (Right() < x || Left() > (x + width))
		return CT_Disjoint;	

	if( Bottom() < y || Top() > (y+height) )
		return CT_Disjoint;

	if( Left() <= x && Right() >= (x + width) && 
		Top() <= y &&  Bottom() >= (y+height) )
		return CT_Contains;

	return CT_Intersects;
}


template<typename Real>
bool Rectangle<Real>::Contains( Real x, Real y ) const
{
	return (x >= Left() && x <= Right() && y >= Top() && y <= Bottom());
}


template<typename Real>
Real Rectangle<Real>::Bottom() const
{
	return Y + Height;
}

template<typename Real>
Real Rectangle<Real>::Right() const
{
	return X + Width;
}

template<typename Real>
Real Rectangle<Real>::Top() const
{
	return Y;
}

template<typename Real>
Real RcEngine::Rectangle<Real>::Left() const
{
	return X;
}
