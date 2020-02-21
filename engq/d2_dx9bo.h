
class model3b : public model3 {
public:
	model3b(string namea) : model3(namea)/*,m_pVertexShader(0),m_pVertexDeclaration(0)*/ {}
//	~model3b();
	void showline() // shows info about this instance when showref is called, and add this
	{
		logger("model3b ");
		modelb::showline();
	} 
	void close();
//	clipresult draw(mat4* o2w,float dissolve,pointf3* color);
//	void createshader();
//	void destroyshader();
};


