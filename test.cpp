#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;

Float randomf01()
{
	unsigned int seed = time(NULL);

	unsigned char key[16];
	for (size_t i = 0; i < 16; i++)
		key[i] = seed % 256;

	static PRG prg(key);

	float fa, fb;
	prg.random_data(&fa, 4);
	prg.random_data(&fb, 4);

	const unsigned int andmask = 0x3fffffff;
	const unsigned int ormask  = 0x3f800000;

	Float fandmask(*(float*)&andmask);
	Float formask(*(float*)&ormask);

	Float aa(fa, ALICE);
	Float bb(fb, BOB);

	Float random = (aa ^ bb) & fandmask;
	random = ((random & formask) ^ random ^ formask) - Float(1.0);

	cout << "Random: \t"<< random.reveal<double>() << endl;

	return random;
}

// CDF: res = -b * sign(r - 0.5) * ln(1 - 2 * |r - 0.5|)
void laplacian1(float b = 1.0)
{
	Float r = randomf01();
	Float t = r - Float(0.5);
	Float res = Float(-b) * (Float(1.0) - Float(2.0) * t.abs()).ln();
	res = res.If(t.less_equal(Float(0.0)), -res);

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

// Gamma: res = b * (ln(r1) - ln(r2))
void laplacian2(float b = 1.0)
{
	Float r1 = randomf01();
	Float r2 = randomf01();
	Float res = Float(b) * (r1 / r2).ln();

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

// Central Limit Theorem: res = X1 + ... + X12 - 6 (Xi in (0,1))
void gaussian1()
{
	Float res = randomf01();
	for (int i = 0; i < 11; i++)
		res = res + randomf01();

	res = res - Float(6.0);

	cout << "Res: \t"<< res.reveal<double>() << endl;
}

// Box–Muller method: res = sqrt(-2*lnU)cos(2*pi*V) (U, V in (0,1))
void gaussian2()
{
	Float u = randomf01();
	Float v = randomf01();

	Float res = (Float(-2) * u.ln()).sqrt() * (Float(2 * 3.14) * v).cos();
	
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
	gaussian2();

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
