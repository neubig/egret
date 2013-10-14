
SRCS=\
	Egret/src/BerkeleyGrammar.cpp \
	Egret/src/BerkeleyLexicon.cpp \
	Egret/src/BerkeleyRule.cpp \
	Egret/src/BerkeleyRuleSet.cpp \
	Egret/src/BerkeleyStateInfo.cpp \
	Egret/src/Bijection.cpp \
	Egret/src/Binarization.cpp \
	Egret/src/bob-hash.cpp \
	Egret/src/ERSet.cpp \
	Egret/src/ExtentInfo.cpp \
	Egret/src/Forest-Berkeley-2.cpp \
	Egret/src/Forest-Berkeley.cpp \
	Egret/src/Forest-Berkeley-ObjectFunction.cpp \
	Egret/src/Forest-Berkeley-ruleByParent.cpp \
	Egret/src/Forest.cpp \
	Egret/src/JKArgs.cpp \
	Egret/src/Jstream.cpp \
	Egret/src/Label.cpp \
	Egret/src/Main.cpp \
	Egret/src/Node.cpp \
	Egret/src/ReplaceLabelMachine.cpp \
	Egret/src/Rule.cpp \
	Egret/src/RuleSet.cpp \
	Egret/src/TransLabel.cpp \
	Egret/src/Tree.cpp \
	Egret/src/utils.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all: egret

egret: $(OBJS)
	g++ $(LDFLAGS) -o egret $(OBJS)

clean:
	$(RM) egret $(OBJS)
