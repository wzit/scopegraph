#ifndef SCOPEGRAPH_SCOPE_H__
#define SCOPEGRAPH_SCOPE_H__

#include <set>
#include "Agent.h"
#include "PassesDown.h"
#include "PassesUp.h"
#include "FiltersDown.h"
#include "FiltersUp.h"
#include "ProvidesInner.h"
#include "AcceptsInner.h"
#include "detail/PassesDownImpl.h"
#include "detail/PassesUpImpl.h"
#include "detail/FiltersDownImpl.h"
#include "detail/FiltersUpImpl.h"
#include "detail/ProvidesInnerImpl.h"
#include "detail/AcceptsInnerImpl.h"
#include "detail/Spy.h"
#include "detail/ParamDefault.h"

namespace sg {

/**
 * Signals, that each scope accepts from its own agents (therefore "inner" 
 * signals).  The scope implements the callback itself (therefore "internal").
 */
typedef AcceptsInner<AddAgent> ScopeInternalInnerAccepts;

template <typename Derived, typename ... Params>
class Scope :
	public detail::PassesDownImpl<
			Derived,
			typename detail::ParamDefault<PassesDown<Signal>,Params...>::Value
	>,
	public detail::PassesUpImpl<
			Derived,
			typename detail::ParamDefault<PassesUp<>,Params...>::Value
	>,
	public detail::FiltersDownImpl<
			Derived,
			typename detail::ParamDefault<FiltersDown<>,Params...>::Value
	>,
	public detail::FiltersUpImpl<
			Derived,
			typename detail::ParamDefault<FiltersUp<>,Params...>::Value
	>,
	public detail::ProvidesInnerImpl<
			Derived,
			typename detail::ParamDefault<ProvidesInner<>,Params...>::Value
	>,
	// user defined inner signals
	public detail::AcceptsInnerImpl<
			Derived,
			typename detail::ParamDefault<AcceptsInner<>,Params...>::Value
	>,
	// internal inner signals
	public detail::AcceptsInnerImpl<
			Scope<Derived, Params...>,
			ScopeInternalInnerAccepts
	>,
	public Agent<
		Derived,
		typename detail::ParamDefault<Provides<>,Params...>::Value,
		typename detail::ParamDefault<Accepts<>,Params...>::Value
	>
	{

private:

	typedef detail::PassesDownImpl<
			Derived,
			typename detail::ParamDefault<PassesDown<Signal>,Params...>::Value
	> PassesDownType;

	typedef detail::PassesUpImpl<
			Derived,
			typename detail::ParamDefault<PassesUp<>,Params...>::Value
	> PassesUpType;

	typedef detail::FiltersDownImpl<
			Derived,
			typename detail::ParamDefault<FiltersDown<>,Params...>::Value
	> FiltersDownType;

	typedef detail::FiltersUpImpl<
			Derived,
			typename detail::ParamDefault<FiltersUp<>,Params...>::Value
	> FiltersUpType;

	typedef detail::ProvidesInnerImpl<
			Derived,
			typename detail::ParamDefault<ProvidesInner<>,Params...>::Value
	> ProvidesInnerType;

	typedef detail::AcceptsInnerImpl<
			Derived,
			typename detail::ParamDefault<AcceptsInner<>,Params...>::Value
	> AcceptsInnerType;

	typedef detail::AcceptsInnerImpl<
			Scope<Derived, Params...>,
			ScopeInternalInnerAccepts
	> InternalAcceptsInnerType;

	typedef Agent<
		Derived,
		typename detail::ParamDefault<Provides<>,Params...>::Value,
		typename detail::ParamDefault<Accepts<>,Params...>::Value
	> AgentType;

	typedef detail::Spy SpyType;

public:

	Scope() {

		PassesDownType::init(*this);
		PassesUpType::init(*this);
		FiltersDownType::init(*this);
		FiltersUpType::init(*this);
		ProvidesInnerType::init(_spy);
		AcceptsInnerType::init(_spy);
		InternalAcceptsInnerType::init(_spy);
	}

	using AgentType::send;

	/**
	 * Add an agent to this scope. Returns true, if the agent was not added 
	 * before already.
	 */
	bool add(std::shared_ptr<detail::AgentBase> agent) {

		if (_agents.count(agent))
			return false;

		connect(*agent);
		_agents.insert(agent);

		agent->introduceAs(agent);

		return true;
	}

	/**
	 * Remove an agent from this scope. Returns false, if the agent was not part 
	 * of this scope.
	 */
	bool remove(std::shared_ptr<detail::AgentBase> agent) {

		auto iterator = std::find(_agents.begin(), _agents.end(), agent);

		if (iterator == _agents.end())
			return false;

		disconnect(*agent);
		_agents.erase(iterator);

		return true;
	}

	/**
	 * Get the number of agents in this scope.
	 */
	std::size_t size() { return _agents.size(); }

	/**
	 * Get the spy of this scope. This is an agent that lives in this scope and 
	 * provides communication with the outside world, e.g., the parent scope.
	 */
	SpyType& getSpy() {

		return _spy;
	}

	/**
	 * Internal signal callback to add an agent.
	 */
	void onInnerSignal(AddAgent& signal) {

		add(signal.getAgent());
	}

protected:

	using ProvidesInnerType::sendInner;

	template <typename SignalType, typename ... Args>
	void sendInner(Args ... args) {

		SignalType signal(args...);
		sendInner(signal);
	}

private:

	// connect an agent to all agents contained in this scope and the spy
	void connect(detail::AgentBase& agent) {

		for (auto const& other : _agents)
			other->connect(agent);

		_spy.connect(agent);
	}

	// disconnect an agent from all agents contained in this scope and the spy
	void disconnect(detail::AgentBase& agent) {

		for (auto const& other : _agents)
			other->disconnect(agent);

		_spy.disconnect(agent);
	}

	std::set<std::shared_ptr<detail::AgentBase> > _agents;

	SpyType _spy;
};

} // namespace sg

#endif // SCOPEGRAPH_SCOPE_H__

