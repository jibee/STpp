#include "Log.h"

Log Log::log;

Log& Log::put(char c) {
	m_log[line][position]=c;
	position++;
	return *this;
}

Log& Log::endl() {
	m_log[line][position]=0;
	line++;
	line%=sizeof(m_log)/sizeof(m_log[0]);
	position = 0;
	return *this;
}

void Log::dump(OStream& o) {
	for(unsigned i=0; i<sizeof(m_log)/sizeof(*m_log); ++i) {
		unsigned p = (position + i+1) % sizeof(m_log)/sizeof(*m_log);
		o << (char*)m_log[p] << ::endl;
	}
}

