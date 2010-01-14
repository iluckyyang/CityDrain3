#ifndef PARALLELSIMULATION_H
#define PARALLELSIMULATION_H

#include <simulation.h>
#include <boost/unordered/unordered_map.hpp>

using namespace boost;

CD3_DECLARE_SIMULATION(ParallelSimulation)
public:
	ParallelSimulation();
	virtual ~ParallelSimulation();

	virtual int run(ptime time, int dt);

	void setModel(IModel *model);
private:
	void setNodeDepends() const;
	void run(Node *n, ptime time);
	std::vector<Node *> sources;
};

#endif // PARALLELSIMULATION_H
