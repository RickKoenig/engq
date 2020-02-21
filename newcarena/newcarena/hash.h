struct hashi {
	static const U32 NUMHASHBYTES=16; // 128 bit
	U8 data[NUMHASHBYTES];
	hashi();
	hashi(U8* data,S32 len);
	void add(U8* data,S32 len) const;
	void final();
	string dump() const;
	bool operator==(const hashi& rhs);
	bool operator!=(const hashi& rhs);
};

// add more functions, no new data
struct hashj : public hashi {
	hashj() : hashi() {}
	hashj(const hashi& h) : hashi(h) {}
	hashj(const json& js);
	json save() const;
};

