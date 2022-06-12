class Controls {
public:
	enum class ControlType {
		AI,
		DUMMY,
		HUMAN,
	};
	bool forward;
	bool left;
	bool right;
	bool reverse;
	ControlType type;
	Controls( ControlType type)
		: forward(false)
		, left(false)
		, right(false)
		, reverse(false)
		, type(type)
	{}

	void update()
	{
		switch (type) {
		case ControlType::HUMAN:
			forward = wininfo.keystate[K_UP];
			left = wininfo.keystate[K_LEFT];
			right = wininfo.keystate[K_RIGHT];
			reverse = wininfo.keystate[K_DOWN];
			break;
		case ControlType::DUMMY:
			forward = true;
			break;
		}
	}

};
