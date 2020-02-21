#include <m_eng.h>
#include <l_misclibm.h>

#include "utils/json.h"
#include "hash.h"
#include "tracklist.h"
#include "constructor.h"

tracklist::tracklist()
{
pushandsetdirdown(getconstructeddir());
	struct trk* tk;
//	struct newtrack* ntk;
	hashi track_hash;
	scriptdir sd(0);
	U32 i,n=sd.num();
	for (i=0;i<n;++i)
		if (isfileext(sd.idx(i).c_str(),"trkoc")) { // break it with 'trzk' should be 'trk' hmm...
//		if (isfileext(sd.idx(i).c_str(),"trkoc") || isfileext(sd.idx(i).c_str(),"trk")) { // break it with 'trzk' should be 'trk' hmm...
			char trkname[50];
			mgetname(sd.idx(i).c_str(),trkname);
			track_names.push_back(trkname);
			json js(fileload_string(sd.idx(i).c_str()));
			tk = new trk(js);
//			if (tk) { // try to load old constructor (now json file)
			track_hash=hashi((U8*)tk,sizeof(*tk));//track_hashs[j]);
			delete tk;
/*			} else if (ntk=loadnewtrackscript(trkname)) {
				U8* data;
				U32 datalen;
				data=writenewtrack(ntk,&datalen);
				track_hash=gettrackhash(data,datalen);
//				memfree(data);
				delete[] data;
				freenewtrackscript(ntk);
			} else
				errorexit("unknown track type '%s'",trkname); */
			track_hashs.push_back(track_hash);
		}
popdir();
	for (i=0;i<track_names.size();++i) {
		logger_disableindent();
		logger("track %3d: is '%-20s' ",i,track_names[i].c_str());
//		U32 j;
//		for (j=0;j<hash::NUMHASHBYTES;++j)
//			logger("%02X ",track_hashs[i].data[j]);
		logger("%s\n",track_hashs[i].dump().c_str());
		logger_enableindent();
	}
}

// see if we have a file that matches hash that server sent us
string tracklist::hastrackhash(const hashi& trkhash) const
{
	U32 i,j,n=track_names.size();
	for (i=0;i<n;i++) {
		for (j=0;j<hashi::NUMHASHBYTES;j++)
			if (track_hashs[i].data[j]!=trkhash.data[j])
				break;
		if (j==hashi::NUMHASHBYTES)
			return track_names[i];
	}
	return "";
}

string tracklist::uniquetrackname(string trkname) const
{
	U32 i,n=track_names.size();
	for (i=0;i<n;i++)
		if (track_names[i]==trkname) // it's in the list
			break;
	if (i==track_names.size()) {
		return trkname;
	}
	trkname+='x';
	return uniquetrackname(trkname);
}
