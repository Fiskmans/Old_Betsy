
namespace CommonUtilities
{
	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable& aCopy) = delete;
		NonCopyable& operator= (const NonCopyable& aOther) = delete;
	};
}