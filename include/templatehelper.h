#ifndef TEMPLATEHELPER_H
#define TEMPLATEHELPER_H
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

}
#endif