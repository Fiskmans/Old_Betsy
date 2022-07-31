
#ifndef TOOLS_DEREFERENCING_ITERATOR_WRAPPER_H
#define TOOLS_DEREFERENCING_ITERATOR_WRAPPER_H

#include <concepts>
#include <iterator>

namespace tools 
{
	template<std::forward_iterator BaseIterator>
	class DereferencingIteratorWrapper : public BaseIterator
	{
	public:
		DereferencingIteratorWrapper() = default;
		DereferencingIteratorWrapper(const BaseIterator& aOther) : BaseIterator(aOther) { }
		auto& operator*() const { return *BaseIterator::operator*(); }
		auto& operator->() const { return *BaseIterator::operator->(); }
	};
}

#endif