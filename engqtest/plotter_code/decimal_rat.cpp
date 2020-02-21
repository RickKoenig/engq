// included with u_s_plotter.cpp
// number theory, display ratios for fractions, run it to see it
#define RAT
#ifdef RAT
void dorat()
{
	logger("check ratio's of fractions for terminating or repeating decimals\n");
	S32 i,j,k=0;
	const S32 maxc = 18;
	for (i=2;i<30;++i) {
		// check for multiples of 2 and 5
		S32 m = i;
		bool had2or5 = false;
/*		while(true) {
			if (m%2 == 0) {
				m /= 2;
				had2or5 = true;
				continue;
			}
			if (m%5 == 0) {
				m /= 5;
				had2or5 = true;
				continue;
			}
			break;
		} */
		had2or5 = i%2 == 0 || i%5 == 0;
		if (had2or5/* && m!=1*/) {
			logger("1/%d = skipping\n",i);
			continue;
		}
		U64 ten = 1;
		for (j=0;j<maxc;++j) {
			ten = 10*ten;
			if (ten%i == 0) {
				U64 q = ten / i;
				logger("1/%d = %llu/%llu\n",i,q,ten);
				break;
			}
		}
		if (j == maxc) {
			U64 nine = 0;
			for (k=0;k<maxc;++k) {
				nine = 10*nine + 9;
				if (nine%i == 0) {
					U64 q = nine / i;
					logger("1/%d = %llu/%llu\n",i,q,nine);
					break;
				}
			}
			if (k == maxc)
				logger("1/%d = ?\n",i);
		}
	}
}
#endif
