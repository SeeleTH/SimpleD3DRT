#ifndef TEMPLATEHELPER_H
#define TEMPLATEHELPER_H

#include "macrohelper.h"

#include <cstdlib>

namespace NPTemplateHelper
{
	template <typename C>
	static int ClassCategoryGenerateTypeID() {
		static UINT32 TypeID;
		return ++TypeID;
	}

	template <typename C, typename T>
	class ClassIDGenerator {
	public:
		static const int GetTypeID() {
			static const UINT32 TypeID = ClassCategoryGenerateTypeID<C>();
			return TypeID;
		};
	};

#define CLASS_ID_GEN_FUNC(__BASECLASS__,__CLASS__) \
	virtual UINT32 GetTypeID() { return ClassIDGenerator<__BASECLASS__ ,__CLASS__>::GetTypeID(); }

	class NonCopyable
	{
	private:
		NonCopyable(const NonCopyable& x);
		NonCopyable& operator=(const NonCopyable& x);
	public:
		NonCopyable() {}; // Default constructor  
	};

	template<typename T>
	class Singleton {
	public:
		static T& Instance();
		static void Destroy();

	protected:
		inline explicit Singleton() {
			N_ASSERT(Singleton::instance_ == 0);
			Singleton::instance_ = static_cast<T*>(this);
		}
		inline ~Singleton() {
			Singleton::instance_ = 0;
		}

	private:
		static T* CreateInstance();
		static void ScheduleForDestruction(void(*)());
		static void DestroyInstance(T*);

	private:
		static T* instance_;

	private:
		inline explicit Singleton(Singleton const&) {}
		inline Singleton& operator=(Singleton const&) { return *this; }
	};

	template<typename T>
	typename T& Singleton<T>::Instance() {
		if (Singleton::instance_ == 0) {
			Singleton::instance_ = CreateInstance();
			ScheduleForDestruction(Singleton::Destroy);
		}
		return *(Singleton::instance_);
	}

	template<typename T>
	void Singleton<T>::Destroy() {
		if (Singleton::instance_ != 0) {
			DestroyInstance(Singleton::instance_);
			Singleton::instance_ = 0;
		}
	}

	template<typename T>
	inline typename T* Singleton<T>::CreateInstance() {
		return new T();
	}

	template<typename T>
	inline void Singleton<T>::ScheduleForDestruction(void(*pFun)()) {
		std::atexit(pFun);
	}

	template<typename T>
	inline void Singleton<T>::DestroyInstance(T* p) {
		delete p;
	}

	template<typename T>
	typename T* Singleton<T>::instance_ = 0;

}
#endif