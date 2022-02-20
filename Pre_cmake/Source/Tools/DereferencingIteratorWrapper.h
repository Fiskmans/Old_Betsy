#pragma once
#include <concepts>
#include <iterator>

template<std::forward_iterator BaseIterator>
class DereferencingIteratorWrapper : public BaseIterator
{
public:
	DereferencingIteratorWrapper() = default;
	DereferencingIteratorWrapper(const BaseIterator& aOther) : BaseIterator(aOther) { }
	auto& operator*() const { return *BaseIterator::operator*(); }
	auto& operator->() const { return *BaseIterator::operator->(); }
};