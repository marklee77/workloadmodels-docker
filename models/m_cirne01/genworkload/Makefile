.PHONY: clean install

m_cirne01: genworkload.cpp clock.cpp downey.cpp jobspec.cpp nasjobs.cpp util.cpp jobstream.cpp random.cpp randlib.cpp com.cpp 
	g++ -w -g -O -D`uname` -static -o m_cirne01 genworkload.cpp clock.cpp downey.cpp jobspec.cpp nasjobs.cpp util.cpp jobstream.cpp random.cpp randlib.cpp com.cpp

clean:
	rm -f m_cirne01

install: m_cirne01
	install -o root -g root -m 755 m_cirne_01 /usr/local/bin/m_cirne01
