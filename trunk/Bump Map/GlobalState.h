#ifndef _GlobalState__H
#define _GlobalState__H

class GlobalState
{
public:
	// supported global states
	enum StateType
	{
		ALPHA,
		CULL,
		MATERIAL,
		POLYGONOFFSET,
		STENCIL,
		WIREFRAME,
		ZBUFFER
	};

public: 
	virtual ~GlobalState(){};
	virtual StateType GetStateType () const = 0;

protected:
protected:
	GlobalState ();

};




#endif
