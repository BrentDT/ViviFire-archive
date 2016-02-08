
/*----------------------------------------------------------------------
ViviFire Programming Language
Copyright 2012-2016, Brent D. Thorn

You can get the latest version at the Bay Six Software website at
http://www.b6sw.com/

To the extent possible under law, the author(s) have dedicated all 
copyright and related and neighboring rights to this software to the 
public domain worldwide. This software is distributed without any warranty. 
You should have received a copy of the CC0 Public Domain Dedication along 
with this software. 
If not, please consult the website at
http://creativecommons.org/publicdomain/zero/1.0/legalcode
----------------------------------------------------------------------*/

#include <memory.h>
#include <string.h>
#include "Scanner.h"




// string handling, wide character


wchar_t* coco_string_create(const wchar_t* value) {
	return coco_string_create(value, 0);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex) {
	int valueLen = 0;
	int len = 0;

	if (value) {
		valueLen = wcslen(value);
		len = valueLen - startIndex;
	}

	return coco_string_create(value, startIndex, len);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex, int length) {
	int len = 0;
	wchar_t* data;

	if (value) { len = length; }
	data = new wchar_t[len + 1];
	wcsncpy(data, &(value[startIndex]), len);
	data[len] = 0;

	return data;
}

wchar_t* coco_string_create_upper(const wchar_t* data) {
	if (!data) { return NULL; }

	int dataLen = 0;
	if (data) { dataLen = wcslen(data); }

	wchar_t *newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		if ((L'a' <= data[i]) && (data[i] <= L'z')) {
			newData[i] = data[i] + (L'A' - L'a');
		}
		else { newData[i] = data[i]; }
	}

	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_lower(const wchar_t* data) {
	if (!data) { return NULL; }
	int dataLen = wcslen(data);
	return coco_string_create_lower(data, 0, dataLen);
}

wchar_t* coco_string_create_lower(const wchar_t* data, int startIndex, int dataLen) {
	if (!data) { return NULL; }

	wchar_t* newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		wchar_t ch = data[startIndex + i];
		if ((L'A' <= ch) && (ch <= L'Z')) {
			newData[i] = ch - (L'A' - L'a');
		}
		else { newData[i] = ch; }
	}
	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_append(const wchar_t* data1, const wchar_t* data2) {
	wchar_t* data;
	int data1Len = 0;
	int data2Len = 0;

	if (data1) { data1Len = wcslen(data1); }
	if (data2) {data2Len = wcslen(data2); }

	data = new wchar_t[data1Len + data2Len + 1];

	if (data1) { wcscpy(data, data1); }
	if (data2) { wcscpy(data + data1Len, data2); }

	data[data1Len + data2Len] = 0;

	return data;
}

wchar_t* coco_string_create_append(const wchar_t *target, const wchar_t appendix) {
	int targetLen = coco_string_length(target);
	wchar_t* data = new wchar_t[targetLen + 2];
	wcsncpy(data, target, targetLen);
	data[targetLen] = appendix;
	data[targetLen + 1] = 0;
	return data;
}

void coco_string_delete(wchar_t* &data) {
	delete [] data;
	data = NULL;
}

int coco_string_length(const wchar_t* data) {
	if (data) { return wcslen(data); }
	return 0;
}

bool coco_string_endswith(const wchar_t* data, const wchar_t *end) {
	int dataLen = wcslen(data);
	int endLen = wcslen(end);
	return (endLen <= dataLen) && (wcscmp(data + dataLen - endLen, end) == 0);
}

int coco_string_indexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcschr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

int coco_string_lastindexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcsrchr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

void coco_string_merge(wchar_t* &target, const wchar_t* appendix) {
	if (!appendix) { return; }
	wchar_t* data = coco_string_create_append(target, appendix);
	delete [] target;
	target = data;
}

bool coco_string_equal(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp( data1, data2 ) == 0;
}

int coco_string_compareto(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp(data1, data2);
}

int coco_string_hash(const wchar_t *data) {
	int h = 0;
	if (!data) { return 0; }
	while (*data != 0) {
		h = (h * 7) ^ *data;
		++data;
	}
	if (h < 0) { h = -h; }
	return h;
}

// string handling, ascii character

wchar_t* coco_string_create(const char* value) {
	int len = 0;
	if (value) { len = strlen(value); }
	wchar_t* data = new wchar_t[len + 1];
	for (int i = 0; i < len; ++i) { data[i] = (wchar_t) value[i]; }
	data[len] = 0;
	return data;
}

char* coco_string_create_char(const wchar_t *value) {
	int len = coco_string_length(value);
	char *res = new char[len + 1];
	for (int i = 0; i < len; ++i) { res[i] = (char) value[i]; }
	res[len] = 0;
	return res;
}

void coco_string_delete(char* &data) {
	delete [] data;
	data = NULL;
}


Token::Token() {
	kind = 0;
	pos  = 0;
	col  = 0;
	line = 0;
	val  = NULL;
	next = NULL;
}

Token::~Token() {
	coco_string_delete(val);
}

Buffer::Buffer(FILE* s, bool isUserStream) {
// ensure binary read on windows
#if _MSC_VER >= 1300
	_setmode(_fileno(s), _O_BINARY);
#endif
	stream = s; this->isUserStream = isUserStream;
	if (CanSeek()) {
		fseek(s, 0, SEEK_END);
		fileLen = ftell(s);
		fseek(s, 0, SEEK_SET);
		bufLen = (fileLen < COCO_MAX_BUFFER_LENGTH) ? fileLen : COCO_MAX_BUFFER_LENGTH;
		bufStart = INT_MAX; // nothing in the buffer so far
	} else {
		fileLen = bufLen = bufStart = 0;
	}
	bufCapacity = (bufLen>0) ? bufLen : COCO_MIN_BUFFER_LENGTH;
	buf = new unsigned char[bufCapacity];	
	if (fileLen > 0) SetPos(0);          // setup  buffer to position 0 (start)
	else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
	if (bufLen == fileLen && CanSeek()) Close();
}

Buffer::Buffer(Buffer *b) {
	buf = b->buf;
	bufCapacity = b->bufCapacity;
	b->buf = NULL;
	bufStart = b->bufStart;
	bufLen = b->bufLen;
	fileLen = b->fileLen;
	bufPos = b->bufPos;
	stream = b->stream;
	b->stream = NULL;
	isUserStream = b->isUserStream;
}

Buffer::Buffer(const unsigned char* buf, int len) {
	this->buf = new unsigned char[len];
	memcpy(this->buf, buf, len*sizeof(unsigned char));
	bufStart = 0;
	bufCapacity = bufLen = len;
	fileLen = len;
	bufPos = 0;
	stream = NULL;
}

Buffer::~Buffer() {
	Close(); 
	if (buf != NULL) {
		delete [] buf;
		buf = NULL;
	}
}

void Buffer::Close() {
	if (!isUserStream && stream != NULL) {
		fclose(stream);
		stream = NULL;
	}
}

int Buffer::Read() {
	if (bufPos < bufLen) {
		return buf[bufPos++];
	} else if (GetPos() < fileLen) {
		SetPos(GetPos()); // shift buffer start to Pos
		return buf[bufPos++];
	} else if ((stream != NULL) && !CanSeek() && (ReadNextStreamChunk() > 0)) {
		return buf[bufPos++];
	} else {
		return EoF;
	}
}

int Buffer::Peek() {
	int curPos = GetPos();
	int ch = Read();
	SetPos(curPos);
	return ch;
}

// beg .. begin, zero-based, inclusive, in byte
// end .. end, zero-based, exclusive, in byte
wchar_t* Buffer::GetString(int beg, int end) {
	int len = 0;
	wchar_t *buf = new wchar_t[end - beg];
	int oldPos = GetPos();
	SetPos(beg);
	while (GetPos() < end) buf[len++] = (wchar_t) Read();
	SetPos(oldPos);
	wchar_t *res = coco_string_create(buf, 0, len);
	coco_string_delete(buf);
	return res;
}

int Buffer::GetPos() {
	return bufPos + bufStart;
}

void Buffer::SetPos(int value) {
	if ((value >= fileLen) && (stream != NULL) && !CanSeek()) {
		// Wanted position is after buffer and the stream
		// is not seek-able e.g. network or console,
		// thus we have to read the stream manually till
		// the wanted position is in sight.
		while ((value >= fileLen) && (ReadNextStreamChunk() > 0));
	}

	if ((value < 0) || (value > fileLen)) {
		wprintf(L"--- buffer out of bounds access, position: %d\n", value);
		exit(1);
	}

	if ((value >= bufStart) && (value < (bufStart + bufLen))) { // already in buffer
		bufPos = value - bufStart;
	} else if (stream != NULL) { // must be swapped in
		fseek(stream, value, SEEK_SET);
		bufLen = fread(buf, sizeof(unsigned char), bufCapacity, stream);
		bufStart = value; bufPos = 0;
	} else {
		bufPos = fileLen - bufStart; // make Pos return fileLen
	}
}

// Read the next chunk of bytes from the stream, increases the buffer
// if needed and updates the fields fileLen and bufLen.
// Returns the number of bytes read.
int Buffer::ReadNextStreamChunk() {
	int free = bufCapacity - bufLen;
	if (free == 0) {
		// in the case of a growing input stream
		// we can neither seek in the stream, nor can we
		// foresee the maximum length, thus we must adapt
		// the buffer size on demand.
		bufCapacity = bufLen * 2;
		unsigned char *newBuf = new unsigned char[bufCapacity];
		memcpy(newBuf, buf, bufLen*sizeof(unsigned char));
		delete [] buf;
		buf = newBuf;
		free = bufLen;
	}
	int read = fread(buf + bufLen, sizeof(unsigned char), free, stream);
	if (read > 0) {
		fileLen = bufLen = (bufLen + read);
		return read;
	}
	// end of stream reached
	return 0;
}

bool Buffer::CanSeek() {
	return (stream != NULL) && (ftell(stream) != -1);
}

int UTF8Buffer::Read() {
	int ch;
	do {
		ch = Buffer::Read();
		// until we find a utf8 start (0xxxxxxx or 11xxxxxx)
	} while ((ch >= 128) && ((ch & 0xC0) != 0xC0) && (ch != EoF));
	if (ch < 128 || ch == EoF) {
		// nothing to do, first 127 chars are the same in ascii and utf8
		// 0xxxxxxx or end of file character
	} else if ((ch & 0xF0) == 0xF0) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x07; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F; ch = Buffer::Read();
		int c4 = ch & 0x3F;
		ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
	} else if ((ch & 0xE0) == 0xE0) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x0F; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F;
		ch = (((c1 << 6) | c2) << 6) | c3;
	} else if ((ch & 0xC0) == 0xC0) {
		// 110xxxxx 10xxxxxx
		int c1 = ch & 0x1F; ch = Buffer::Read();
		int c2 = ch & 0x3F;
		ch = (c1 << 6) | c2;
	}
	return ch;
}

Scanner::Scanner(const unsigned char* buf, int len) {
	buffer = new Buffer(buf, len);
	Init();
}

Scanner::Scanner(const wchar_t* fileName) {
	FILE* stream;
	char *chFileName = coco_string_create_char(fileName);
	if ((stream = fopen(chFileName, "rb")) == NULL) {
		wprintf(L"--- Cannot open file %ls\n", fileName);
		exit(1);
	}
	coco_string_delete(chFileName);
	buffer = new Buffer(stream, false);
	Init();
}

Scanner::Scanner(FILE* s) {
	buffer = new Buffer(s, true);
	Init();
}

Scanner::~Scanner() {
	char* cur = (char*) firstHeap;

	while(cur != NULL) {
		cur = *(char**) (cur + COCO_HEAP_BLOCK_SIZE);
		free(firstHeap);
		firstHeap = cur;
	}
	delete [] tval;
	delete buffer;
}

void Scanner::Init() {
	EOL    = '\n';
	eofSym = 0;
	maxT = 172;
	noSym = 172;
	int i;
	for (i = 10; i <= 10; ++i) start.set(i, 1);
	for (i = 133; i <= 133; ++i) start.set(i, 1);
	for (i = 8232; i <= 8233; ++i) start.set(i, 1);
	for (i = 13; i <= 13; ++i) start.set(i, 253);
	for (i = 48; i <= 57; ++i) start.set(i, 254);
	for (i = 98; i <= 100; ++i) start.set(i, 255);
	for (i = 103; i <= 108; ++i) start.set(i, 255);
	for (i = 112; i <= 113; ++i) start.set(i, 255);
	for (i = 116; i <= 118; ++i) start.set(i, 255);
	for (i = 121; i <= 122; ++i) start.set(i, 255);
	for (i = 170; i <= 170; ++i) start.set(i, 255);
	for (i = 181; i <= 181; ++i) start.set(i, 255);
	for (i = 186; i <= 186; ++i) start.set(i, 255);
	for (i = 192; i <= 214; ++i) start.set(i, 255);
	for (i = 216; i <= 246; ++i) start.set(i, 255);
	for (i = 248; i <= 566; ++i) start.set(i, 255);
	for (i = 592; i <= 705; ++i) start.set(i, 255);
	for (i = 710; i <= 721; ++i) start.set(i, 255);
	for (i = 736; i <= 740; ++i) start.set(i, 255);
	for (i = 750; i <= 750; ++i) start.set(i, 255);
	for (i = 890; i <= 890; ++i) start.set(i, 255);
	for (i = 902; i <= 902; ++i) start.set(i, 255);
	for (i = 904; i <= 906; ++i) start.set(i, 255);
	for (i = 908; i <= 908; ++i) start.set(i, 255);
	for (i = 910; i <= 929; ++i) start.set(i, 255);
	for (i = 931; i <= 974; ++i) start.set(i, 255);
	for (i = 976; i <= 1013; ++i) start.set(i, 255);
	for (i = 1015; i <= 1019; ++i) start.set(i, 255);
	for (i = 1024; i <= 1153; ++i) start.set(i, 255);
	for (i = 1162; i <= 1230; ++i) start.set(i, 255);
	for (i = 1232; i <= 1269; ++i) start.set(i, 255);
	for (i = 1272; i <= 1273; ++i) start.set(i, 255);
	for (i = 1280; i <= 1295; ++i) start.set(i, 255);
	for (i = 1329; i <= 1366; ++i) start.set(i, 255);
	for (i = 1369; i <= 1369; ++i) start.set(i, 255);
	for (i = 1377; i <= 1415; ++i) start.set(i, 255);
	for (i = 1488; i <= 1514; ++i) start.set(i, 255);
	for (i = 1520; i <= 1522; ++i) start.set(i, 255);
	for (i = 1569; i <= 1594; ++i) start.set(i, 255);
	for (i = 1600; i <= 1610; ++i) start.set(i, 255);
	for (i = 1646; i <= 1647; ++i) start.set(i, 255);
	for (i = 1649; i <= 1747; ++i) start.set(i, 255);
	for (i = 1749; i <= 1749; ++i) start.set(i, 255);
	for (i = 1765; i <= 1766; ++i) start.set(i, 255);
	for (i = 1774; i <= 1775; ++i) start.set(i, 255);
	for (i = 1786; i <= 1788; ++i) start.set(i, 255);
	for (i = 1791; i <= 1791; ++i) start.set(i, 255);
	for (i = 1808; i <= 1808; ++i) start.set(i, 255);
	for (i = 1810; i <= 1839; ++i) start.set(i, 255);
	for (i = 1869; i <= 1871; ++i) start.set(i, 255);
	for (i = 1920; i <= 1957; ++i) start.set(i, 255);
	for (i = 1969; i <= 1969; ++i) start.set(i, 255);
	for (i = 2308; i <= 2361; ++i) start.set(i, 255);
	for (i = 2365; i <= 2365; ++i) start.set(i, 255);
	for (i = 2384; i <= 2384; ++i) start.set(i, 255);
	for (i = 2392; i <= 2401; ++i) start.set(i, 255);
	for (i = 2437; i <= 2444; ++i) start.set(i, 255);
	for (i = 2447; i <= 2448; ++i) start.set(i, 255);
	for (i = 2451; i <= 2472; ++i) start.set(i, 255);
	for (i = 2474; i <= 2480; ++i) start.set(i, 255);
	for (i = 2482; i <= 2482; ++i) start.set(i, 255);
	for (i = 2486; i <= 2489; ++i) start.set(i, 255);
	for (i = 2493; i <= 2493; ++i) start.set(i, 255);
	for (i = 2524; i <= 2525; ++i) start.set(i, 255);
	for (i = 2527; i <= 2529; ++i) start.set(i, 255);
	for (i = 2544; i <= 2545; ++i) start.set(i, 255);
	for (i = 2565; i <= 2570; ++i) start.set(i, 255);
	for (i = 2575; i <= 2576; ++i) start.set(i, 255);
	for (i = 2579; i <= 2600; ++i) start.set(i, 255);
	for (i = 2602; i <= 2608; ++i) start.set(i, 255);
	for (i = 2610; i <= 2611; ++i) start.set(i, 255);
	for (i = 2613; i <= 2614; ++i) start.set(i, 255);
	for (i = 2616; i <= 2617; ++i) start.set(i, 255);
	for (i = 2649; i <= 2652; ++i) start.set(i, 255);
	for (i = 2654; i <= 2654; ++i) start.set(i, 255);
	for (i = 2674; i <= 2676; ++i) start.set(i, 255);
	for (i = 2693; i <= 2701; ++i) start.set(i, 255);
	for (i = 2703; i <= 2705; ++i) start.set(i, 255);
	for (i = 2707; i <= 2728; ++i) start.set(i, 255);
	for (i = 2730; i <= 2736; ++i) start.set(i, 255);
	for (i = 2738; i <= 2739; ++i) start.set(i, 255);
	for (i = 2741; i <= 2745; ++i) start.set(i, 255);
	for (i = 2749; i <= 2749; ++i) start.set(i, 255);
	for (i = 2768; i <= 2768; ++i) start.set(i, 255);
	for (i = 2784; i <= 2785; ++i) start.set(i, 255);
	for (i = 2821; i <= 2828; ++i) start.set(i, 255);
	for (i = 2831; i <= 2832; ++i) start.set(i, 255);
	for (i = 2835; i <= 2856; ++i) start.set(i, 255);
	for (i = 2858; i <= 2864; ++i) start.set(i, 255);
	for (i = 2866; i <= 2867; ++i) start.set(i, 255);
	for (i = 2869; i <= 2873; ++i) start.set(i, 255);
	for (i = 2877; i <= 2877; ++i) start.set(i, 255);
	for (i = 2908; i <= 2909; ++i) start.set(i, 255);
	for (i = 2911; i <= 2913; ++i) start.set(i, 255);
	for (i = 2929; i <= 2929; ++i) start.set(i, 255);
	for (i = 2947; i <= 2947; ++i) start.set(i, 255);
	for (i = 2949; i <= 2954; ++i) start.set(i, 255);
	for (i = 2958; i <= 2960; ++i) start.set(i, 255);
	for (i = 2962; i <= 2965; ++i) start.set(i, 255);
	for (i = 2969; i <= 2970; ++i) start.set(i, 255);
	for (i = 2972; i <= 2972; ++i) start.set(i, 255);
	for (i = 2974; i <= 2975; ++i) start.set(i, 255);
	for (i = 2979; i <= 2980; ++i) start.set(i, 255);
	for (i = 2984; i <= 2986; ++i) start.set(i, 255);
	for (i = 2990; i <= 2997; ++i) start.set(i, 255);
	for (i = 2999; i <= 3001; ++i) start.set(i, 255);
	for (i = 3077; i <= 3084; ++i) start.set(i, 255);
	for (i = 3086; i <= 3088; ++i) start.set(i, 255);
	for (i = 3090; i <= 3112; ++i) start.set(i, 255);
	for (i = 3114; i <= 3123; ++i) start.set(i, 255);
	for (i = 3125; i <= 3129; ++i) start.set(i, 255);
	for (i = 3168; i <= 3169; ++i) start.set(i, 255);
	for (i = 3205; i <= 3212; ++i) start.set(i, 255);
	for (i = 3214; i <= 3216; ++i) start.set(i, 255);
	for (i = 3218; i <= 3240; ++i) start.set(i, 255);
	for (i = 3242; i <= 3251; ++i) start.set(i, 255);
	for (i = 3253; i <= 3257; ++i) start.set(i, 255);
	for (i = 3261; i <= 3261; ++i) start.set(i, 255);
	for (i = 3294; i <= 3294; ++i) start.set(i, 255);
	for (i = 3296; i <= 3297; ++i) start.set(i, 255);
	for (i = 3333; i <= 3340; ++i) start.set(i, 255);
	for (i = 3342; i <= 3344; ++i) start.set(i, 255);
	for (i = 3346; i <= 3368; ++i) start.set(i, 255);
	for (i = 3370; i <= 3385; ++i) start.set(i, 255);
	for (i = 3424; i <= 3425; ++i) start.set(i, 255);
	for (i = 3461; i <= 3478; ++i) start.set(i, 255);
	for (i = 3482; i <= 3505; ++i) start.set(i, 255);
	for (i = 3507; i <= 3515; ++i) start.set(i, 255);
	for (i = 3517; i <= 3517; ++i) start.set(i, 255);
	for (i = 3520; i <= 3526; ++i) start.set(i, 255);
	for (i = 3585; i <= 3632; ++i) start.set(i, 255);
	for (i = 3634; i <= 3635; ++i) start.set(i, 255);
	for (i = 3648; i <= 3654; ++i) start.set(i, 255);
	for (i = 3713; i <= 3714; ++i) start.set(i, 255);
	for (i = 3716; i <= 3716; ++i) start.set(i, 255);
	for (i = 3719; i <= 3720; ++i) start.set(i, 255);
	for (i = 3722; i <= 3722; ++i) start.set(i, 255);
	for (i = 3725; i <= 3725; ++i) start.set(i, 255);
	for (i = 3732; i <= 3735; ++i) start.set(i, 255);
	for (i = 3737; i <= 3743; ++i) start.set(i, 255);
	for (i = 3745; i <= 3747; ++i) start.set(i, 255);
	for (i = 3749; i <= 3749; ++i) start.set(i, 255);
	for (i = 3751; i <= 3751; ++i) start.set(i, 255);
	for (i = 3754; i <= 3755; ++i) start.set(i, 255);
	for (i = 3757; i <= 3760; ++i) start.set(i, 255);
	for (i = 3762; i <= 3763; ++i) start.set(i, 255);
	for (i = 3773; i <= 3773; ++i) start.set(i, 255);
	for (i = 3776; i <= 3780; ++i) start.set(i, 255);
	for (i = 3782; i <= 3782; ++i) start.set(i, 255);
	for (i = 3804; i <= 3805; ++i) start.set(i, 255);
	for (i = 3840; i <= 3840; ++i) start.set(i, 255);
	for (i = 3904; i <= 3911; ++i) start.set(i, 255);
	for (i = 3913; i <= 3946; ++i) start.set(i, 255);
	for (i = 3976; i <= 3979; ++i) start.set(i, 255);
	for (i = 4096; i <= 4129; ++i) start.set(i, 255);
	for (i = 4131; i <= 4135; ++i) start.set(i, 255);
	for (i = 4137; i <= 4138; ++i) start.set(i, 255);
	for (i = 4176; i <= 4181; ++i) start.set(i, 255);
	for (i = 4256; i <= 4293; ++i) start.set(i, 255);
	for (i = 4304; i <= 4344; ++i) start.set(i, 255);
	for (i = 4352; i <= 4441; ++i) start.set(i, 255);
	for (i = 4447; i <= 4514; ++i) start.set(i, 255);
	for (i = 4520; i <= 4601; ++i) start.set(i, 255);
	for (i = 4608; i <= 4614; ++i) start.set(i, 255);
	for (i = 4616; i <= 4678; ++i) start.set(i, 255);
	for (i = 4680; i <= 4680; ++i) start.set(i, 255);
	for (i = 4682; i <= 4685; ++i) start.set(i, 255);
	for (i = 4688; i <= 4694; ++i) start.set(i, 255);
	for (i = 4696; i <= 4696; ++i) start.set(i, 255);
	for (i = 4698; i <= 4701; ++i) start.set(i, 255);
	for (i = 4704; i <= 4742; ++i) start.set(i, 255);
	for (i = 4744; i <= 4744; ++i) start.set(i, 255);
	for (i = 4746; i <= 4749; ++i) start.set(i, 255);
	for (i = 4752; i <= 4782; ++i) start.set(i, 255);
	for (i = 4784; i <= 4784; ++i) start.set(i, 255);
	for (i = 4786; i <= 4789; ++i) start.set(i, 255);
	for (i = 4792; i <= 4798; ++i) start.set(i, 255);
	for (i = 4800; i <= 4800; ++i) start.set(i, 255);
	for (i = 4802; i <= 4805; ++i) start.set(i, 255);
	for (i = 4808; i <= 4814; ++i) start.set(i, 255);
	for (i = 4816; i <= 4822; ++i) start.set(i, 255);
	for (i = 4824; i <= 4846; ++i) start.set(i, 255);
	for (i = 4848; i <= 4878; ++i) start.set(i, 255);
	for (i = 4880; i <= 4880; ++i) start.set(i, 255);
	for (i = 4882; i <= 4885; ++i) start.set(i, 255);
	for (i = 4888; i <= 4894; ++i) start.set(i, 255);
	for (i = 4896; i <= 4934; ++i) start.set(i, 255);
	for (i = 4936; i <= 4954; ++i) start.set(i, 255);
	for (i = 5024; i <= 5108; ++i) start.set(i, 255);
	for (i = 5121; i <= 5740; ++i) start.set(i, 255);
	for (i = 5743; i <= 5750; ++i) start.set(i, 255);
	for (i = 5761; i <= 5786; ++i) start.set(i, 255);
	for (i = 5792; i <= 5866; ++i) start.set(i, 255);
	for (i = 5870; i <= 5872; ++i) start.set(i, 255);
	for (i = 5888; i <= 5900; ++i) start.set(i, 255);
	for (i = 5902; i <= 5905; ++i) start.set(i, 255);
	for (i = 5920; i <= 5937; ++i) start.set(i, 255);
	for (i = 5952; i <= 5969; ++i) start.set(i, 255);
	for (i = 5984; i <= 5996; ++i) start.set(i, 255);
	for (i = 5998; i <= 6000; ++i) start.set(i, 255);
	for (i = 6016; i <= 6067; ++i) start.set(i, 255);
	for (i = 6103; i <= 6103; ++i) start.set(i, 255);
	for (i = 6108; i <= 6108; ++i) start.set(i, 255);
	for (i = 6176; i <= 6263; ++i) start.set(i, 255);
	for (i = 6272; i <= 6312; ++i) start.set(i, 255);
	for (i = 6400; i <= 6428; ++i) start.set(i, 255);
	for (i = 6480; i <= 6509; ++i) start.set(i, 255);
	for (i = 7424; i <= 7531; ++i) start.set(i, 255);
	for (i = 7680; i <= 7835; ++i) start.set(i, 255);
	for (i = 7840; i <= 7929; ++i) start.set(i, 255);
	for (i = 7936; i <= 7957; ++i) start.set(i, 255);
	for (i = 7960; i <= 7965; ++i) start.set(i, 255);
	for (i = 7968; i <= 8005; ++i) start.set(i, 255);
	for (i = 8008; i <= 8013; ++i) start.set(i, 255);
	for (i = 8016; i <= 8023; ++i) start.set(i, 255);
	for (i = 8025; i <= 8025; ++i) start.set(i, 255);
	for (i = 8027; i <= 8027; ++i) start.set(i, 255);
	for (i = 8029; i <= 8029; ++i) start.set(i, 255);
	for (i = 8031; i <= 8061; ++i) start.set(i, 255);
	for (i = 8064; i <= 8116; ++i) start.set(i, 255);
	for (i = 8118; i <= 8124; ++i) start.set(i, 255);
	for (i = 8126; i <= 8126; ++i) start.set(i, 255);
	for (i = 8130; i <= 8132; ++i) start.set(i, 255);
	for (i = 8134; i <= 8140; ++i) start.set(i, 255);
	for (i = 8144; i <= 8147; ++i) start.set(i, 255);
	for (i = 8150; i <= 8155; ++i) start.set(i, 255);
	for (i = 8160; i <= 8172; ++i) start.set(i, 255);
	for (i = 8178; i <= 8180; ++i) start.set(i, 255);
	for (i = 8182; i <= 8188; ++i) start.set(i, 255);
	for (i = 8305; i <= 8305; ++i) start.set(i, 255);
	for (i = 8319; i <= 8319; ++i) start.set(i, 255);
	for (i = 8450; i <= 8450; ++i) start.set(i, 255);
	for (i = 8455; i <= 8455; ++i) start.set(i, 255);
	for (i = 8458; i <= 8467; ++i) start.set(i, 255);
	for (i = 8469; i <= 8469; ++i) start.set(i, 255);
	for (i = 8473; i <= 8477; ++i) start.set(i, 255);
	for (i = 8484; i <= 8484; ++i) start.set(i, 255);
	for (i = 8486; i <= 8486; ++i) start.set(i, 255);
	for (i = 8488; i <= 8488; ++i) start.set(i, 255);
	for (i = 8490; i <= 8493; ++i) start.set(i, 255);
	for (i = 8495; i <= 8497; ++i) start.set(i, 255);
	for (i = 8499; i <= 8505; ++i) start.set(i, 255);
	for (i = 8509; i <= 8511; ++i) start.set(i, 255);
	for (i = 8517; i <= 8521; ++i) start.set(i, 255);
	for (i = 8544; i <= 8579; ++i) start.set(i, 255);
	for (i = 12293; i <= 12295; ++i) start.set(i, 255);
	for (i = 12321; i <= 12329; ++i) start.set(i, 255);
	for (i = 12337; i <= 12341; ++i) start.set(i, 255);
	for (i = 12344; i <= 12348; ++i) start.set(i, 255);
	for (i = 12353; i <= 12438; ++i) start.set(i, 255);
	for (i = 12445; i <= 12447; ++i) start.set(i, 255);
	for (i = 12449; i <= 12538; ++i) start.set(i, 255);
	for (i = 12540; i <= 12543; ++i) start.set(i, 255);
	for (i = 12549; i <= 12588; ++i) start.set(i, 255);
	for (i = 12593; i <= 12686; ++i) start.set(i, 255);
	for (i = 12704; i <= 12727; ++i) start.set(i, 255);
	for (i = 12784; i <= 12799; ++i) start.set(i, 255);
	for (i = 13312; i <= 13312; ++i) start.set(i, 255);
	for (i = 19893; i <= 19893; ++i) start.set(i, 255);
	for (i = 19968; i <= 42124; ++i) start.set(i, 255);
	for (i = 44032; i <= 44032; ++i) start.set(i, 255);
	for (i = 55203; i <= 55203; ++i) start.set(i, 255);
	for (i = 64256; i <= 64262; ++i) start.set(i, 255);
	for (i = 64275; i <= 64279; ++i) start.set(i, 255);
	for (i = 64285; i <= 64285; ++i) start.set(i, 255);
	for (i = 64287; i <= 64296; ++i) start.set(i, 255);
	for (i = 64298; i <= 64310; ++i) start.set(i, 255);
	for (i = 64312; i <= 64316; ++i) start.set(i, 255);
	for (i = 64318; i <= 64318; ++i) start.set(i, 255);
	for (i = 64320; i <= 64321; ++i) start.set(i, 255);
	for (i = 64323; i <= 64324; ++i) start.set(i, 255);
	for (i = 64326; i <= 64433; ++i) start.set(i, 255);
	for (i = 64467; i <= 64829; ++i) start.set(i, 255);
	for (i = 64848; i <= 64911; ++i) start.set(i, 255);
	for (i = 64914; i <= 64967; ++i) start.set(i, 255);
	for (i = 65008; i <= 65019; ++i) start.set(i, 255);
	for (i = 65136; i <= 65140; ++i) start.set(i, 255);
	for (i = 65142; i <= 65276; ++i) start.set(i, 255);
	for (i = 65313; i <= 65338; ++i) start.set(i, 255);
	for (i = 65345; i <= 65370; ++i) start.set(i, 255);
	for (i = 65382; i <= 65470; ++i) start.set(i, 255);
	for (i = 65474; i <= 65479; ++i) start.set(i, 255);
	for (i = 65482; i <= 65487; ++i) start.set(i, 255);
	for (i = 65490; i <= 65495; ++i) start.set(i, 255);
	for (i = 65498; i <= 65500; ++i) start.set(i, 255);
	start.set(44, 2);
	start.set(46, 256);
	start.set(33, 4);
	start.set(40, 5);
	start.set(41, 7);
	start.set(91, 8);
	start.set(93, 9);
	start.set(123, 10);
	start.set(125, 257);
	start.set(61, 16);
	start.set(58, 17);
	start.set(38, 258);
	start.set(97, 259);
	start.set(111, 260);
	start.set(120, 261);
	start.set(115, 262);
	start.set(43, 347);
	start.set(45, 348);
	start.set(42, 349);
	start.set(47, 350);
	start.set(109, 263);
	start.set(114, 264);
	start.set(59, 31);
	start.set(64, 265);
	start.set(96, 71);
	start.set(34, 266);
	start.set(95, 267);
	start.set(36, 78);
	start.set(35, 268);
	start.set(124, 91);
	start.set(101, 269);
	start.set(110, 270);
	start.set(119, 271);
	start.set(102, 272);
	start.set(39, 252);
	start.set(60, 351);
	start.set(62, 352);
	start.set(63, 345);
	start.set(94, 346);
		start.set(Buffer::EoF, -1);
	keywords.set(L"#base", 44);
	keywords.set(L"#null", 45);
	keywords.set(L"#self", 46);
	keywords.set(L"abstract", 47);
	keywords.set(L"case", 48);
	keywords.set(L"class", 49);
	keywords.set(L"constructor", 50);
	keywords.set(L"destructor", 51);
	keywords.set(L"do", 52);
	keywords.set(L"does", 53);
	keywords.set(L"else", 54);
	keywords.set(L"elseif", 55);
	keywords.set(L"event", 76);
	keywords.set(L"function", 79);
	keywords.set(L"in", 80);
	keywords.set(L"is", 81);
	keywords.set(L"loop", 82);
	keywords.set(L"method", 83);
	keywords.set(L"property", 84);
	keywords.set(L"select", 85);
	keywords.set(L"shared", 86);
	keywords.set(L"sub", 87);
	keywords.set(L"try", 88);
	keywords.set(L"unit", 89);
	keywords.set(L"where", 90);
	keywords.set(L"while", 91);
	keywords.set(L"afterward", 94);
	keywords.set(L"base", 95);
	keywords.set(L"shl", 96);
	keywords.set(L"shr", 97);
	keywords.set(L"call", 98);
	keywords.set(L"to", 99);
	keywords.set(L"catch", 100);
	keywords.set(L"finally", 101);
	keywords.set(L"optional", 102);
	keywords.set(L"otherwise", 103);
	keywords.set(L"then", 104);
	keywords.set(L"if", 110);
	keywords.set(L"ctor", 111);
	keywords.set(L"as", 112);
	keywords.set(L"dtor", 113);
	keywords.set(L"dim", 114);
	keywords.set(L"var", 115);
	keywords.set(L"enum", 116);
	keywords.set(L"exit", 117);
	keywords.set(L"flags", 118);
	keywords.set(L"byref", 119);
	keywords.set(L"step", 120);
	keywords.set(L"goto", 121);
	keywords.set(L"library", 122);
	keywords.set(L"and", 123);
	keywords.set(L"andthen", 124);
	keywords.set(L"or", 125);
	keywords.set(L"orelse", 126);
	keywords.set(L"xor", 127);
	keywords.set(L"mod", 131);
	keywords.set(L"rem", 132);
	keywords.set(L"new", 133);
	keywords.set(L"begin", 134);
	keywords.set(L"object", 135);
	keywords.set(L"override", 136);
	keywords.set(L"boolean", 138);
	keywords.set(L"tiny", 139);
	keywords.set(L"byte", 140);
	keywords.set(L"char", 141);
	keywords.set(L"string", 142);
	keywords.set(L"short", 143);
	keywords.set(L"ushort", 144);
	keywords.set(L"integer", 145);
	keywords.set(L"uinteger", 146);
	keywords.set(L"single", 147);
	keywords.set(L"int", 148);
	keywords.set(L"uint", 149);
	keywords.set(L"long", 150);
	keywords.set(L"ulong", 151);
	keywords.set(L"date", 152);
	keywords.set(L"double", 153);
	keywords.set(L"xfp", 154);
	keywords.set(L"huge", 155);
	keywords.set(L"uhuge", 156);
	keywords.set(L"quad", 157);
	keywords.set(L"on", 158);
	keywords.set(L"get", 159);
	keywords.set(L"set", 160);
	keywords.set(L"require", 161);
	keywords.set(L"return", 162);
	keywords.set(L"let", 163);
	keywords.set(L"struct", 164);
	keywords.set(L"handles", 165);
	keywords.set(L"throw", 166);
	keywords.set(L"tol", 167);
	keywords.set(L"trait", 168);
	keywords.set(L"not", 169);
	keywords.set(L"wait", 170);
	keywords.set(L"until", 171);


	tvalLength = 128;
	tval = new wchar_t[tvalLength]; // text of current token

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	heap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	firstHeap = heap;
	heapEnd = (void**) (((char*) heap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heapTop = heap;
	if (sizeof(Token) > COCO_HEAP_BLOCK_SIZE) {
		wprintf(L"--- Too small COCO_HEAP_BLOCK_SIZE\n");
		exit(1);
	}

	pos = -1; line = 1; col = 0; charPos = -1;
	oldEols = 0;
	NextCh();
	if (ch == 0xEF) { // check optional byte order mark for UTF-8
		NextCh(); int ch1 = ch;
		NextCh(); int ch2 = ch;
		if (ch1 != 0xBB || ch2 != 0xBF) {
			wprintf(L"Illegal byte order mark at start of file");
			exit(1);
		}
		Buffer *oldBuf = buffer;
		buffer = new UTF8Buffer(buffer); col = 0; charPos = -1;
		delete oldBuf; oldBuf = NULL;
		NextCh();
	}


	pt = tokens = CreateToken(); // first token is a dummy
}

void Scanner::NextCh() {
	if (oldEols > 0) { ch = EOL; oldEols--; }
	else {
		pos = buffer->GetPos();
		// buffer reads unicode chars, if UTF8 has been detected
		ch = buffer->Read(); col++; charPos++;
		// replace isolated '\r' by '\n' in order to make
		// eol handling uniform across Windows, Unix and Mac
		if (ch == L'\r' && buffer->Peek() != L'\n') ch = EOL;
		if (ch == EOL) { line++; col = 0; }
	}
		valCh = ch;
		if ('A' <= ch && ch <= 'Z') ch = ch - 'A' + 'a'; // ch.ToLower()
}

void Scanner::AddCh() {
	if (tlen >= tvalLength) {
		tvalLength *= 2;
		wchar_t *newBuf = new wchar_t[tvalLength];
		memcpy(newBuf, tval, tlen*sizeof(wchar_t));
		delete [] tval;
		tval = newBuf;
	}
	if (ch != Buffer::EoF) {
		tval[tlen++] = valCh;
		NextCh();
	}
}


bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'*') {
		NextCh();
		for(;;) {
			if (ch == L'*') {
				NextCh();
				if (ch == L'/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == L'/') {
				NextCh();
				if (ch == L'*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}

bool Scanner::Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == 39) {
		NextCh();
		for(;;) {
			if (ch == 39) {
				NextCh();
				if (ch == L'/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == L'/') {
				NextCh();
				if (ch == 39) {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}


void Scanner::CreateHeapBlock() {
	void* newHeap;
	char* cur = (char*) firstHeap;

	while(((char*) tokens < cur) || ((char*) tokens > (cur + COCO_HEAP_BLOCK_SIZE))) {
		cur = *((char**) (cur + COCO_HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	newHeap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	*heapEnd = newHeap;
	heapEnd = (void**) (((char*) newHeap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heap = newHeap;
	heapTop = heap;
}

Token* Scanner::CreateToken() {
	Token *t;
	if (((char*) heapTop + (int) sizeof(Token)) >= (char*) heapEnd) {
		CreateHeapBlock();
	}
	t = (Token*) heapTop;
	heapTop = (void*) ((char*) heapTop + sizeof(Token));
	t->val = NULL;
	t->next = NULL;
	return t;
}

void Scanner::AppendVal(Token *t) {
	int reqMem = (tlen + 1) * sizeof(wchar_t);
	if (((char*) heapTop + reqMem) >= (char*) heapEnd) {
		if (reqMem > COCO_HEAP_BLOCK_SIZE) {
			wprintf(L"--- Too long token value\n");
			exit(1);
		}
		CreateHeapBlock();
	}
	t->val = (wchar_t*) heapTop;
	heapTop = (void*) ((char*) heapTop + reqMem);

	wcsncpy(t->val, tval, tlen);
	t->val[tlen] = L'\0';
}

Token* Scanner::NextToken() {
	while (ch == ' ' ||
			ch == 9 || ch == 160
	) NextCh();
	if ((ch == L'/' && Comment0()) || (ch == L'/' && Comment1())) return NextToken();
	int apx = 0;
	int recKind = noSym;
	int recEnd = pos;
	t = CreateToken();
	t->pos = pos; t->col = col; t->line = line; t->charPos = charPos;
	int state = start.state(ch);
	tlen = 0; AddCh();

	switch (state) {
		case -1: { t->kind = eofSym; break; } // NextCh already done
		case 0: {
			case_0:
			if (recKind != noSym) {
				tlen = recEnd - t->pos;
				SetScannerBehindT();
			}
			t->kind = recKind; break;
		} // NextCh already done
		case 1:
			case_1:
			recEnd = pos; recKind = 1;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_1;}
			else if (ch == 13) {AddCh(); goto case_273;}
			else {t->kind = 1; break;}
		case 2:
			case_2:
			recEnd = pos; recKind = 2;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_3;}
			else if (ch == 13) {AddCh(); goto case_274;}
			else if (ch == 9 || ch == L' ') {AddCh(); goto case_2;}
			else {t->kind = 2; break;}
		case 3:
			case_3:
			recEnd = pos; recKind = 2;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_3;}
			else if (ch == 13) {AddCh(); goto case_274;}
			else {t->kind = 2; break;}
		case 4:
			{t->kind = 4; break;}
		case 5:
			case_5:
			recEnd = pos; recKind = 5;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_6;}
			else if (ch == 13) {AddCh(); goto case_275;}
			else if (ch == 9 || ch == L' ') {AddCh(); goto case_5;}
			else {t->kind = 5; break;}
		case 6:
			case_6:
			recEnd = pos; recKind = 5;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_6;}
			else if (ch == 13) {AddCh(); goto case_275;}
			else {t->kind = 5; break;}
		case 7:
			{t->kind = 6; break;}
		case 8:
			{t->kind = 7; break;}
		case 9:
			{t->kind = 8; break;}
		case 10:
			case_10:
			recEnd = pos; recKind = 9;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_11;}
			else if (ch == 13) {AddCh(); goto case_276;}
			else if (ch == 9 || ch == L' ') {AddCh(); goto case_10;}
			else {t->kind = 9; break;}
		case 11:
			case_11:
			recEnd = pos; recKind = 9;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_11;}
			else if (ch == 13) {AddCh(); goto case_276;}
			else {t->kind = 9; break;}
		case 12:
			case_12:
			if (ch == 9 || ch == L' ') {AddCh(); goto case_13;}
			else if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_12;}
			else if (ch == 13) {AddCh(); goto case_278;}
			else if (ch == L'}') {apx++; AddCh(); goto case_15;}
			else {goto case_0;}
		case 13:
			case_13:
			if (ch == 9 || ch == L' ') {AddCh(); goto case_13;}
			else if (ch == L'}') {apx++; AddCh(); goto case_279;}
			else {goto case_0;}
		case 14:
			case_14:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 10; break;}
		case 15:
			case_15:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 10; break;}
		case 16:
			{t->kind = 11; break;}
		case 17:
			if (ch == L'=') {AddCh(); goto case_18;}
			else {goto case_0;}
		case 18:
			case_18:
			{t->kind = 12; break;}
		case 19:
			case_19:
			{t->kind = 13; break;}
		case 20:
			case_20:
			{t->kind = 14; break;}
		case 21:
			case_21:
			{t->kind = 15; break;}
		case 22:
			case_22:
			{t->kind = 16; break;}
		case 23:
			case_23:
			{t->kind = 17; break;}
		case 24:
			case_24:
			{t->kind = 18; break;}
		case 25:
			case_25:
			{t->kind = 19; break;}
		case 26:
			case_26:
			{t->kind = 20; break;}
		case 27:
			case_27:
			{t->kind = 21; break;}
		case 28:
			case_28:
			{t->kind = 22; break;}
		case 29:
			case_29:
			{t->kind = 23; break;}
		case 30:
			case_30:
			{t->kind = 24; break;}
		case 31:
			{t->kind = 25; break;}
		case 32:
			case_32:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 25; break;}
		case 33:
			case_33:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 25; break;}
		case 34:
			case_34:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 25; break;}
		case 35:
			case_35:
			recEnd = pos; recKind = 26;
			if (ch == 39 || (ch >= L'0' && ch <= L'1')) {AddCh(); goto case_35;}
			else if (ch == L'_') {AddCh(); goto case_36;}
			else {t->kind = 26; break;}
		case 36:
			case_36:
			if (ch == 39 || (ch >= L'0' && ch <= L'1')) {AddCh(); goto case_35;}
			else {goto case_0;}
		case 37:
			case_37:
			recEnd = pos; recKind = 27;
			if ((ch >= L'0' && ch <= L'7')) {AddCh(); goto case_37;}
			else if (ch == L'_') {AddCh(); goto case_38;}
			else {t->kind = 27; break;}
		case 38:
			case_38:
			if ((ch >= L'0' && ch <= L'7')) {AddCh(); goto case_37;}
			else {goto case_0;}
		case 39:
			case_39:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 28; break;}
		case 40:
			case_40:
			recEnd = pos; recKind = 29;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f')) {AddCh(); goto case_40;}
			else if (ch == L'_') {AddCh(); goto case_41;}
			else {t->kind = 29; break;}
		case 41:
			case_41:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f')) {AddCh(); goto case_40;}
			else {goto case_0;}
		case 42:
			case_42:
			recEnd = pos; recKind = 30;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_42;}
			else if (ch == L'.') {AddCh(); goto case_43;}
			else {t->kind = 30; break;}
		case 43:
			case_43:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_44;}
			else {goto case_0;}
		case 44:
			case_44:
			recEnd = pos; recKind = 30;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_44;}
			else {t->kind = 30; break;}
		case 45:
			case_45:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_46;}
			else {goto case_0;}
		case 46:
			case_46:
			recEnd = pos; recKind = 32;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_46;}
			else if (ch == L'e') {AddCh(); goto case_47;}
			else if (ch == L'_') {AddCh(); goto case_50;}
			else {t->kind = 32; break;}
		case 47:
			case_47:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_49;}
			else if (ch == L'+' || ch == L'-') {AddCh(); goto case_48;}
			else {goto case_0;}
		case 48:
			case_48:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_49;}
			else {goto case_0;}
		case 49:
			case_49:
			recEnd = pos; recKind = 32;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_49;}
			else {t->kind = 32; break;}
		case 50:
			case_50:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_46;}
			else {goto case_0;}
		case 51:
			case_51:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_52;}
			else {goto case_0;}
		case 52:
			case_52:
			recEnd = pos; recKind = 32;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_52;}
			else {t->kind = 32; break;}
		case 53:
			case_53:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_54;}
			else {goto case_0;}
		case 54:
			case_54:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_54;}
			else if (ch == L'.') {AddCh(); goto case_55;}
			else {goto case_0;}
		case 55:
			case_55:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_56;}
			else {goto case_0;}
		case 56:
			case_56:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_56;}
			else {t->kind = 33; break;}
		case 57:
			case_57:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_58;}
			else {goto case_0;}
		case 58:
			case_58:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_58;}
			else if (ch == L'-') {AddCh(); goto case_59;}
			else {goto case_0;}
		case 59:
			case_59:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_60;}
			else {goto case_0;}
		case 60:
			case_60:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_60;}
			else {t->kind = 33; break;}
		case 61:
			case_61:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_62;}
			else {goto case_0;}
		case 62:
			case_62:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_62;}
			else if (ch == L'/') {AddCh(); goto case_63;}
			else {goto case_0;}
		case 63:
			case_63:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_64;}
			else {goto case_0;}
		case 64:
			case_64:
			recEnd = pos; recKind = 33;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_64;}
			else {t->kind = 33; break;}
		case 65:
			case_65:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_66;}
			else {goto case_0;}
		case 66:
			case_66:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_66;}
			else if (ch == L':') {AddCh(); goto case_67;}
			else {goto case_0;}
		case 67:
			case_67:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_68;}
			else {goto case_0;}
		case 68:
			case_68:
			recEnd = pos; recKind = 34;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_68;}
			else if (ch == L'.') {AddCh(); goto case_69;}
			else {t->kind = 34; break;}
		case 69:
			case_69:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_70;}
			else {goto case_0;}
		case 70:
			case_70:
			recEnd = pos; recKind = 34;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_70;}
			else {t->kind = 34; break;}
		case 71:
			if ((ch >= L' ' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_72;}
			else {goto case_0;}
		case 72:
			case_72:
			recEnd = pos; recKind = 35;
			if ((ch >= L'a' && ch <= L'z')) {AddCh(); goto case_73;}
			else if (ch == L'`') {AddCh(); goto case_74;}
			else {t->kind = 35; break;}
		case 73:
			case_73:
			recEnd = pos; recKind = 35;
			if ((ch >= L'a' && ch <= L'z')) {AddCh(); goto case_73;}
			else {t->kind = 35; break;}
		case 74:
			case_74:
			{t->kind = 35; break;}
		case 75:
			case_75:
			recEnd = pos; recKind = 37;
			if (ch == L'"') {apx = 0; AddCh(); goto case_280;}
			else {
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 37; break;}
		case 76:
			case_76:
			recEnd = pos; recKind = 37;
			if ((ch >= L' ' && ch <= L'!') || (ch >= L'#' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_76;}
			else if (ch == L'"') {AddCh(); goto case_280;}
			else {t->kind = 37; break;}
		case 77:
			case_77:
			recEnd = pos; recKind = 37;
			if ((ch >= L' ' && ch <= L'!') || (ch >= L'#' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_77;}
			else if (ch == L'"') {AddCh(); goto case_280;}
			else {t->kind = 37; break;}
		case 78:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_79;}
			else {goto case_0;}
		case 79:
			case_79:
			recEnd = pos; recKind = 38;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_79;}
			else if (ch == L'_') {AddCh(); goto case_80;}
			else {t->kind = 38; break;}
		case 80:
			case_80:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z')) {AddCh(); goto case_79;}
			else {goto case_0;}
		case 81:
			case_81:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 82:
			case_82:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 83:
			case_83:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 84:
			case_84:
			recEnd = pos; recKind = 40;
			if ((ch >= L'0' && ch <= L'9') || ch == L's' || ch == L'u') {AddCh(); goto case_85;}
			else {t->kind = 40; break;}
		case 85:
			case_85:
			recEnd = pos; recKind = 40;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_85;}
			else {t->kind = 40; break;}
		case 86:
			case_86:
			{t->kind = 40; break;}
		case 87:
			case_87:
			recEnd = pos; recKind = 42;
			if ((ch >= L'0' && ch <= L'9') || ch == L's' || ch == L'u') {AddCh(); goto case_88;}
			else {t->kind = 42; break;}
		case 88:
			case_88:
			recEnd = pos; recKind = 42;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_88;}
			else {t->kind = 42; break;}
		case 89:
			case_89:
			recEnd = pos; recKind = 42;
			if ((ch >= L'1' && ch <= L'2') || ch == L'4') {AddCh(); goto case_90;}
			else {t->kind = 42; break;}
		case 90:
			case_90:
			{t->kind = 42; break;}
		case 91:
			case_91:
			recEnd = pos; recKind = 43;
			if (ch == L'-') {AddCh(); goto case_91;}
			else {t->kind = 43; break;}
		case 92:
			case_92:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 56; break;}
		case 93:
			case_93:
			if (ch == L'a') {AddCh(); goto case_94;}
			else {goto case_0;}
		case 94:
			case_94:
			if (ch == L's') {AddCh(); goto case_95;}
			else {goto case_0;}
		case 95:
			case_95:
			if (ch == L's') {AddCh(); goto case_96;}
			else {goto case_0;}
		case 96:
			case_96:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_97;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_96;}
			else {goto case_0;}
		case 97:
			case_97:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 58; break;}
		case 98:
			case_98:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_110;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_98;}
			else {goto case_0;}
		case 99:
			case_99:
			if (ch == L'o') {AddCh(); goto case_100;}
			else {goto case_0;}
		case 100:
			case_100:
			if (ch == L'r') {AddCh(); goto case_98;}
			else {goto case_0;}
		case 101:
			case_101:
			if (ch == L'n') {AddCh(); goto case_102;}
			else {goto case_0;}
		case 102:
			case_102:
			if (ch == L's') {AddCh(); goto case_103;}
			else {goto case_0;}
		case 103:
			case_103:
			if (ch == L't') {AddCh(); goto case_104;}
			else {goto case_0;}
		case 104:
			case_104:
			if (ch == L'r') {AddCh(); goto case_105;}
			else {goto case_0;}
		case 105:
			case_105:
			if (ch == L'u') {AddCh(); goto case_106;}
			else {goto case_0;}
		case 106:
			case_106:
			if (ch == L'c') {AddCh(); goto case_107;}
			else {goto case_0;}
		case 107:
			case_107:
			if (ch == L't') {AddCh(); goto case_108;}
			else {goto case_0;}
		case 108:
			case_108:
			if (ch == L'o') {AddCh(); goto case_109;}
			else {goto case_0;}
		case 109:
			case_109:
			if (ch == L'r') {AddCh(); goto case_98;}
			else {goto case_0;}
		case 110:
			case_110:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 59; break;}
		case 111:
			case_111:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_122;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_111;}
			else {goto case_0;}
		case 112:
			case_112:
			if (ch == L'o') {AddCh(); goto case_113;}
			else {goto case_0;}
		case 113:
			case_113:
			if (ch == L'r') {AddCh(); goto case_111;}
			else {goto case_0;}
		case 114:
			case_114:
			if (ch == L's') {AddCh(); goto case_115;}
			else {goto case_0;}
		case 115:
			case_115:
			if (ch == L't') {AddCh(); goto case_116;}
			else {goto case_0;}
		case 116:
			case_116:
			if (ch == L'r') {AddCh(); goto case_117;}
			else {goto case_0;}
		case 117:
			case_117:
			if (ch == L'u') {AddCh(); goto case_118;}
			else {goto case_0;}
		case 118:
			case_118:
			if (ch == L'c') {AddCh(); goto case_119;}
			else {goto case_0;}
		case 119:
			case_119:
			if (ch == L't') {AddCh(); goto case_120;}
			else {goto case_0;}
		case 120:
			case_120:
			if (ch == L'o') {AddCh(); goto case_121;}
			else {goto case_0;}
		case 121:
			case_121:
			if (ch == L'r') {AddCh(); goto case_111;}
			else {goto case_0;}
		case 122:
			case_122:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 60; break;}
		case 123:
			case_123:
			if (ch == L'n') {AddCh(); goto case_124;}
			else {goto case_0;}
		case 124:
			case_124:
			if (ch == L'u') {AddCh(); goto case_125;}
			else {goto case_0;}
		case 125:
			case_125:
			if (ch == L'm') {AddCh(); goto case_126;}
			else {goto case_0;}
		case 126:
			case_126:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_127;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_126;}
			else {goto case_0;}
		case 127:
			case_127:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 61; break;}
		case 128:
			case_128:
			if (ch == L'a') {AddCh(); goto case_129;}
			else {goto case_0;}
		case 129:
			case_129:
			if (ch == L'g') {AddCh(); goto case_130;}
			else {goto case_0;}
		case 130:
			case_130:
			if (ch == L's') {AddCh(); goto case_131;}
			else {goto case_0;}
		case 131:
			case_131:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_132;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_131;}
			else {goto case_0;}
		case 132:
			case_132:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 62; break;}
		case 133:
			case_133:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_135;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_282;}
			else {goto case_0;}
		case 134:
			case_134:
			if (ch == L'r') {AddCh(); goto case_133;}
			else {goto case_0;}
		case 135:
			case_135:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 63; break;}
		case 136:
			case_136:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 63; break;}
		case 137:
			case_137:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 63; break;}
		case 138:
			case_138:
			if (ch == L'n') {AddCh(); goto case_139;}
			else {goto case_0;}
		case 139:
			case_139:
			if (ch == L'c') {AddCh(); goto case_140;}
			else {goto case_0;}
		case 140:
			case_140:
			if (ch == L't') {AddCh(); goto case_141;}
			else {goto case_0;}
		case 141:
			case_141:
			if (ch == L'i') {AddCh(); goto case_142;}
			else {goto case_0;}
		case 142:
			case_142:
			if (ch == L'o') {AddCh(); goto case_143;}
			else {goto case_0;}
		case 143:
			case_143:
			if (ch == L'n') {AddCh(); goto case_144;}
			else {goto case_0;}
		case 144:
			case_144:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_145;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_144;}
			else {goto case_0;}
		case 145:
			case_145:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 64; break;}
		case 146:
			case_146:
			if (ch == L'f') {AddCh(); goto case_147;}
			else {goto case_0;}
		case 147:
			case_147:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_148;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_147;}
			else {goto case_0;}
		case 148:
			case_148:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 65; break;}
		case 149:
			case_149:
			if (ch == L'e') {AddCh(); goto case_150;}
			else {goto case_0;}
		case 150:
			case_150:
			if (ch == L't') {AddCh(); goto case_151;}
			else {goto case_0;}
		case 151:
			case_151:
			if (ch == L'h') {AddCh(); goto case_152;}
			else {goto case_0;}
		case 152:
			case_152:
			if (ch == L'o') {AddCh(); goto case_153;}
			else {goto case_0;}
		case 153:
			case_153:
			if (ch == L'd') {AddCh(); goto case_154;}
			else {goto case_0;}
		case 154:
			case_154:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_155;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_154;}
			else {goto case_0;}
		case 155:
			case_155:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 66; break;}
		case 156:
			case_156:
			if (ch == L'e') {AddCh(); goto case_157;}
			else {goto case_0;}
		case 157:
			case_157:
			if (ch == L'w') {AddCh(); goto case_158;}
			else {goto case_0;}
		case 158:
			case_158:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_159;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_158;}
			else {goto case_0;}
		case 159:
			case_159:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 67; break;}
		case 160:
			case_160:
			if (ch == L'b') {AddCh(); goto case_161;}
			else {goto case_0;}
		case 161:
			case_161:
			if (ch == L'j') {AddCh(); goto case_162;}
			else {goto case_0;}
		case 162:
			case_162:
			if (ch == L'e') {AddCh(); goto case_163;}
			else {goto case_0;}
		case 163:
			case_163:
			if (ch == L'c') {AddCh(); goto case_164;}
			else {goto case_0;}
		case 164:
			case_164:
			if (ch == L't') {AddCh(); goto case_165;}
			else {goto case_0;}
		case 165:
			case_165:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_166;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_165;}
			else {goto case_0;}
		case 166:
			case_166:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 68; break;}
		case 167:
			case_167:
			if (ch == L'r') {AddCh(); goto case_168;}
			else {goto case_0;}
		case 168:
			case_168:
			if (ch == L'o') {AddCh(); goto case_169;}
			else {goto case_0;}
		case 169:
			case_169:
			if (ch == L'p') {AddCh(); goto case_170;}
			else {goto case_0;}
		case 170:
			case_170:
			if (ch == L'e') {AddCh(); goto case_171;}
			else {goto case_0;}
		case 171:
			case_171:
			if (ch == L'r') {AddCh(); goto case_172;}
			else {goto case_0;}
		case 172:
			case_172:
			if (ch == L't') {AddCh(); goto case_173;}
			else {goto case_0;}
		case 173:
			case_173:
			if (ch == L'y') {AddCh(); goto case_174;}
			else {goto case_0;}
		case 174:
			case_174:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_175;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_174;}
			else {goto case_0;}
		case 175:
			case_175:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 69; break;}
		case 176:
			case_176:
			if (ch == L'l') {AddCh(); goto case_177;}
			else {goto case_0;}
		case 177:
			case_177:
			if (ch == L'e') {AddCh(); goto case_178;}
			else {goto case_0;}
		case 178:
			case_178:
			if (ch == L'c') {AddCh(); goto case_179;}
			else {goto case_0;}
		case 179:
			case_179:
			if (ch == L't') {AddCh(); goto case_180;}
			else {goto case_0;}
		case 180:
			case_180:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_181;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_180;}
			else {goto case_0;}
		case 181:
			case_181:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 70; break;}
		case 182:
			case_182:
			if (ch == L'r') {AddCh(); goto case_183;}
			else {goto case_0;}
		case 183:
			case_183:
			if (ch == L'u') {AddCh(); goto case_184;}
			else {goto case_0;}
		case 184:
			case_184:
			if (ch == L'c') {AddCh(); goto case_185;}
			else {goto case_0;}
		case 185:
			case_185:
			if (ch == L't') {AddCh(); goto case_186;}
			else {goto case_0;}
		case 186:
			case_186:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_187;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_186;}
			else {goto case_0;}
		case 187:
			case_187:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 71; break;}
		case 188:
			case_188:
			if (ch == L'b') {AddCh(); goto case_189;}
			else {goto case_0;}
		case 189:
			case_189:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_190;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_189;}
			else {goto case_0;}
		case 190:
			case_190:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 72; break;}
		case 191:
			case_191:
			if (ch == L'i') {AddCh(); goto case_192;}
			else {goto case_0;}
		case 192:
			case_192:
			if (ch == L't') {AddCh(); goto case_193;}
			else {goto case_0;}
		case 193:
			case_193:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_194;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_193;}
			else {goto case_0;}
		case 194:
			case_194:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 73; break;}
		case 195:
			case_195:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_196;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_195;}
			else {goto case_0;}
		case 196:
			case_196:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 74; break;}
		case 197:
			case_197:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_202;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_197;}
			else {goto case_0;}
		case 198:
			case_198:
			if (ch == L'h') {AddCh(); goto case_199;}
			else {goto case_0;}
		case 199:
			case_199:
			if (ch == L'i') {AddCh(); goto case_200;}
			else {goto case_0;}
		case 200:
			case_200:
			if (ch == L'l') {AddCh(); goto case_201;}
			else {goto case_0;}
		case 201:
			case_201:
			if (ch == L'e') {AddCh(); goto case_197;}
			else {goto case_0;}
		case 202:
			case_202:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 75; break;}
		case 203:
			case_203:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 77; break;}
		case 204:
			case_204:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 77; break;}
		case 205:
			case_205:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 77; break;}
		case 206:
			case_206:
			if (ch == L'c') {AddCh(); goto case_207;}
			else {goto case_0;}
		case 207:
			case_207:
			if (ch == L'h') {AddCh(); goto case_208;}
			else {goto case_0;}
		case 208:
			case_208:
			{t->kind = 78; break;}
		case 209:
			case_209:
			if (ch == L'o') {AddCh(); goto case_210;}
			else {goto case_0;}
		case 210:
			case_210:
			if (ch == L'n') {AddCh(); goto case_211;}
			else {goto case_0;}
		case 211:
			case_211:
			if (ch == L's') {AddCh(); goto case_212;}
			else {goto case_0;}
		case 212:
			case_212:
			if (ch == L't') {AddCh(); goto case_213;}
			else {goto case_0;}
		case 213:
			case_213:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_214;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_213;}
			else {goto case_0;}
		case 214:
			case_214:
			recEnd = pos; recKind = 173;
			if (ch == 10) {AddCh(); goto case_215;}
			else {t->kind = 173; break;}
		case 215:
			case_215:
			{t->kind = 173; break;}
		case 216:
			case_216:
			if (ch == L'e') {AddCh(); goto case_217;}
			else {goto case_0;}
		case 217:
			case_217:
			if (ch == L'p') {AddCh(); goto case_218;}
			else {goto case_0;}
		case 218:
			case_218:
			if (ch == L'r') {AddCh(); goto case_219;}
			else {goto case_0;}
		case 219:
			case_219:
			if (ch == L'e') {AddCh(); goto case_220;}
			else {goto case_0;}
		case 220:
			case_220:
			if (ch == L'c') {AddCh(); goto case_221;}
			else {goto case_0;}
		case 221:
			case_221:
			if (ch == L'a') {AddCh(); goto case_222;}
			else {goto case_0;}
		case 222:
			case_222:
			if (ch == L't') {AddCh(); goto case_223;}
			else {goto case_0;}
		case 223:
			case_223:
			if (ch == L'e') {AddCh(); goto case_224;}
			else {goto case_0;}
		case 224:
			case_224:
			recEnd = pos; recKind = 174;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_224;}
			else {t->kind = 174; break;}
		case 225:
			case_225:
			if (ch == L'f') {AddCh(); goto case_226;}
			else {goto case_0;}
		case 226:
			case_226:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_227;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_226;}
			else {goto case_0;}
		case 227:
			case_227:
			recEnd = pos; recKind = 175;
			if (ch == 10) {AddCh(); goto case_228;}
			else {t->kind = 175; break;}
		case 228:
			case_228:
			{t->kind = 175; break;}
		case 229:
			case_229:
			if (ch == L's') {AddCh(); goto case_230;}
			else {goto case_0;}
		case 230:
			case_230:
			if (ch == L'e') {AddCh(); goto case_231;}
			else {goto case_0;}
		case 231:
			case_231:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_232;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_231;}
			else {goto case_0;}
		case 232:
			case_232:
			recEnd = pos; recKind = 176;
			if (ch == 10) {AddCh(); goto case_233;}
			else {t->kind = 176; break;}
		case 233:
			case_233:
			{t->kind = 176; break;}
		case 234:
			case_234:
			if (ch == L'd') {AddCh(); goto case_235;}
			else {goto case_0;}
		case 235:
			case_235:
			if (ch == 9 || ch == L' ') {AddCh(); goto case_235;}
			else if (ch == L'i') {AddCh(); goto case_236;}
			else {goto case_0;}
		case 236:
			case_236:
			if (ch == L'f') {AddCh(); goto case_237;}
			else {goto case_0;}
		case 237:
			case_237:
			recEnd = pos; recKind = 177;
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_238;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_237;}
			else {t->kind = 177; break;}
		case 238:
			case_238:
			recEnd = pos; recKind = 177;
			if (ch == 10) {AddCh(); goto case_239;}
			else {t->kind = 177; break;}
		case 239:
			case_239:
			{t->kind = 177; break;}
		case 240:
			case_240:
			if (ch == L'r') {AddCh(); goto case_241;}
			else {goto case_0;}
		case 241:
			case_241:
			if (ch == L'o') {AddCh(); goto case_242;}
			else {goto case_0;}
		case 242:
			case_242:
			if (ch == L'r') {AddCh(); goto case_243;}
			else {goto case_0;}
		case 243:
			case_243:
			recEnd = pos; recKind = 178;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_243;}
			else {t->kind = 178; break;}
		case 244:
			case_244:
			if (ch == L'a') {AddCh(); goto case_245;}
			else {goto case_0;}
		case 245:
			case_245:
			if (ch == L'r') {AddCh(); goto case_246;}
			else {goto case_0;}
		case 246:
			case_246:
			if (ch == L'n') {AddCh(); goto case_247;}
			else {goto case_0;}
		case 247:
			case_247:
			recEnd = pos; recKind = 179;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_247;}
			else {t->kind = 179; break;}
		case 248:
			case_248:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_249;}
			else if (ch == 9 || ch == L' ') {AddCh(); goto case_248;}
			else if (ch == 39) {AddCh(); goto case_250;}
			else {goto case_0;}
		case 249:
			case_249:
			recEnd = pos; recKind = 180;
			if (ch == 10) {AddCh(); goto case_251;}
			else {t->kind = 180; break;}
		case 250:
			case_250:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_249;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_250;}
			else {goto case_0;}
		case 251:
			case_251:
			{t->kind = 180; break;}
		case 252:
			case_252:
			recEnd = pos; recKind = 181;
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_252;}
			else {t->kind = 181; break;}
		case 253:
			recEnd = pos; recKind = 1;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_1;}
			else if (ch == 13) {AddCh(); goto case_273;}
			else {t->kind = 1; break;}
		case 254:
			case_254:
			recEnd = pos; recKind = 28;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_254;}
			else if (ch == L'_') {AddCh(); goto case_283;}
			else if (ch == L'e') {apx++; AddCh(); goto case_284;}
			else if (ch == L'.') {AddCh(); goto case_285;}
			else {t->kind = 28; break;}
		case 255:
			case_255:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 256:
			recEnd = pos; recKind = 3;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_46;}
			else {t->kind = 3; break;}
		case 257:
			recEnd = pos; recKind = 10;
			if (ch == 9 || ch == L' ') {AddCh(); goto case_277;}
			else if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_12;}
			else if (ch == 13) {AddCh(); goto case_278;}
			else if (ch == L'}') {apx++; AddCh(); goto case_14;}
			else {t->kind = 10; break;}
		case 258:
			recEnd = pos; recKind = 25;
			if (ch == L'=') {AddCh(); goto case_19;}
			else if (ch == L'b') {apx++; AddCh(); goto case_288;}
			else if (ch == L'h') {apx++; AddCh(); goto case_289;}
			else if (ch == L'o') {apx++; AddCh(); goto case_290;}
			else {t->kind = 25; break;}
		case 259:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'm') || (ch >= L'o' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'n') {AddCh(); goto case_291;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 260:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'q') || (ch >= L's' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'r') {AddCh(); goto case_292;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 261:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'n') || (ch >= L'p' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'o') {AddCh(); goto case_293;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 262:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'g') || (ch >= L'i' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'h') {AddCh(); goto case_294;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 263:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'n') || (ch >= L'p' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'o') {AddCh(); goto case_295;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 264:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'd') || (ch >= L'f' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'e') {AddCh(); goto case_296;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 265:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_297;}
			else if (ch == L'c') {AddCh(); goto case_209;}
			else if (ch == L'd') {AddCh(); goto case_216;}
			else if (ch == L'i') {AddCh(); goto case_225;}
			else if (ch == L'e') {AddCh(); goto case_298;}
			else if (ch == L'w') {AddCh(); goto case_244;}
			else {goto case_0;}
		case 266:
			case_266:
			recEnd = pos; recKind = 37;
			if ((ch >= L' ' && ch <= L'!') || (ch >= L'#' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_266;}
			else if (ch == L'"') {AddCh(); goto case_299;}
			else {t->kind = 37; break;}
		case 267:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_249;}
			else if (ch == 9 || ch == L' ') {AddCh(); goto case_248;}
			else if (ch == L'"') {AddCh(); goto case_266;}
			else if (ch == 39) {AddCh(); goto case_250;}
			else {goto case_0;}
		case 268:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_300;}
			else {goto case_0;}
		case 269:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'm') || (ch >= L'o' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'n') {AddCh(); goto case_301;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 270:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'd') || (ch >= L'f' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'e') {AddCh(); goto case_302;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 271:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'd') || (ch >= L'f' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'e') {AddCh(); goto case_303;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 272:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'n') || (ch >= L'p' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'o') {AddCh(); goto case_304;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 273:
			case_273:
			recEnd = pos; recKind = 1;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_1;}
			else if (ch == 13) {AddCh(); goto case_273;}
			else {t->kind = 1; break;}
		case 274:
			case_274:
			recEnd = pos; recKind = 2;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_3;}
			else if (ch == 13) {AddCh(); goto case_274;}
			else {t->kind = 2; break;}
		case 275:
			case_275:
			recEnd = pos; recKind = 5;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_6;}
			else if (ch == 13) {AddCh(); goto case_275;}
			else {t->kind = 5; break;}
		case 276:
			case_276:
			recEnd = pos; recKind = 9;
			if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_11;}
			else if (ch == 13) {AddCh(); goto case_276;}
			else {t->kind = 9; break;}
		case 277:
			case_277:
			if (ch == 9 || ch == L' ') {AddCh(); goto case_277;}
			else if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_12;}
			else if (ch == 13) {AddCh(); goto case_278;}
			else if (ch == L'}') {apx++; AddCh(); goto case_305;}
			else {goto case_0;}
		case 278:
			case_278:
			if (ch == 9 || ch == L' ') {AddCh(); goto case_13;}
			else if (ch == 10 || ch == 133 || (ch >= 8232 && ch <= 8233)) {AddCh(); goto case_12;}
			else if (ch == 13) {AddCh(); goto case_278;}
			else if (ch == L'}') {apx++; AddCh(); goto case_15;}
			else {goto case_0;}
		case 279:
			case_279:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 10; break;}
		case 280:
			case_280:
			if (ch == L'"') {AddCh(); goto case_76;}
			else if (ch == L'/') {AddCh(); goto case_306;}
			else if (ch == 92) {AddCh(); goto case_307;}
			else {goto case_0;}
		case 281:
			case_281:
			if (ch == L't') {AddCh(); goto case_112;}
			else if (ch == L'e') {AddCh(); goto case_114;}
			else {goto case_0;}
		case 282:
			case_282:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_136;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_282;}
			else if ((ch >= L'a' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_137;}
			else {goto case_0;}
		case 283:
			case_283:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_308;}
			else {goto case_0;}
		case 284:
			case_284:
			if ((ch >= L'a' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_39;}
			else if ((ch >= L'0' && ch <= L'9')) {apx = 0; AddCh(); goto case_52;}
			else if (ch == L'+' || ch == L'-') {apx = 0; AddCh(); goto case_51;}
			else {goto case_0;}
		case 285:
			case_285:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_309;}
			else {goto case_0;}
		case 286:
			case_286:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else {goto case_0;}
		case 287:
			case_287:
			recEnd = pos; recKind = 40;
			if ((ch >= L'a' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_81;}
			else if (ch == L'0' || ch == L'3' || (ch >= L'5' && ch <= L'9')) {apx++; AddCh(); goto case_82;}
			else if (ch == L'1') {apx++; AddCh(); goto case_310;}
			else if (ch == L'2') {apx++; AddCh(); goto case_311;}
			else if (ch == L'4') {apx++; AddCh(); goto case_312;}
			else {t->kind = 40; break;}
		case 288:
			case_288:
			if ((ch >= L'2' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_32;}
			else if (ch == 39 || (ch >= L'0' && ch <= L'1')) {apx++; AddCh(); goto case_313;}
			else {goto case_0;}
		case 289:
			case_289:
			if ((ch >= L'g' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_33;}
			else if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f')) {apx++; AddCh(); goto case_314;}
			else {goto case_0;}
		case 290:
			case_290:
			if ((ch >= L'8' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_34;}
			else if ((ch >= L'0' && ch <= L'7')) {apx++; AddCh(); goto case_315;}
			else {goto case_0;}
		case 291:
			case_291:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'c') || (ch >= L'e' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'd') {AddCh(); goto case_316;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 292:
			case_292:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_21;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 293:
			case_293:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'q') || (ch >= L's' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'r') {AddCh(); goto case_317;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 294:
			case_294:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'k') || (ch >= L'm' && ch <= L'q') || (ch >= L's' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'l') {AddCh(); goto case_318;}
			else if (ch == L'r') {AddCh(); goto case_319;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 295:
			case_295:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'c') || (ch >= L'e' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'd') {AddCh(); goto case_320;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 296:
			case_296:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'l') || (ch >= L'n' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'm') {AddCh(); goto case_321;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 297:
			case_297:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_297;}
			else if (ch == L'.') {AddCh(); goto case_53;}
			else if (ch == L'-') {AddCh(); goto case_57;}
			else if (ch == L'/') {AddCh(); goto case_61;}
			else if (ch == L':') {AddCh(); goto case_65;}
			else {goto case_0;}
		case 298:
			case_298:
			if (ch == L'l') {AddCh(); goto case_229;}
			else if (ch == L'n') {AddCh(); goto case_234;}
			else if (ch == L'r') {AddCh(); goto case_240;}
			else {goto case_0;}
		case 299:
			case_299:
			recEnd = pos; recKind = 36;
			if (ch == L'"') {AddCh(); goto case_322;}
			else if (ch == L'/') {AddCh(); goto case_323;}
			else if (ch == 92) {AddCh(); goto case_324;}
			else {t->kind = 36; break;}
		case 300:
			case_300:
			recEnd = pos; recKind = 41;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_300;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_325;}
			else if (ch == L'%') {AddCh(); goto case_87;}
			else if (ch == L'#') {AddCh(); goto case_89;}
			else if (ch == L'$') {AddCh(); goto case_90;}
			else {t->kind = 41; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 301:
			case_301:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'c') || (ch >= L'e' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'd') {AddCh(); goto case_326;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 302:
			case_302:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'w') || (ch >= L'y' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'x') {AddCh(); goto case_327;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 303:
			case_303:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'm') || (ch >= L'o' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'n') {AddCh(); goto case_328;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 304:
			case_304:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'q') || (ch >= L's' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'r') {AddCh(); goto case_329;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 305:
			case_305:
			{
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 10; break;}
		case 306:
			case_306:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_75;}
			else if (ch == L'"') {AddCh(); goto case_77;}
			else {goto case_0;}
		case 307:
			case_307:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_75;}
			else if (ch == L'"') {AddCh(); goto case_77;}
			else {goto case_0;}
		case 308:
			case_308:
			recEnd = pos; recKind = 28;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_308;}
			else if (ch == L'_') {AddCh(); goto case_283;}
			else if (ch == L'e') {apx++; AddCh(); goto case_284;}
			else if (ch == L'.') {AddCh(); goto case_45;}
			else {t->kind = 28; break;}
		case 309:
			case_309:
			recEnd = pos; recKind = 32;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_309;}
			else if (ch == L'.') {AddCh(); goto case_330;}
			else if (ch == L'e') {AddCh(); goto case_47;}
			else if (ch == L'_') {AddCh(); goto case_50;}
			else {t->kind = 32; break;}
		case 310:
			case_310:
			recEnd = pos; recKind = 40;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_83;}
			else {t->kind = 40; break;}
		case 311:
			case_311:
			recEnd = pos; recKind = 40;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_83;}
			else {t->kind = 40; break;}
		case 312:
			case_312:
			recEnd = pos; recKind = 40;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_83;}
			else {t->kind = 40; break;}
		case 313:
			case_313:
			recEnd = pos; recKind = 26;
			if ((ch >= L'2' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_32;}
			else if (ch == 39 || (ch >= L'0' && ch <= L'1')) {apx++; AddCh(); goto case_313;}
			else if (ch == L'_') {apx = 0; AddCh(); goto case_36;}
			else {t->kind = 26; break;}
		case 314:
			case_314:
			recEnd = pos; recKind = 29;
			if ((ch >= L'g' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_33;}
			else if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f')) {apx++; AddCh(); goto case_314;}
			else if (ch == L'_') {apx = 0; AddCh(); goto case_41;}
			else {t->kind = 29; break;}
		case 315:
			case_315:
			recEnd = pos; recKind = 27;
			if ((ch >= L'8' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx++; AddCh(); goto case_34;}
			else if ((ch >= L'0' && ch <= L'7')) {apx++; AddCh(); goto case_315;}
			else if (ch == L'_') {apx = 0; AddCh(); goto case_38;}
			else {t->kind = 27; break;}
		case 316:
			case_316:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_20;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 317:
			case_317:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_22;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 318:
			case_318:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_23;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 319:
			case_319:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_24;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 320:
			case_320:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_29;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 321:
			case_321:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'=') {AddCh(); goto case_30;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 322:
			case_322:
			recEnd = pos; recKind = 37;
			if ((ch >= L' ' && ch <= L'!') || (ch >= L'#' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_322;}
			else if (ch == L'"') {AddCh(); goto case_299;}
			else {t->kind = 37; break;}
		case 323:
			case_323:
			if (ch == 10 || ch == 13 || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_75;}
			else if (ch == L'"') {AddCh(); goto case_331;}
			else {goto case_0;}
		case 324:
			case_324:
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_75;}
			else if (ch == L'"') {AddCh(); goto case_331;}
			else {goto case_0;}
		case 325:
			case_325:
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_300;}
			else {goto case_0;}
		case 326:
			case_326:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx = 0; AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {apx = 0; AddCh(); goto case_286;}
			else if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_92;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_332;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {apx = 0; AddCh(); goto case_84;}
			else if (ch == L'$') {apx = 0; AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 327:
			case_327:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L's') || (ch >= L'u' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L't') {AddCh(); goto case_333;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 328:
			case_328:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'c') || (ch >= L'e' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else if (ch == L'd') {AddCh(); goto case_334;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 329:
			case_329:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_335;}
			else if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_203;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 330:
			case_330:
			recEnd = pos; recKind = 31;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_42;}
			else {t->kind = 31; break;}
		case 331:
			case_331:
			recEnd = pos; recKind = 37;
			if ((ch >= L' ' && ch <= L'!') || (ch >= L'#' && ch <= 132) || (ch >= 134 && ch <= 8231) || (ch >= 8234 && ch <= 65535)) {AddCh(); goto case_331;}
			else if (ch == L'"') {AddCh(); goto case_299;}
			else {t->kind = 37; break;}
		case 332:
			case_332:
			recEnd = pos; recKind = 57;
			if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_92;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_332;}
			else if (ch == L'c') {apx = 0; AddCh(); goto case_336;}
			else if (ch == L'd') {apx = 0; AddCh(); goto case_281;}
			else if (ch == L'e') {apx = 0; AddCh(); goto case_123;}
			else if (ch == L'f') {apx = 0; AddCh(); goto case_337;}
			else if (ch == L'i') {apx = 0; AddCh(); goto case_146;}
			else if (ch == L'm') {apx = 0; AddCh(); goto case_149;}
			else if (ch == L'n') {apx = 0; AddCh(); goto case_156;}
			else if (ch == L'o') {apx = 0; AddCh(); goto case_160;}
			else if (ch == L'p') {apx = 0; AddCh(); goto case_167;}
			else if (ch == L's') {apx = 0; AddCh(); goto case_338;}
			else if (ch == L't') {apx = 0; AddCh(); goto case_339;}
			else if (ch == L'w') {apx = 0; AddCh(); goto case_198;}
			else {t->kind = 57; break;}
		case 333:
			case_333:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {AddCh(); goto case_286;}
			else if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_135;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_282;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {AddCh(); goto case_84;}
			else if (ch == L'$') {AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 334:
			case_334:
			recEnd = pos; recKind = 39;
			if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || ch == 170 || ch == 173 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || (ch >= 768 && ch <= 855) || (ch >= 861 && ch <= 879) || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1155 && ch <= 1158) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1425 && ch <= 1441) || (ch >= 1443 && ch <= 1465) || (ch >= 1467 && ch <= 1469) || ch == 1471 || (ch >= 1473 && ch <= 1474) || ch == 1476 || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1536 && ch <= 1539) || (ch >= 1552 && ch <= 1557) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1624) || (ch >= 1632 && ch <= 1641) || (ch >= 1646 && ch <= 1747) || (ch >= 1749 && ch <= 1757) || (ch >= 1759 && ch <= 1768) || (ch >= 1770 && ch <= 1788) || ch == 1791 || (ch >= 1807 && ch <= 1866) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1969) || (ch >= 2305 && ch <= 2361) || (ch >= 2364 && ch <= 2381) || (ch >= 2384 && ch <= 2388) || (ch >= 2392 && ch <= 2403) || (ch >= 2406 && ch <= 2415) || (ch >= 2433 && ch <= 2435) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || (ch >= 2492 && ch <= 2500) || (ch >= 2503 && ch <= 2504) || (ch >= 2507 && ch <= 2509) || ch == 2519 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2531) || (ch >= 2534 && ch <= 2545) || (ch >= 2561 && ch <= 2563) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || ch == 2620 || (ch >= 2622 && ch <= 2626) || (ch >= 2631 && ch <= 2632) || (ch >= 2635 && ch <= 2637) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2662 && ch <= 2676) || (ch >= 2689 && ch <= 2691) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || (ch >= 2748 && ch <= 2757) || (ch >= 2759 && ch <= 2761) || (ch >= 2763 && ch <= 2765) || ch == 2768 || (ch >= 2784 && ch <= 2787) || (ch >= 2790 && ch <= 2799) || (ch >= 2817 && ch <= 2819) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || (ch >= 2876 && ch <= 2883) || (ch >= 2887 && ch <= 2888) || (ch >= 2891 && ch <= 2893) || (ch >= 2902 && ch <= 2903) || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || (ch >= 2918 && ch <= 2927) || ch == 2929 || (ch >= 2946 && ch <= 2947) || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3006 && ch <= 3010) || (ch >= 3014 && ch <= 3016) || (ch >= 3018 && ch <= 3021) || ch == 3031 || (ch >= 3047 && ch <= 3055) || (ch >= 3073 && ch <= 3075) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3134 && ch <= 3140) || (ch >= 3142 && ch <= 3144) || (ch >= 3146 && ch <= 3149) || (ch >= 3157 && ch <= 3158) || (ch >= 3168 && ch <= 3169) || (ch >= 3174 && ch <= 3183) || (ch >= 3202 && ch <= 3203) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || (ch >= 3260 && ch <= 3268) || (ch >= 3270 && ch <= 3272) || (ch >= 3274 && ch <= 3277) || (ch >= 3285 && ch <= 3286) || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3302 && ch <= 3311) || (ch >= 3330 && ch <= 3331) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3390 && ch <= 3395) || (ch >= 3398 && ch <= 3400) || (ch >= 3402 && ch <= 3405) || ch == 3415 || (ch >= 3424 && ch <= 3425) || (ch >= 3430 && ch <= 3439) || (ch >= 3458 && ch <= 3459) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || ch == 3530 || (ch >= 3535 && ch <= 3540) || ch == 3542 || (ch >= 3544 && ch <= 3551) || (ch >= 3570 && ch <= 3571) || (ch >= 3585 && ch <= 3642) || (ch >= 3648 && ch <= 3662) || (ch >= 3664 && ch <= 3673) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3769) || (ch >= 3771 && ch <= 3773) || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3784 && ch <= 3789) || (ch >= 3792 && ch <= 3801) || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3864 && ch <= 3865) || (ch >= 3872 && ch <= 3881) || ch == 3893 || ch == 3895 || ch == 3897 || (ch >= 3902 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3953 && ch <= 3972) || (ch >= 3974 && ch <= 3979) || (ch >= 3984 && ch <= 3991) || (ch >= 3993 && ch <= 4028) || ch == 4038 || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4140 && ch <= 4146) || (ch >= 4150 && ch <= 4153) || (ch >= 4160 && ch <= 4169) || (ch >= 4176 && ch <= 4185) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 4969 && ch <= 4977) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5908) || (ch >= 5920 && ch <= 5940) || (ch >= 5952 && ch <= 5971) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6002 && ch <= 6003) || (ch >= 6016 && ch <= 6099) || ch == 6103 || (ch >= 6108 && ch <= 6109) || (ch >= 6112 && ch <= 6121) || (ch >= 6155 && ch <= 6157) || (ch >= 6160 && ch <= 6169) || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6313) || (ch >= 6400 && ch <= 6428) || (ch >= 6432 && ch <= 6443) || (ch >= 6448 && ch <= 6459) || (ch >= 6470 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || (ch >= 8203 && ch <= 8207) || (ch >= 8234 && ch <= 8238) || (ch >= 8288 && ch <= 8291) || (ch >= 8298 && ch <= 8303) || ch == 8305 || ch == 8319 || (ch >= 8400 && ch <= 8412) || ch == 8417 || (ch >= 8421 && ch <= 8426) || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12335) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12441 && ch <= 12442) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || (ch >= 64285 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65056 && ch <= 65059) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || ch == 65279 || (ch >= 65296 && ch <= 65305) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500) || (ch >= 65529 && ch <= 65531)) {apx = 0; AddCh(); goto case_255;}
			else if (ch == L'_' || (ch >= 8255 && ch <= 8256) || ch == 8276 || ch == 12539 || (ch >= 65075 && ch <= 65076) || (ch >= 65101 && ch <= 65103) || ch == 65343 || ch == 65381) {apx = 0; AddCh(); goto case_286;}
			else if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_202;}
			else if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_197;}
			else if (ch == L'#') {apx++; AddCh(); goto case_287;}
			else if (ch == L'%') {apx = 0; AddCh(); goto case_84;}
			else if (ch == L'$') {apx = 0; AddCh(); goto case_86;}
			else {t->kind = 39; wchar_t *literal = coco_string_create_lower(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 335:
			case_335:
			if (ch == 9 || ch == L' ') {apx++; AddCh(); goto case_335;}
			else if ((ch >= L'a' && ch <= L'd') || (ch >= L'f' && ch <= L'z') || ch == 170 || ch == 181 || ch == 186 || (ch >= 192 && ch <= 214) || (ch >= 216 && ch <= 246) || (ch >= 248 && ch <= 566) || (ch >= 592 && ch <= 705) || (ch >= 710 && ch <= 721) || (ch >= 736 && ch <= 740) || ch == 750 || ch == 890 || ch == 902 || (ch >= 904 && ch <= 906) || ch == 908 || (ch >= 910 && ch <= 929) || (ch >= 931 && ch <= 974) || (ch >= 976 && ch <= 1013) || (ch >= 1015 && ch <= 1019) || (ch >= 1024 && ch <= 1153) || (ch >= 1162 && ch <= 1230) || (ch >= 1232 && ch <= 1269) || (ch >= 1272 && ch <= 1273) || (ch >= 1280 && ch <= 1295) || (ch >= 1329 && ch <= 1366) || ch == 1369 || (ch >= 1377 && ch <= 1415) || (ch >= 1488 && ch <= 1514) || (ch >= 1520 && ch <= 1522) || (ch >= 1569 && ch <= 1594) || (ch >= 1600 && ch <= 1610) || (ch >= 1646 && ch <= 1647) || (ch >= 1649 && ch <= 1747) || ch == 1749 || (ch >= 1765 && ch <= 1766) || (ch >= 1774 && ch <= 1775) || (ch >= 1786 && ch <= 1788) || ch == 1791 || ch == 1808 || (ch >= 1810 && ch <= 1839) || (ch >= 1869 && ch <= 1871) || (ch >= 1920 && ch <= 1957) || ch == 1969 || (ch >= 2308 && ch <= 2361) || ch == 2365 || ch == 2384 || (ch >= 2392 && ch <= 2401) || (ch >= 2437 && ch <= 2444) || (ch >= 2447 && ch <= 2448) || (ch >= 2451 && ch <= 2472) || (ch >= 2474 && ch <= 2480) || ch == 2482 || (ch >= 2486 && ch <= 2489) || ch == 2493 || (ch >= 2524 && ch <= 2525) || (ch >= 2527 && ch <= 2529) || (ch >= 2544 && ch <= 2545) || (ch >= 2565 && ch <= 2570) || (ch >= 2575 && ch <= 2576) || (ch >= 2579 && ch <= 2600) || (ch >= 2602 && ch <= 2608) || (ch >= 2610 && ch <= 2611) || (ch >= 2613 && ch <= 2614) || (ch >= 2616 && ch <= 2617) || (ch >= 2649 && ch <= 2652) || ch == 2654 || (ch >= 2674 && ch <= 2676) || (ch >= 2693 && ch <= 2701) || (ch >= 2703 && ch <= 2705) || (ch >= 2707 && ch <= 2728) || (ch >= 2730 && ch <= 2736) || (ch >= 2738 && ch <= 2739) || (ch >= 2741 && ch <= 2745) || ch == 2749 || ch == 2768 || (ch >= 2784 && ch <= 2785) || (ch >= 2821 && ch <= 2828) || (ch >= 2831 && ch <= 2832) || (ch >= 2835 && ch <= 2856) || (ch >= 2858 && ch <= 2864) || (ch >= 2866 && ch <= 2867) || (ch >= 2869 && ch <= 2873) || ch == 2877 || (ch >= 2908 && ch <= 2909) || (ch >= 2911 && ch <= 2913) || ch == 2929 || ch == 2947 || (ch >= 2949 && ch <= 2954) || (ch >= 2958 && ch <= 2960) || (ch >= 2962 && ch <= 2965) || (ch >= 2969 && ch <= 2970) || ch == 2972 || (ch >= 2974 && ch <= 2975) || (ch >= 2979 && ch <= 2980) || (ch >= 2984 && ch <= 2986) || (ch >= 2990 && ch <= 2997) || (ch >= 2999 && ch <= 3001) || (ch >= 3077 && ch <= 3084) || (ch >= 3086 && ch <= 3088) || (ch >= 3090 && ch <= 3112) || (ch >= 3114 && ch <= 3123) || (ch >= 3125 && ch <= 3129) || (ch >= 3168 && ch <= 3169) || (ch >= 3205 && ch <= 3212) || (ch >= 3214 && ch <= 3216) || (ch >= 3218 && ch <= 3240) || (ch >= 3242 && ch <= 3251) || (ch >= 3253 && ch <= 3257) || ch == 3261 || ch == 3294 || (ch >= 3296 && ch <= 3297) || (ch >= 3333 && ch <= 3340) || (ch >= 3342 && ch <= 3344) || (ch >= 3346 && ch <= 3368) || (ch >= 3370 && ch <= 3385) || (ch >= 3424 && ch <= 3425) || (ch >= 3461 && ch <= 3478) || (ch >= 3482 && ch <= 3505) || (ch >= 3507 && ch <= 3515) || ch == 3517 || (ch >= 3520 && ch <= 3526) || (ch >= 3585 && ch <= 3632) || (ch >= 3634 && ch <= 3635) || (ch >= 3648 && ch <= 3654) || (ch >= 3713 && ch <= 3714) || ch == 3716 || (ch >= 3719 && ch <= 3720) || ch == 3722 || ch == 3725 || (ch >= 3732 && ch <= 3735) || (ch >= 3737 && ch <= 3743) || (ch >= 3745 && ch <= 3747) || ch == 3749 || ch == 3751 || (ch >= 3754 && ch <= 3755) || (ch >= 3757 && ch <= 3760) || (ch >= 3762 && ch <= 3763) || ch == 3773 || (ch >= 3776 && ch <= 3780) || ch == 3782 || (ch >= 3804 && ch <= 3805) || ch == 3840 || (ch >= 3904 && ch <= 3911) || (ch >= 3913 && ch <= 3946) || (ch >= 3976 && ch <= 3979) || (ch >= 4096 && ch <= 4129) || (ch >= 4131 && ch <= 4135) || (ch >= 4137 && ch <= 4138) || (ch >= 4176 && ch <= 4181) || (ch >= 4256 && ch <= 4293) || (ch >= 4304 && ch <= 4344) || (ch >= 4352 && ch <= 4441) || (ch >= 4447 && ch <= 4514) || (ch >= 4520 && ch <= 4601) || (ch >= 4608 && ch <= 4614) || (ch >= 4616 && ch <= 4678) || ch == 4680 || (ch >= 4682 && ch <= 4685) || (ch >= 4688 && ch <= 4694) || ch == 4696 || (ch >= 4698 && ch <= 4701) || (ch >= 4704 && ch <= 4742) || ch == 4744 || (ch >= 4746 && ch <= 4749) || (ch >= 4752 && ch <= 4782) || ch == 4784 || (ch >= 4786 && ch <= 4789) || (ch >= 4792 && ch <= 4798) || ch == 4800 || (ch >= 4802 && ch <= 4805) || (ch >= 4808 && ch <= 4814) || (ch >= 4816 && ch <= 4822) || (ch >= 4824 && ch <= 4846) || (ch >= 4848 && ch <= 4878) || ch == 4880 || (ch >= 4882 && ch <= 4885) || (ch >= 4888 && ch <= 4894) || (ch >= 4896 && ch <= 4934) || (ch >= 4936 && ch <= 4954) || (ch >= 5024 && ch <= 5108) || (ch >= 5121 && ch <= 5740) || (ch >= 5743 && ch <= 5750) || (ch >= 5761 && ch <= 5786) || (ch >= 5792 && ch <= 5866) || (ch >= 5870 && ch <= 5872) || (ch >= 5888 && ch <= 5900) || (ch >= 5902 && ch <= 5905) || (ch >= 5920 && ch <= 5937) || (ch >= 5952 && ch <= 5969) || (ch >= 5984 && ch <= 5996) || (ch >= 5998 && ch <= 6000) || (ch >= 6016 && ch <= 6067) || ch == 6103 || ch == 6108 || (ch >= 6176 && ch <= 6263) || (ch >= 6272 && ch <= 6312) || (ch >= 6400 && ch <= 6428) || (ch >= 6480 && ch <= 6509) || (ch >= 7424 && ch <= 7531) || (ch >= 7680 && ch <= 7835) || (ch >= 7840 && ch <= 7929) || (ch >= 7936 && ch <= 7957) || (ch >= 7960 && ch <= 7965) || (ch >= 7968 && ch <= 8005) || (ch >= 8008 && ch <= 8013) || (ch >= 8016 && ch <= 8023) || ch == 8025 || ch == 8027 || ch == 8029 || (ch >= 8031 && ch <= 8061) || (ch >= 8064 && ch <= 8116) || (ch >= 8118 && ch <= 8124) || ch == 8126 || (ch >= 8130 && ch <= 8132) || (ch >= 8134 && ch <= 8140) || (ch >= 8144 && ch <= 8147) || (ch >= 8150 && ch <= 8155) || (ch >= 8160 && ch <= 8172) || (ch >= 8178 && ch <= 8180) || (ch >= 8182 && ch <= 8188) || ch == 8305 || ch == 8319 || ch == 8450 || ch == 8455 || (ch >= 8458 && ch <= 8467) || ch == 8469 || (ch >= 8473 && ch <= 8477) || ch == 8484 || ch == 8486 || ch == 8488 || (ch >= 8490 && ch <= 8493) || (ch >= 8495 && ch <= 8497) || (ch >= 8499 && ch <= 8505) || (ch >= 8509 && ch <= 8511) || (ch >= 8517 && ch <= 8521) || (ch >= 8544 && ch <= 8579) || (ch >= 12293 && ch <= 12295) || (ch >= 12321 && ch <= 12329) || (ch >= 12337 && ch <= 12341) || (ch >= 12344 && ch <= 12348) || (ch >= 12353 && ch <= 12438) || (ch >= 12445 && ch <= 12447) || (ch >= 12449 && ch <= 12538) || (ch >= 12540 && ch <= 12543) || (ch >= 12549 && ch <= 12588) || (ch >= 12593 && ch <= 12686) || (ch >= 12704 && ch <= 12727) || (ch >= 12784 && ch <= 12799) || ch == 13312 || ch == 19893 || (ch >= 19968 && ch <= 42124) || ch == 44032 || ch == 55203 || (ch >= 64256 && ch <= 64262) || (ch >= 64275 && ch <= 64279) || ch == 64285 || (ch >= 64287 && ch <= 64296) || (ch >= 64298 && ch <= 64310) || (ch >= 64312 && ch <= 64316) || ch == 64318 || (ch >= 64320 && ch <= 64321) || (ch >= 64323 && ch <= 64324) || (ch >= 64326 && ch <= 64433) || (ch >= 64467 && ch <= 64829) || (ch >= 64848 && ch <= 64911) || (ch >= 64914 && ch <= 64967) || (ch >= 65008 && ch <= 65019) || (ch >= 65136 && ch <= 65140) || (ch >= 65142 && ch <= 65276) || (ch >= 65313 && ch <= 65338) || (ch >= 65345 && ch <= 65370) || (ch >= 65382 && ch <= 65470) || (ch >= 65474 && ch <= 65479) || (ch >= 65482 && ch <= 65487) || (ch >= 65490 && ch <= 65495) || (ch >= 65498 && ch <= 65500)) {apx++; AddCh(); goto case_205;}
			else if (ch == 10 || ch == 13 || ch == 39 || ch == L'/' || ch == 133 || (ch >= 8232 && ch <= 8233)) {apx++; AddCh(); goto case_204;}
			else if (ch == L'e') {apx++; AddCh(); goto case_340;}
			else {goto case_0;}
		case 336:
			case_336:
			if (ch == L'l') {AddCh(); goto case_93;}
			else if (ch == L't') {AddCh(); goto case_99;}
			else if (ch == L'o') {AddCh(); goto case_101;}
			else {goto case_0;}
		case 337:
			case_337:
			if (ch == L'l') {AddCh(); goto case_128;}
			else if (ch == L'o') {AddCh(); goto case_134;}
			else if (ch == L'u') {AddCh(); goto case_138;}
			else {goto case_0;}
		case 338:
			case_338:
			if (ch == L'e') {AddCh(); goto case_176;}
			else if (ch == L't') {AddCh(); goto case_182;}
			else if (ch == L'u') {AddCh(); goto case_188;}
			else {goto case_0;}
		case 339:
			case_339:
			if (ch == L'r') {AddCh(); goto case_341;}
			else {goto case_0;}
		case 340:
			case_340:
			recEnd = pos; recKind = 77;
			if (ch == L'a') {apx = 0; AddCh(); goto case_206;}
			else {
				tlen -= apx;
				SetScannerBehindT();				buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
				for (int i = 0; i < tlen; i++) NextCh();
				t->kind = 77; break;}
		case 341:
			case_341:
			if (ch == L'a') {AddCh(); goto case_191;}
			else if (ch == L'y') {AddCh(); goto case_195;}
			else {goto case_0;}
		case 342:
			case_342:
			{t->kind = 106; break;}
		case 343:
			case_343:
			{t->kind = 108; break;}
		case 344:
			case_344:
			{t->kind = 109; break;}
		case 345:
			{t->kind = 128; break;}
		case 346:
			{t->kind = 137; break;}
		case 347:
			recEnd = pos; recKind = 92;
			if (ch == L'=') {AddCh(); goto case_25;}
			else {t->kind = 92; break;}
		case 348:
			recEnd = pos; recKind = 93;
			if (ch == L'=') {AddCh(); goto case_26;}
			else {t->kind = 93; break;}
		case 349:
			recEnd = pos; recKind = 129;
			if (ch == L'=') {AddCh(); goto case_27;}
			else {t->kind = 129; break;}
		case 350:
			recEnd = pos; recKind = 130;
			if (ch == L'=') {AddCh(); goto case_28;}
			else {t->kind = 130; break;}
		case 351:
			recEnd = pos; recKind = 105;
			if (ch == L'=') {AddCh(); goto case_342;}
			else if (ch == L'>') {AddCh(); goto case_344;}
			else {t->kind = 105; break;}
		case 352:
			recEnd = pos; recKind = 107;
			if (ch == L'=') {AddCh(); goto case_343;}
			else {t->kind = 107; break;}

	}
	AppendVal(t);
	return t;
}

void Scanner::SetScannerBehindT() {
	buffer->SetPos(t->pos);
	NextCh();
	line = t->line; col = t->col; charPos = t->charPos;
	for (int i = 0; i < tlen; i++) NextCh();
}

// get the next token (possibly a token already seen during peeking)
Token* Scanner::Scan() {
	if (tokens->next == NULL) {
		return pt = tokens = NextToken();
	} else {
		pt = tokens = tokens->next;
		return tokens;
	}
}

// peek for the next token, ignore pragmas
Token* Scanner::Peek() {
	do {
		if (pt->next == NULL) {
			pt->next = NextToken();
		}
		pt = pt->next;
	} while (pt->kind > maxT); // skip pragmas

	return pt;
}

// make sure that peeking starts at the current scan position
void Scanner::ResetPeek() {
	pt = tokens;
}


