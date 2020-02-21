// json stuff
// lobby point of view
class gamedescj {
public:
	netdescj n;
	envj e;
	avaj a;
	S32 nhumanplayers; // if host (or bot)
	S32 nbotplayers;
	gamedescj(const json& js);
	json save() const;
};
