from authorizerPackage import *

# Tasks:
# create a data frame for a single file to store everything and group by type
# add a variable/column commentIndicator to indicate if a line is or contains comment
#   where:
#       0 indicates pure code
#       0.5 indicates this line contains comment
#       1 indicates pure comment
#       2 indicates macro
#
# DF structure:
# ##      0       ##       1       ##     2    ##      3      ##     4     ##
# ## originalCode ## typeIndicator ## pureCode ## pureComment ## pureMacro ##
#  0              ##               ##          ##             ##           ##
#  1              ##               ##          ##             ##           ##

# convert all .txt files in 'project\volume\data\interim' to dataframes
# and store as .csv files in 'project\volume\data\interim'
for i in range(len(fileNames)):
    df2Csv(createDataFrame(fileNames[i][:-1] + 'txt'), fileNames[i][:-1] + 'csv')



# # create target dataframe for a single file
# df = createDataFrame('bits_TJ.txt')
#
# # write the dataframe df as 'bits_TJ.csv' in 'project\volume\data\interim'
# df2Csv(df, 'bits_TJ.csv')
