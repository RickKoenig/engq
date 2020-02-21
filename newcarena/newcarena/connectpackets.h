// server sends this packet to newly connected client
class welcomej {
public:
	envj e; // includes a hash
	S32 numplayers;
	S32 yourid;
	welcomej(const json& js);
	welcomej();
	json save() const;
};

// client then sends this packet back to server
class hij {
public:
	avaj a;
	bool hastrack; // if hash same hash
	hij(const json& js);
	hij(const avaj& aa,bool ht) : a(aa),hastrack(ht) {}
	json save() const;
};

// server send back an 'trk' track packet if client doesn't havetrack 'hastrack'

// later on server sends an array of 'caroj' 's as a go packet
