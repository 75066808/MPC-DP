#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;

void test(int party, int number) 
{
    Float a((float)number, ALICE);
    Float b((float)number, BOB);

	Float res = a.ln();

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

// CDF: res = -b * sign(r - 0.5) * ln(1 - 2 * |r - 0.5|)
void method1(int party, int seed, float b = 1.0)
{
	unsigned char key[16];
	for (size_t i = 0; i < 16; i++)
		key[i] = seed % 256;

	PRG prg(key);

	unsigned char ir;
	prg.random_data(&ir, 1);

	float fr = (float)ir / (1 << 8);

	Float r(fr, ALICE);
	Float t = r - Float(0.5);
	
	Float res = Float(-b) * (Float(1.0) - Float(2.0) * t.abs()).ln();
	
	res = res.If(t.less_equal(Float(0.0)), -res);

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

// Gamma: res = b * (ln(r1) - ln(r2))
void method2(int party, int seed, int b = 1)
{
	unsigned char key[16];
	for (size_t i = 0; i < 16; i++)
		key[i] = seed % 256;

	PRG prg(key);

	unsigned char ia, ib;
	prg.random_data(&ia, 1);
	prg.random_data(&ib, 1);

	float fa = (float)ia / (1 << 8);	
	float fb = (float)ib / (1 << 8);

	Float aa(fa, ALICE);
	Float bb(fb, BOB);

	Float res = Float(b) * (aa / bb).ln();

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

int main(int argc, char** argv) 
{
	int port, party;
	parse_party_and_port(argv, &party, &port);

	int num = 20;
	if (argc > 3)
		num = atoi(argv[3]);

	NetIO * io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);

	setup_semi_honest(io, party);
	method1(party, time(NULL));

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
