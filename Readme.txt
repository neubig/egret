Egret: A set of syntax parsing tools, including inside-outside grammar estimation, pure PCFG parser, and a latent-annotation-based syntax parser (a reimplementation of Berkeley Parser, with F1-Measure(length <=40words): Chinese 85.45%, English 89.64%, All length: Chinese 84.16%, English 89.02%). It features static and dynamic pruning, and can output n-best trees and packed forests for other NLP applications. It is open source and free for research purposes. 

Directory
=========================
1) Egret 
   - Egret, Project Dir, for code generation
   - src , source code
   - Egret.sln , VS 8.0 solution file
2) chn_grammar, contains latent-annotation pcfg grammar trained on Penn Chinese TreeBank 6.0
   - levelx.grammar, x level grammar file
   - levelx.lexicon, x level lexicon file
3) eng_grammar, contains latent-annotation pcfg grammar trained on Penn English Treebank 3.0
4) testeng.gra, a pure pcfg grammar example
5) testeng.txt, English test sentences
6) test-chn.txt, Chinese test sentences
7) make-linux.sh, shell file to make the linux binary file
8) egret.exe, win32 binary file 

How to compile
=========================
For windows, please open the file "Egret.sln", and click "build". Note: you may install stlport first.
For linux, just run make-linux.sh

Example Usage
=========================
***Example usage of the latent-annotation pcfg Parser
--get 100 best trees
  egret -lapcfg -i=test-chn.txt -data=chn_grammar -n=100 > log.txt
  
--parse with n level grammar (default is 5)
  egret -lapcfg -i=test-chn.txt -data=chn_grammar -grammarLevel=3 > log.txt
 
--get packed forest (dynamic pruning)
  egret -lapcfg -i=test-chn.txt -data=chn_grammar -nbest4threshold=100 -printForest > log.txt
  -nbest4threshold=100 means all the hyperedges which do not appear in the 100 nbest trees will be pruned
  However, please note that even after pruning there are still exponential number of additional trees embedded in the forest 
  because of the sharing structure of forest.
  
--get packed forest (static pruning)
  egret -lapcfg -i=test-chn.txt -data=chn_grammar -threshold=10 -printForest > log.txt
  -threshold=10 means all the hyperedges with margin larger than 10 to the viterbi path will be prune
   
***Example usage of the pure pcfg Parser
  egret -pcfg -g=testeng.gra -i=testeng.txt -n=3
 
***Example usage of inside-outside extimation
  egret -io -g=testeng.gra -i=testeng.txt -pass=3

Acknowledgment
=========================
Parts of this software were written when I studied syntax parsing in NUS and XMU. 
I would like to thank Prof. Hwee Tou Ng(NUS) and Prof. Xiaodong Shi(XMU) for their guidance and support during this process.

Reference
=========================
The inside-outside algorithm was described in
"Trainable grammars for speech recognition."
J. Baker (1979). In J. J. Wolf and D. H. Klatt, editors, Speech communication papers presented at the 97th meeting of the Acoustical Society of America, pages 547¨C550, Cambridge, MA, June 1979. MIT.
And
Christopher D. Manning, Heinrich Sh¨¹tze (1999): Foundations of statistical natural language processing.

The K-Best Algorithm was described in
"Better k-best Parsing." 
Liang Huang and David Chiang (2005). 
In Proceedings of the 9th International Workshop on Parsing Technologies (IWPT 2005), Vancouver, BC. 

The latent-annotation model was described in 
"Learning Accurate, Compact, and Interpretable Tree Annotation"
Slav Petrov, Leon Barrett, Romain Thibaux and Dan Klein 
in COLING-ACL 2006  
And
"Improved Inference for Unlexicalized Parsing"
Slav Petrov and Dan Klein 
in HLT-NAACL 2007
And 
I also learned a lot of valuable details from the source code of Berkeley Parser.

Contact
=========================
Syntax parsing is so full of fun that I have to share my joy with those who are interested in this topic.
Hui Zhang 
zhangh1982@gmail.com
http://sites.google.com/site/zhangh1982

-------------------------
This version of Egret has been modified by Graham Neubig to fix a few bugs.
Major changes include:
* Preventing a segmentation fault in English parsing on encountering unusual
  unknown words
* Removing a flag that lexicalizes punctuation in Chinese parsing only for
  tree output (but not forest output)
* Adding a Japanese parsing model (trained on the Japanese word dependency
  corpus of Mori et al. 2014)
