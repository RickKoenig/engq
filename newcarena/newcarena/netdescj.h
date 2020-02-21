class netdescj {
public:
	S32 gamenet;
	U32 serverip;
	S32 serverport;
	netdescj(const json& j);
	netdescj() : gamenet(0),serverip(0),serverport(0) {}
	json save() const;
};
