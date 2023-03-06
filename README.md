# Authorizer
 Spring 2021 DS 340W Semester Project


## Convention

Following this directory structure
```
|--project_name                           <- Project root level that is checked into github
  |--project                              <- Project folder
    |--README.md                          <- Top-level README for developers
    |--volume
    |   |--data
    |   |   |--external                   <- Data from third party sources
    |   |   |--interim                    <- Intermediate data that has been transformed
    |   |   |--processed                  <- The final model-ready data
    |   |   |--raw                        <- The original data dump
    |   |
    |   |--models                         <- Trained model files that can be read into R or Python
    |
    |--required
    |   |--requirements.txt               <- The required libraries for reproducing the Python environment
    |
    |
    |--src
    |   |--development                    <- Scripts for testing and development purpose that are not formatted
    |   |   |--remove_template.py
    |   |   |--similarity.ipynb
    |   |   |--string_similarity_check.py
    |   |--features                       <- Scripts for turning raw and external data into model-ready data
    |   |   |--utils                      <- Scripts for pycparser features such as preprocessing
    |   |   |--authorizerPackage.py       <- Functions for all features needed
    |   |   |--codeAndCommentSparator.py
    |   |   |--codeElementExtractor.py
    |   |   |--dataReader.py              
    |   |   |--main.py                    <- Script that contains the instruction of this project
    |   |   |--pycparserApplication.py
    |   |   |--randomForestClassifier.py  <- Script that generates random forest classifier on extracted features
    |   |   |--similarityScoreByLine.py   <- Script that extract features based on similarity by line
    |   |   |--similarityScoreByWord.py   <- Script that extract features based on similarity by word
    |   |   |--structureExtractor.py      <- Script that extract features based on structure
    |   |   |--testing.py                 <- Script saved for all testing and developing purpose
    |   |
    |   |--models                         <- Scripts for training and saving models
    |   |   |--.gitkeep
    |   |
    |
    |
    |
    |--.getignore                         <- List of files not to sync with github
```
