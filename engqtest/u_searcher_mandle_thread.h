namespace u_s_searcher {
/*struct task {
	string code;
	task(string v="default") : code(v) {}
	task(const C8* v) : code(string(v)) {}
	task(const task& o) : code(o.code) {} 
};*/

void addtask(string tsk);
bool waittask(string tsk);
void printstatus();
void setthreadquit();
void waitthreaddie();
void threadtest();
void threadinit();
void threadexit();
void resettask(string tsk);
 
} // end namespace u_s_searcher
