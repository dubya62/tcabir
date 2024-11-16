
TESTFILE := test.c

Main: Main.class Lexer.class Normalizer.class Simplifier.class Converter.class
	java $@ $(TESTFILE)

%.class: %.java
	javac $^

clean:
	rm -rf *.class
