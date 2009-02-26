#include "flow.h"
#include <map>
#include <calculationunit.h>
#include <cd3assert.h>

struct FlowDefinition {
	std::vector<std::string> names;
	std::map<const CalculationUnit *, std::vector<std::string> > unit_names;
	std::map<std::string, int> positions;
};

Flow::Flow() {
	f = boost::shared_ptr<FlowPriv>(new FlowPriv());
	fd = boost::shared_ptr<FlowDefinition>(new FlowDefinition());
}

Flow::Flow(const Flow &other) {
	f = other.f;
	fd = other.fd;
}

Flow::~Flow() {

}

void Flow::copy() {
	copyData();
	copyDefinition();
}

void Flow::copyData() {
	if (!f.unique()) {
		FlowPriv *old = f.get();
		f = boost::shared_ptr<FlowPriv>(new FlowPriv(*old));
	}
}

void Flow::copyDefinition() {
	if (!fd.unique()) {
		FlowDefinition *old = fd.get();
		fd = boost::shared_ptr<FlowDefinition>(new FlowDefinition(*old));
	}
}

void Flow::addUnit(const std::string &name,
				 const CalculationUnit *unit,
				 const double value){
	assert(fd->positions.find(name) == fd->positions.end(), "name already defined");
	assert(unit, "unit is null");

	copyDefinition();
	copyData();
	f->push_back(value);
	fd->names.push_back(name);
	if (fd->unit_names.find(unit) == fd->unit_names.end())
		fd->unit_names[unit] = std::vector<std::string>();
	fd->unit_names[unit].push_back(name);
	fd->positions[name] = f->size()-1;
}

void Flow::setValue(const std::string &name,
			 const double value){
	assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	copyData();
	(*f)[fd->positions[name]] = value;
}

double Flow::getValue(const std::string &name) const{
	assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	return (*f)[fd->positions[name]];
}

const std::vector<std::string> & Flow::getNames() const {
	return fd->names;
}

const std::vector<std::string> & Flow::getUnitNames(const CalculationUnit *unit) const{
	assert(unit, "null unit not allowed");
	assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	return fd->unit_names[unit];
}

void mix(Flow *f, const Flow * const *inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->setValue(qename, qe);
	//f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		//f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
		f->setValue(cname, Ci/qe);
	}
}

Flow *mix(const Flow * const *inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	Flow *f = new Flow();

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}

