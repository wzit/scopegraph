#ifndef SCOPEGRAPH_SIGNALS_H__
#define SCOPEGRAPH_SIGNALS_H__

#include <memory>

namespace sg {

// forward declaration
namespace detail {
class AgentBase;
}

/**
 * Base class for all scope-graph signals.
 */
class Signal {};

/**
 * A symbol to represent no signal.
 */
class Nothing : public Signal {};

/**
 * A signal to add an agent in the current scope.
 */
class AddAgent : public Signal {

public:

	typedef Signal parent_type;

	AddAgent() {}

	AddAgent(std::shared_ptr<detail::AgentBase>&& agent) : _agent(agent) {}

	std::shared_ptr<detail::AgentBase> getAgent() { return _agent; }

private:

	std::shared_ptr<detail::AgentBase> _agent;
};

/**
 * A signal notifying about an added agent.
 */
class AgentAdded : public Signal {

public:

	typedef Signal parent_type;

	AgentAdded(std::shared_ptr<detail::AgentBase> agent) : _agent(agent) {}

	std::shared_ptr<detail::AgentBase> getAgent() { return _agent; }

private:

	std::shared_ptr<detail::AgentBase> _agent;
};

} // namespace sg

#endif // SCOPEGRAPH_SIGNALS_H__

