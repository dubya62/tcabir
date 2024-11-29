
TESTFILE := test.c

Main: Main.class Lexer.class Normalizer.class Simplifier.class Converter.class TypeChecker.class Ignorer.class Inliner.class Jumper.class
	java $@ $(TESTFILE)

%.class: %.java
	javac $^

clean:
	rm -rf *.class
