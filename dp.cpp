#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;

void twosideGeo(int party, int seed, float s = 0.5) {
	unsigned char key[16];
	for (size_t i = 0; i < 16; i++)
		key[i] = seed % 256;

	PRG prg(key);

	unsigned char ir;
	prg.random_data(&ir, 1);

	float fr = (float)ir / (1 << 8);

	Float r(0.7, ALICE);
	//Float t = r - Float(0.5);
	Float res = Float(-1.0) * Float(s) * (Float(1.0)-r).ln() - Float(s)*((Float(1.0)/Float(s)).exp()+1).ln(); 
	res = res.If(r.less_equal(Float(0.5)), Float(s) * r.ln() + Float(s)*((Float(1.0)/Float(s)).exp()+1).ln() - 1);

	cout << "Res: \t"<< res.reveal<double>() << endl;
}


int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	int num = 20;
	if(argc > 3)
		num = atoi(argv[3]);
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_semi_honest(io, party);
	twosideGeo(party, time(NULL));
	cout << "and gates number:" << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
