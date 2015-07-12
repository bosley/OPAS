The string analyzer can be loaded with data structures containing maps for word 'similar' words to be grouped "Verbs, nouns, etc"
so they can be searched by type later on. The compairison is done via Levenshtein Distance on strings to allow for up to %25 error.
This is DIFFERENCE_THRESHOLD in stringAnalyzer.h. 

In addition, the compairison of two words can be directly called via compair.


I wrote this in QT 5.2.0, but there is no QT specific code in the files containing the stringanalyzer class.


Note: 


  I put this in a namespace because I intend on adding on to this significantly for other projects.
