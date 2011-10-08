#pragma once

#ifndef _Singleton__H
#define _Singleton__H

#include "Prerequisites.h"
#include "HeaderPrefix.h"


#pragma warning (push)
#pragma warning (disable : 4661)

namespace Engine
{
	template <typename T> 
	class _ApiExport Singleton
	{
	private:
		Singleton(const Singleton<T> &);
		Singleton& operator=(const Singleton<T> &);

	protected:
		static T* ms_Singleton;

	public:
		Singleton( void )
		{
			assert( !ms_Singleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
			int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
			ms_Singleton = (T*)((int)this + offset);
#else
			ms_Singleton = static_cast< T* >( this );
#endif
		}

		virtual ~Singleton( void )
		{  
			assert( ms_Singleton ); 
			ms_Singleton = 0; 
		}

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

	};

#define SINGLETON_DECL_HEADER(T) static T& GetSingleton( void ); static T* GetSingletonPtr( void );
#define SINGLETON_DECL(T) template<> T* Singleton<T>::ms_Singleton = 0;						 \
	T& T::GetSingleton( void ) { assert( ms_Singleton );  return ( *ms_Singleton );  }		 \
	T* T::GetSingletonPtr( void ) { return ms_Singleton; }  

}

#pragma warning (pop)

#include "HeaderSuffix.h"
#endif