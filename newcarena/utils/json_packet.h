void writejsontoken(socker* s,S32 id,const json& js); // write out a json  to socket
json readjsontoken(const C8* tstr,S32 tstrlen); // read a json from cstr, tstrlen is just a check (security?)
