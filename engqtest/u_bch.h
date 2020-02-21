//// BCH
U32 encoder15_5(U32 m);
U32 encoder18_6(U32 m);
U32 decoder15_5(U32 code,U32 erasure,S32* nerrors,S32* nerase); // nerrors == -1 decode error
U32 decoder18_6(U32 code,U32 erasure,S32* nerrors,S32* nerase);

//// Reed Solomon
// reed solomon will be stored with least significant coeff stored first (little endian)
typedef vector<U32> rs;

extern S32 gf_div_err;
rs rs_encode_msg(const rs& in, U32 nsym);
rs rs_correct_msg(const rs msg_in, U32 nsym,S32 *nerrors,S32 *nerasures); // nerrors == -1 decode error
void rs_correct_errata(rs& code,const rs& synd,const rs& pos);
rs rs_find_errors(const rs& synd,U32 nmess,S32* nerrs);
rs rs_calc_syndromes(const rs& code, U32 nsym);
