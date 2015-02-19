.PHONY: clean install

m_cirne01moldify: clock.cpp moldify.cpp downey.cpp jobspec.cpp nasjobs.cpp util.cpp moldability.cpp random.cpp randlib.cpp com.cpp
	g++ -w -g -O -D`uname` -o m_cirne01moldify clock.cpp moldify.cpp downey.cpp jobspec.cpp nasjobs.cpp util.cpp moldability.cpp random.cpp randlib.cpp com.cpp

clean:
	rm -f m_cirne01moldify

install: m_cirne01moldify
	install -o root -g root -m 755 m_cirne_01moldify /usr/local/bin/m_cirne01moldify
