#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <queue>
class Bor
{
public:
	Bor()
	{
		root = new State;
		root->isInitState = true;
		reset();
	}
	void make_step(char symbol)
	{
		State* newState = make_transition(current_state, symbol);
		if (newState == nullptr && current_state == root) {
			return;
		}
		if (newState == nullptr) {
			current_state = current_state->sufRefer;
			//check_for_terms_and_print();
			make_step(symbol);
		}
		else {
			current_state = newState;
			check_for_terms_and_print();
		}
		
	}
	void reset()
	{
		stage = 1;
		current_state = root;
	}
	void configurate (std::vector<std::string> &vocab)
	{
		vocab_ = vocab;
		uint32_t IDcounter = 0;
		State* state = root;
		for (int32_t i = 0; i < vocab.size(); ++i) {
			state = root;
			for (auto j = vocab.at(i).begin(); j != vocab.at(i).end(); ++j) {
				auto search = state->transitions.find(*j);
				if (search != state->transitions.end()) {
					state = search->second;
				}
				else {
					++IDcounter;

					State* newState = new State;
					newState->ID = IDcounter;
					newState->parent = state;
					newState->parentTransSymbol = *j;
					state->transitions.insert(std::make_pair(*j, newState));
					state = newState;
				}
				if (j == vocab.at(i).end() - 1)
				{
					state->vocabID = i;
					state->isTerminate = true;
				}
			}
		}
		calculate_suf_refers();
		calculate_compressed_refers();
	}
	class State {
	public:
		State() {};
		uint32_t ID = 0;
		uint32_t vocabID = -1;
		State* parent = nullptr;
		char parentTransSymbol = '0';
		bool isTerminate = false;
		bool isInitState = false;
		std::map<char, State*> transitions;
		State* sufRefer = nullptr;
		State* compressedRefer = nullptr;
		~State()
		{
			for (auto& i : transitions) {
				delete i.second;
			}
		}
	};
	void print_bor()
	{
		std::cout << "Bor transitions:\n";
		std::queue<State*> q;
		State* state = nullptr;
		q.push(root);
		while (!q.empty()) {
			state = q.front();
			q.pop();
			for (auto& i : state->transitions) {
				std::cout << state->ID << "--->" << i.second->ID<<" "<<i.first<<'\n';
				q.push(i.second);
			}
		}
	}
	void print_suf_refers()
	{
		std::cout << "Suf refers:\n";
		std::queue<State*> q;
		State* state = nullptr;
		q.push(root);
		while (!q.empty()) {
			state = q.front();
			q.pop();
			for (auto& i : state->transitions) {
				q.push(i.second);
			}
			if (state->sufRefer != nullptr) {
				std::cout << state->ID << "--->" << state->sufRefer->ID << '\n';
			}
			else {
				std::cout << state->ID << "--->" << -1 << '\n';
			}
		}
	}
	void print_compressed_refers()
	{
		std::cout << "Compressed refers:\n";
		std::queue<State*> q;
		State* state = nullptr;
		q.push(root);
		while (!q.empty()) {
			state = q.front();
			q.pop();
			for (auto& i : state->transitions) {
				q.push(i.second);
			}
			if (state->compressedRefer != nullptr) {
				std::cout << state->ID << "--->" << state->compressedRefer->ID << '\n';
			}
			else {
				std::cout << state->ID << "--->" << -1 << '\n';
			}
		}
	}
	~Bor()
	{
		delete root;
	};	
	uint32_t stage;
	State* root;
private:
	std::vector<std::string> vocab_;
	void calculate_suf_refers()
	{
		std::queue<State*> q;
		State* state = nullptr;
		q.push(root);
		while (!q.empty()) {
			state = q.front();
			q.pop();
			if (state != root) {
				State* newSufRef = make_transition(state->parent->sufRefer, state->parentTransSymbol);
				if (newSufRef == nullptr) {
					state->sufRefer = root;
				}
				else {
					state->sufRefer = newSufRef;
				}
			}
			for (auto& i : state->transitions) {
				q.push(i.second);
			}
		}
	}
	void calculate_compressed_refers()
	{
		std::queue<State*> q;
		State* state = nullptr;
		q.push(root);
		while (!q.empty()) {
			state = q.front();
			q.pop();
			if (state != root) {
				if (state->sufRefer->isTerminate) {
					state->compressedRefer = state->sufRefer;
				}
				else {
					if (state->sufRefer == root) {
						state->compressedRefer = nullptr;
					}
					else {
						state->compressedRefer = state->sufRefer->compressedRefer;
					}
				}
			}
			for (auto& i : state->transitions) {
				q.push(i.second);
			}
		}
	}
	void check_for_terms_and_print()
	{
		if (current_state->isTerminate) {
			std::cout << stage << " " << vocab_.at(current_state->vocabID)<<'\n';
		}
		State* termState = current_state->compressedRefer;
		while (termState != nullptr) {
			std::cout << stage << " " << vocab_.at(termState->vocabID) << '\n';
			termState = termState->compressedRefer;
		}
	}
	State* make_transition(State* state, char symbol)
	{
		if (state == nullptr) {
			return nullptr;
		}
		else {
			auto search = state->transitions.find(symbol);
			if (search == state->transitions.end()) {
				return nullptr;
			}
			else {
				return search->second;
			}
		}
	}
	State* current_state;
};
class AhoCarasic
{
public:
	AhoCarasic() {};
	void configurate(std::vector<std::string>& vocab)
	{
		automaton.configurate(vocab);
	}
	void execute(std::string string)
	{
		reset();
		for (auto& i : string) {
			automaton.make_step(i);
			++automaton.stage;
		}
	}

private:
	Bor automaton;
	void reset()
	{
		automaton.reset();
	}
};
int main()
{
	AhoCarasic b;
	std::vector<std::string> vocab;
	vocab.emplace_back("baab");
	vocab.emplace_back("aa");
	vocab.emplace_back("a");
	b.configurate(vocab);
	std::string text = "ddbaab";
	b.execute(text);
}
