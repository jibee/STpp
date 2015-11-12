#ifndef _OSTREAM_H
#define _OSTREAM_H

/** End of line character */
class Endl {};
extern Endl endl;

/** Output stream */
class OStream {
	public:
        /** Put a character in the stream
         * 
         * @param c the character to write to the stream
         */
		virtual OStream& put(char c) = 0;
        /** Puts a string into the stream
         * 
         * @param s the string to be written
         * @param l the number of bytes to be written into the stream
         * */
		virtual OStream& put(char* s, int l);
        /** Emits the end of line character into the stream
         * 
         * */
		virtual OStream& endl() = 0;
};

/** Bottom-less character stream
 * 
 * */
class NullOStream : public OStream {
	public:
		virtual OStream& put(char c) { (void)c; return *this; }
		virtual OStream& endl() { return *this; }
};

OStream& operator<<(OStream& o, char c);
OStream& operator<<(OStream& o, const char* str);
OStream& operator<<(OStream& o, int i);
OStream& operator<<(OStream& o, Endl e);
#endif /* _OSTREAM_H */
