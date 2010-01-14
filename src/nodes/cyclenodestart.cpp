#include "cyclenodestart.h"
#include <iostream>
using namespace std;

CD3_DECLARE_NODE_NAME(CycleNodeStart)

CycleNodeStart::CycleNodeStart() {
	addOutPort(ADD_PARAMETERS(out));
	addState("state", &out);
}

CycleNodeStart::~CycleNodeStart() {

}

int CycleNodeStart::f(ptime time, int dt) {
	(void) dt;
	(void) time;
	return dt;
}
