#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#define FSTRING_BUFFER 16  // Reduced from 31 to 16 (longest string is 8 chars + null)

// string buffer used to load string data from program memory (F() strings)
extern char fstring_buffer[FSTRING_BUFFER];

#endif