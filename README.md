# YiSi: A Semantic Machine Translation Evaluation Metric for Evaluating Languages with Different Levels of Available Resources
## Introduction
YiSi<sup>[a]</sup> is a family of semantic machine translation (MT) evaluation metrics 
with a flexible architecture for evaluating MT output in languages of different 
resource levels. Inspired by MEANT 2.0 (Lo, 2017), YiSi-1 measures the similarity 
between the human references and machine translation by aggregating the weighted
distributional lexical semantic similarity, and, optionally, the shallow semantic 
structures. YiSi-0 is a degenerate resource-free version using the longest 
common character substring accuracy to replace distributional semantics for 
evaluating lexical similarity between the human reference and MT output. On the 
other hand, YiSi-2 is the bilingual reference-less version using bilingual word 
embeddings for evaluating crosslingual lexical semantic similarity between the input 
and MT output. 

YiSi-1 achieved the highest average correlation with human direct assessment (DA) 
judgment across all language pairs at system-level and the highest median correlation
 with DA relative ranking across all language pairs at segment-level in the WMT2018 
metrics task (Ma et al., 2018). YiSi-1 also successfully served in WMT2018 parallel 
corpus filtering task while YiSi-2 showed comparable accuracy in the same task.

YiSi-0 is readily available for evaluating all languages. YiSi-1 requires a 
monolingual corpus in the output language to train the distributional lexical 
semantics model. YiSi-1_srl is designed for resource-rich languages that are equipped 
with an automatic semantic role labeler in the output language. YiSi-2 requires 
bilingual word embeddings and YiSi-2_srl addinionally requires an automatic semantic 
role labeler for both the input and output language.

<sup>[a]</sup> YiSi is the romanization of the Cantonese word "意思/meaning".

## Installation

### Prerequisites
#### Base requirements
- YiSi was developed to run on Linux.
- YiSi is written in C++ and requires a version of `g++` that supports C++11; we're using GCC 4.9.3.
- YiSi requires `make`; we're using GNU Make 3.81.
- YiSi requires `bash`; we're using GNU bash, version 4.1.2.

#### Additional requirements to use SRL
- YiSi interfaces to a Java SRL library (mateplus), thus requires Java JDK 1.8 to build `srlmate.jar`.
- Define the `JAVA_HOME` environment variable:
  ```bash
  export JAVA_HOME=/path/to/jdk_install_directory
  ```
- YiSi depends on mateplus, an extended version of the mate-tools semantic role labeler.
You can download and install mateplus from:  
  https://github.com/microth/mateplus
- Make sure to install all the mateplus dependencies listed in its README.
- Define the `MATEPLUS_HOME` environment variable:
  ```bash
  export MATEPLUS_HOME=/path/to/mateplus_install_director
  ```
  Thus, the location of `mateplus.jar` is `$MATEPLUS_HOME/mateplus.jar`
- Put the JAR files for the dependencies you install for mateplus in `$MATEPLUS_HOME/lib`.
- Put the models you download for mateplus in `$MATEPLUS_HOME/lib`.

### Building YiSi
If building YiSi with SRLMATE in order to use SRL, then either define the `JAVE_HOME`
and `MATEPLUS_HOME` environment variables as instructed above, or edit the default
values defined in the YiSi `src/Makefile` and `test/Makefile`.

You may also want to define:
```bash
export YISI_HOME=/path/to/YiSi_git
```

To build YiSi, run the following commands:
```bash
cd $YISI_HOME/src
make all -j 4
```

To run the YiSi tests, either from `$YISI_HOME/src/` or `$YISI_HOME/test/`, run:
```bash
make test
```

If mateplus is not installed or `MATEPLUS_HOME` does not point at your mateplus,
YiSi will be built without SRLMATE; otherwise YiSi will be built with SRLMATE.

No additional `make install` step is needed for YiSi. The `make all` step builds
all the YiSi programs in `$YISI_HOME/bin/`.

The path to SRLMATE, if it was built, is: `$YISI_HOME/obj/srlmate.jar`

## Running YiSi
Although probably not required, we recommend adding the YiSi bin directory to `$PATH`:
```bash
export PATH=$YISI_HOME/bin:$PATH
```
YiSi has a lot of command line options (see `yisi --help`.
It's easiest to drive YiSi using a config file.
For example:
```bash
> cd $YISI_HOME/test

> cat yisi_1.config
srclang=de
tgtlang=en
lexsim-type=w2v
outlexsim-path=mini.d300.en
reflexweight-type=learn
phrasesim-type=nwpr
ngram-size=3
mode=yisi
alpha=0.8
ref-file=test_ref.en
hyp-file=test_hyp.en
sntscore-file=test_hyp.sntyisi1
docscore-file=test_hyp.docyisi1

> yisi --config yisi_1.config
Reading w2v text model from mini.d300.en
Size of voc: 500 Dimension: 300
Finished reading w2v model.
Learning lex weight from test_ref.en ... Done.
Tokenizing/SRL-ing hyp ... Done.
Tokenizing/SRL-ing ref ... Done.
Evaluating line 1
Evaluating line 2
Evaluating line 3
Evaluating line 4
Evaluating line 5
Evaluating line 6
Evaluating line 7
Evaluating line 8
Evaluating line 9
Evaluating line 10
```
`$YISI_HOME/test/` contains sample config files for running various YiSi scenarios on toy data:
```bash
> cd $YISI_HOME/test
> ls yisi-*.config
yisi-0.config  yisi-1.config  yisi-1_srl.config  yisi-2.config  yisi-2_srl.config
```
Please note: YiSi-2_srl is not ready for release yet, so don't try running `yisi yisi-2_srl.config`.

`$YISI_HOME/bin/` contains also contains many test programs (`*_test`),
which are used primarily for unit-testing.
See `$YISI_HOME/test/Makefile` for examples of how to call these programs, if interested.

## Pretrained word embeddings for YiSi-1
Unit vectors built by word2vec trained on the latest WMT translation task monolingual data are available for download at:
http://chikiu-jackie-lo.org/home/index.php/yisi

## References
[In progress]

## Acknowledgements
I would like to give special thanks to the following people:

Darlene Stewart, for her major efforts in defense coding and packaging the software. This release would be in a much worse shape without her covering up the potholes lying everywhere.

Markus Saers, for his accomodations in licensing the command line parser and fulfilling wishlist items in it.

Everyone in the NRC MTP team and Karteek Addanki, Meriem Beloucif, Nedjma Ousidhoum, Andrew Cattle and Marine Carpuat, for the moral support in the critical moment when YiSi was born.
