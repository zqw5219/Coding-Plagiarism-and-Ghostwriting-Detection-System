from authorizerPackage import *

# run c2Txt() for all files in fileNames
# convert all .c files in 'project\volume\data\raw' to .txt files and store at 'project\volume\data\interim'
for i in fileNames:
    c2Txt(i)



