// cyptic studios sum test order3 to order1
//#define SUMTEST
#ifdef SUMTEST
void dosumtest()
{
	logger("doing sumtest\n");
	S32 i,j,k,el;
	S32 m=20; // number of numbers
	S32 n=100000; // number of tests
	vector<S32> nums(m,0);
	for (k=0;k<n;++k) {
		// make some random numbers
		nums[0] = mt_random(9)+1;
		for (j=1;j<m;++j) {
			nums[j] = mt_random(20)-10;
		}
		// show the numbers
		stringstream ss;
		for (j=0;j<m;++j) {
			ss.width(4);
			ss << nums[j] << " ";
		}
		// do the test O3
		S32 bi = -1,bj = -1;
		S32 bestsum = -1000000;
		for (j=0;j<m;++j) {
			for (i=j;i<m;++i) {
				S32 sum = 0;
				for (el=j;el<=i;++el) {
					sum += nums[el];
				}
				if (sum > bestsum) {
					bi = i;
					bj = j;
					bestsum = sum;
				}
			}
		}
		// better way O1
		bool newpos = true;
		S32 sum2 = 0;
		S32 bi2 = 0,bj2 = 0;
		S32 bestsum2 = 0;
		i = j = 0;
		for (el=0;el<m;++el) {
			if (newpos) {
				j = el; // mark start of positive sequence
				newpos = false;
			}
			sum2 += nums[el];
			if (sum2 >= 0) {
				if (sum2 > bestsum2) {
					bi2 = el;
					bj2 = j;
					bestsum2 = sum2;
				}
			} else {
				sum2 = 0;
				newpos = true;
			}
		}
		if (bestsum != bestsum2 || bj != bj2 || bi != bi2) {
			logger("test %3d = %s ",k,ss.str().c_str());
			logger("bestsum = %3d, start = %3d, len = %3d",bestsum,bj,bi-bj+1);
			logger("bestsum2 = %3d, start2 = %3d, len2 = %3d\n",bestsum2,bj2,bi2-bj2+1);
		}
	}
}
#endif

