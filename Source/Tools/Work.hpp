#pragma once
#include <functional>
#include <tuple>


//Klass som skickas in till Threadpoolen. Hanterar godtyckligt antal argument för funktionen som sätts,
//men kan endast hantera returtyp void.

class Work
{
public:
	friend class ThreadPool;
	Work();
	~Work();

	//Sätter funktionen som ska köras i threadpoolen om den är void void
	void SetWork(std::function<void()> aWork);

	//How to use

	//Work worker;
	//worker.SetWork(std::bind(std::bind(&Game::Update, this)));
	//ThreadPool::GetInstance()->GiveWork(worker);

	//Sätter funktionen som ska köras i threadpoolen om den har argument som inte är void
	template<typename ...Args>
	constexpr void SetWork(const std::function<void(Args...)>& myWork, Args... args);

private:
	std::function<void()> myWork;


	Work(std::function<void()> aWork);

	//Interna funktioner för att kunna hantera funkitonsargument i templates
	template <typename Function, typename Tuple>
	constexpr decltype(auto) RunDelayedFunction(Function&& f, Tuple&& t);

	template <typename Function, typename Tuple, std::size_t... I>
	constexpr decltype(auto) RunDelayedFunctionInternal(Function&& f, Tuple&& t, std::index_sequence<I...>);
};

inline Work::Work()
{
}


inline Work::Work(std::function<void()> aWork)
{
	myWork = aWork;
}


template<typename ...Args>
constexpr void Work::SetWork(const std::function<void(Args...)>& aWork, Args... args)
{
	auto tpl = std::make_tuple(std::forward<Args>(args)...);
	myWork = [this, aWork, tpl]() 
	{
		RunDelayedFunction(aWork, tpl);
	};
}


inline Work::~Work()
{
}

inline void Work::SetWork(std::function<void()> aWork)
{
	myWork = aWork;
}


template <class Function, class Tuple>
constexpr decltype(auto) Work::RunDelayedFunction(Function&& f, Tuple&& t)
{
	return RunDelayedFunctionInternal(std::forward<Function>(f), std::forward<Tuple>(t),
		std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}
template <class Function, class Tuple, std::size_t... I>
constexpr decltype(auto) Work::RunDelayedFunctionInternal(Function&& f, Tuple&& t, std::index_sequence<I...>)
{
	return std::invoke(std::forward<Function>(f),
		std::get<I>(std::forward<Tuple>(t))...);
}
