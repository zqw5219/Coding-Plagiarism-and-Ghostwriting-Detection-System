from authorizerPackage import *

# Finally all steps will be processed here

# currently records the correct order on running all scripts

# import authorizerPackage.py by using:
# from authorizerPackage import *

# 1. run dataReader.py
#       convert all .c files in 'project\volume\data\raw'
#                to .txt files and store in 'project\volume\data\interim'

# # convert all .c files in 'project\volume\data\raw' to .txt files and store at 'project\volume\data\interim'
# for i in fileNames:
#     c2Txt(i)

###################################################################################################################
# 2. run codeAndCommentSeparator.py
#       read converted .txt files in 'project\volume\data\interim'
#       identify each line with label (comment, code, mixed, macro)/(1, 0, 0.5, 2)
#       store in List: typeIndicator
#       DF structure:
#       ##      0       ##       1       ##     2    ##      3      ##     4     ##
#       ## originalCode ## typeIndicator ## pureCode ## pureComment ## pureMacro ##
#        0              ##               ##          ##             ##           ##
#        1              ##               ##          ##             ##           ##

# # convert all .txt files in 'project\volume\data\interim' to dataframes
# # and store as .csv files in 'project\volume\data\interim'
# for i in range(len(fileNames)):
#     df2Csv(createDataFrame(fileNames[i][:-1] + 'txt'), fileNames[i][:-1] + 'csv')

###################################################################################################################
# run similarityScoreByLine.py
# which creates and stores the df with similarity features by line

###################################################################################################################
# run structureExtractor.py
# which creates and stores the df with structure/compleixty features
###################################################################################################################

###################################################################################################################
# Above steps were ran

# run functions that create other features
# combine all features and labels in a single dataframe

# apply random forest classifier
# run randomForestClassifier.py
