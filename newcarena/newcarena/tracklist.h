class tracklist {
public:
	vector<string> track_names;
	vector<hashi> track_hashs;
	tracklist();
	string hastrackhash(const hashi& h) const;
	U32 size() const { return track_names.size(); }
	string uniquetrackname(string trkname) const;
};
