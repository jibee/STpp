#ifndef _LOG_H
#define _LOG_H
#include "OStream.h"

class Log : public OStream {
	private:
		volatile char m_log[32][128];
		int line;
		int position;
	public:
		static Log log;
		virtual Log& put(char);
		virtual Log& endl();
		void dump(OStream& o);
};

#endif /* _LOG_H */
