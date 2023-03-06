from authorizerPackage import *

# inputs:
# fileName.c,
# project index[0-3]
# output:
# float
# similarity('bits_CX.c', 0)

df2Csv(createSimilarityByWordDf(), 'similarityScoreByWord.csv')