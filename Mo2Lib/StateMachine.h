#pragma once
#include <assert.h>
#include <string>
#include <typeinfo>
#include <memory>
#include <unordered_map>
#include <vector>

template <class entity_type>
class StateMachine;

template <class entity_type>
class State
{
private:
	typedef StateMachine<entity_type>* FSM_ptr;
protected:
	FSM_ptr fsm;
public:
	virtual void Begin(entity_type*) {}
	virtual void Execute(entity_type*) {}
	virtual void End(entity_type*) {}

	std::string name;
	virtual ~State() {}
	void SetFSM(FSM_ptr _fsm) { fsm = _fsm; }
};


template <class entity_type>
class StateMachine
{
private:
	typedef std::shared_ptr<State<entity_type>> State_ptr;
	std::unordered_map<int, State_ptr> state_list;

	//char* global_state_name;
	//char* current_state_name;

	entity_type* owner;

	int current_state_index;
	int prev_state_index;
	int global_state_index;

public:

	float dt;
	StateMachine(entity_type* owner) :
		dt(0.f),
		owner(owner) {}

	void SetCurrentState(int index)
	{
		current_state_index = index;
		{
			state_list[current_state_index]->SetFSM(this);
			state_list[current_state_index]->Begin(owner);
		}
	}
	void SetPreviousState(State<entity_type>* s) { pevious_state = s; }
	//void SetGlobalState(State<entity_type>* s) { global_state = s; global_state->Begin(owner); }
	void SetOwner(entity_type* entity) { owner = entity; }
	void SetElapsedtime(float elapsed_time) { dt = elapsed_time; }

	void Update(float elapsed_time)
	{
#if 0
		if (global_state)
		{
			global_state->Execute(owner);
		}

		if (current_state)
		{
			current_state->Execute(owner);
		}
#else
		dt = elapsed_time;
		if (current_state_index >= 0)
		{
			state_list[current_state_index]->Execute(owner);
		}

#endif
	}

	void ChangeState(int state_index)
	{
		prev_state_index = current_state_index;

		state_list[current_state_index]->End(owner);

		current_state_index = state_index;

		state_list[current_state_index]->SetFSM(this);

		state_list[current_state_index]->Begin(owner);
	}


	void RevertToPreviousState()
	{
		ChangeState(previous_state);
	}

	StateMachine<entity_type>& AddState(int index, State_ptr state)
	{
		static std::map<std::string, State_ptr> cache;
		std::string s(typeid(*state.get()).name());
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		auto it = cache.find(s);
		if (it != cache.end())
		{
			state_list[index] = it->second;
			state_list[index]->name = s;
			return *this;
		}

		state_list[index] = state;
		state_list[index]->name = s;
		cache.insert(std::make_pair(s, state));

		return *this;
	}

	//State<entity_type>* GetCurrentState() { return current_state; }
	//State<entity_type>* GetPreviousState() { return previous_state; }
	//State<entity_type>* GetGlobalState() { return global_state; }
	//char* GetGlobalName() { return global_state_name; }
	const char* GetCurrentName() { return state_list[current_state_index]->name.c_str(); }

	//const char* GetName_CurrentState()const
	//{
	//	std::string s(typeid(state_list[current_state_index].get()).name());

	//	//remove the 'class ' part from the front of the string
	//	if (s.size() > 5)
	//	{
	//		s.erase(0, 6);
	//	}

	//	return s.data();
	//	//return current_state_name;
	//}

	//std::string GetName_GlobalState()const
	//{
	//	std::string s(typeid(*current_state).name());

	//	//remove the 'class ' part from the front of the string
	//	if (s.size() > 5)
	//	{
	//		s.erase(0, 6);
	//	}
	//	else
	//	{
	//		s = "InValid";
	//	}

	//	return s;
	//}
};

