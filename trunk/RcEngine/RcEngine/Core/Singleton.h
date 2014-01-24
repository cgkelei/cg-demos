#ifndef _Singleton__H
#define _Singleton__H

namespace RcEngine{


#define SINGLETON_DECL_HEADER(T) static T& GetSingleton( void ); static T* GetSingletonPtr( void );

#define SINGLETON_DECL(T) template<> T* Singleton<T>::ms_Singleton = 0;						 \
	T& T::GetSingleton( void ) { assert( ms_Singleton );  return ( *ms_Singleton );  }		 \
	T* T::GetSingletonPtr( void ) { return ms_Singleton; }  


template <typename T> 
class Singleton
{
public:
	static void Initialize()
	{
		new T();
	}

	static void Finalize()
	{
		delete ms_Singleton;
	}

	static T& GetSingleton( void )
	{	
		assert( ms_Singleton ); 
		return ( *ms_Singleton ); 
	}

	static T* GetSingletonPtr( void )
	{ 
		return ms_Singleton;
	}
	
protected:	
	Singleton( void )
	{
		assert( !ms_Singleton );
		ms_Singleton = static_cast<T*>( this );
	}

	virtual ~Singleton( void )
	{  
		assert( ms_Singleton ); 
		ms_Singleton = 0; 
	}
	
	static T* ms_Singleton;

private:
	Singleton(const Singleton<T> &);
	Singleton& operator=(const Singleton<T> &);	
};

} // Namespace RcEngine


#endif