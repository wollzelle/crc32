####
# DYNAMIC
LIBS=-lz
STATIC=
#
####
# STATIC
#LIBS=/usr/lib/libz.a
#STATIC=-static
####


crc32: crc32.o
	gcc ${STATIC} -o crc32 crc32.o ${LIBS}

all: crc32

install: crc32
	strip crc32
	install -d ${DESTDIR}/usr/local/bin
	install crc32 ${DESTDIR}/usr/local/bin

clean:
	rm -f crc32.o crc32


