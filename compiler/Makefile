
TESTFILE := test.c

Main: Main.class Lexer.class Normalizer.class
	java $@ $(TESTFILE)

%.class: %.java
	javac $^

clean:
	rm -rf *.class
