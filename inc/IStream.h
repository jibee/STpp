#ifndef _ISTREAM_H
#define _ISTREAM_H

/** Imput Stream interface */
class IStream {
	public:
        /** Obtains the next character from the buffer
         * 
         * @return the next character or -1 if no character is available
         */
		virtual int get() = 0;
        /** Obtains whether a character is available */
		virtual bool available() = 0;
        /** Wait until a character becomes available */
		virtual void wait();
};

/** Input stream with no  actual input */
class NullIStream : public IStream {
	public:
		virtual int get() { return -1; }
		virtual bool available() { return false; }
};

IStream& operator>>(IStream& i, unsigned char &c);
IStream& operator>>(IStream& i, char &c);
IStream& operator>>(IStream& i, int &c);
IStream& operator>>(IStream& i, char* str);

//Doesn't work...
template<int n> IStream& operator>>(IStream& s, char str[n]) {
	for(int i=0; i<n; ++i) {
		while(!s.available());
		str[i] = s.get();
		if(str[i] == '\n' || str[i] == '\r')
			str[i]=0;
		if(str[i] == 0) {
			if(i==0)
				--i;
			else
				break;
		}
	}
	return s;
};

#endif /* _ISTREAM_H */
