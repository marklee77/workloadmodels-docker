.PHONY: clean install

m_lublin99: main.c
	gcc main.c -lm -o m_lublin99

clean:
	rm m_lublin99

install:
	install -o root -g root -m 755 m_lublin99 /usr/local/bin/m_lublin99
