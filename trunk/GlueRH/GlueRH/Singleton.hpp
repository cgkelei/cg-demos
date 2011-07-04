#ifndef _Singleton__HPP
#define _Singleton__HPP
#include <cassert>

namespace GlueRH
{
	template<typename T>
	class Singleton
	{
	public:
		static T& GetSingleton() 
		 {  
			if(!mSingleton) 
				mSingleton = new T();  
			assert(mSingleton !=NULL);  
		    return *mSingleton;  
		 }

		 static T* GetSingletonPtr() 
		 {  
			if(!mSingleton) 
				mSingleton = new T();  
			assert(mSingleton !=NULL);  
		    return mSingleton;  
		 }

	protected:
		Singleton();  
	    ~Singleton();  

	private:
		Singleton( const Singleton<T>& );
		Singleton& operator= ( const Singleton<T>& );
		
		static T* mSingleton;
	};

	template <class T> T* Singleton<T>::mSingleton = 0;  
}


#endif 