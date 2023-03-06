import os
import numpy as np
import pandas as pd
import re
import difflib
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
import pycparser
import cffi
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import linear_kernel

############################## Variables ####################################
# all prefix: 'bits_', 'cart_driver_', 'proxy_', 'tsh_'
# a list contains all file name of .c files we are going to convert
fileNames = [
    'bits_FYA.c', 'cart_driver_FYA.c', 'proxy_FYA.c', 'tsh_FYA.c',
    'bits_TJ.c', 'cart_driver_TJ.c', 'proxy_TJ.c', 'tsh_TJ.c',
    'bits_CX.c', 'cart_driver_CX.c', 'proxy_CX.c', 'tsh_CX.c',
    'bits_TEMP.c', 'cart_driver_TEMP.c', 'proxy_TEMP.c', 'tsh_TEMP.c'
]

bitsNames = [
    'bits_FYA.c',
    'bits_TJ.c',
    'bits_CX.c',
    'bits_TEMP.c'
]
cartNames = [
     'cart_driver_FYA.c',
     'cart_driver_TJ.c',
     'cart_driver_CX.c',
     'cart_driver_TEMP.c'
]
proxyNames = [
    'proxy_FYA.c',
    'proxy_TJ.c',
    'proxy_CX.c',
    'proxy_TEMP.c'
]
tshNames = [
    'tsh_FYA.c',
    'tsh_TJ.c',
    'tsh_CX.c',
    'tsh_TEMP.c'
]

patterns = ['struct', 'return', 'int', 'char', '->', '<-', 'void', 'include']

############################## Classes #####################################
class FunctionList(pycparser.c_ast.NodeVisitor):
    def __init__(self, source):
        self.funcs = set()
        self.visit(pycparser.CParser().parser(source))

    def visit_FuncDef(self, node):
        self.funcs.add(node.decl.name)

# class CFFIGenerator(pycparser.c_generator.CGenerator):
#     def __init__(self, blacklist):
#         super().__init__()
#         self.blacklist = blacklist
#
#     def visit_Decl(selfself, n, *args, **kwargs):
#         result = super().visit_Decl(n, *args, **kwargs)
#         if isinstance(n,type, pycparser.c_ast.FuncDecl):
#             if n.name not in self.blacklist:
#                 return 'extern "Python+C" ' + result
#             return result
#
# def convert_function_declarations(source, blacklist):
#     return CFFIGenerator(blacklist).visit(pycparser.CParser().parse(source))

############################## Functions ####################################

### change working directory
# get current working directory
# print(os.getcwd())

# change the current working directory from 'project\src\features' to 'project\volume\data\raw'
def wdFeature2Raw():
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to 'project\volume\data\raw'
    os.chdir(os.path.abspath('..\\volume\\data\\raw'))


# change the current working directory from 'project\volume\data\raw' to 'project\src\features'
def wdRaw2Feature():
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to 'project\volume\data\raw'
    os.chdir(os.path.abspath('..\\src\\features'))

# change the current working directory from 'project\src\features' to 'project\volume\data\interim'
def wdFeature2Interim():
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to 'project\volume\data\interim'
    os.chdir(os.path.abspath('..\\volume\\data\\interim'))


# change the current working directory from 'project\volume\data\interim' to 'project\src\features'
def wdInterim2Feature():
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to the parent directory
    os.chdir(os.path.abspath('..'))
    # change working directory to 'project\volume\data\raw'
    os.chdir(os.path.abspath('..\\src\\features'))

# change the current working directory from 'project\volume\data\raw' to 'project\volume\data\interim'
def wdRaw2Interim():
    # change working directory to 'project\volume\data\interim', where data is the parent directory
    os.chdir(os.path.abspath('..\\interim'))

# change the current working directory from 'project\volume\data\interim' to 'project\volume\data\raw'
def wdInterim2Raw():
    # change working directory to 'project\volume\data\raw'
    os.chdir(os.path.abspath('..\\raw'))



### Convert, Read, Write Files

# read fileName.c in 'project\volume\data\raw', convert it to fileName.txt
# and store at 'project\volume\data\interim'
def readAndStore(fileName):
    # load raw data (currently with given file name for a single file at a time)
    with open(fileName, 'r') as f:
        temp_list = f.readlines()
    # change working directory from 'project\volume\data\raw' to 'project\volume\data\interim'
    wdRaw2Interim()
    # write and store the data we read
    with open(fileName[:-1] + 'txt', 'w') as f:
        f.write(''.join(temp_list))
    # change working directory from 'project\volume\data\interim' to 'project\volume\data\raw'
    wdInterim2Raw()

# convert a single .c file into .txt file and store in the 'project\volume\data\interim' folder
def c2Txt(fileName):
    # Set Up Working Directory to Load Raw Data
    # change the current working directory from 'project\src\features' to 'project\volume\data\raw'
    wdFeature2Raw()
    # convert a single .c file into .txt file and store in the 'project\volume\data\interim' folder
    readAndStore(fileName)
    # change the current working directory from 'project\volume\data\raw' back to 'project\src\features'
    wdRaw2Feature()

# load a single converted .txt data in 'project\volume\data\interim'
def readTxt(fileName):
    # change working directory from 'project\src\features' to 'project\volume\data\interim'
    wdFeature2Interim()
    # load converted interim data (currently with given file name for a single file at a time)
    with open(fileName, 'r') as f:
        # read fileName.txt by lines and return
        tempList = f.readlines()
    # change working directory from 'project\volume\data\interim' to 'project\src\features'
    wdInterim2Feature()
    # remove \n behind each line
    for i in range(len(tempList)):
        tempList[i] = tempList[i].strip()
    return tempList

# write the dataframe df as fileName.csv in 'project\volume\data\interim'
def df2Csv(df, fileName):
    # change working directory from 'project\src\features' to 'project\volume\data\interim'
    wdFeature2Interim()
    # write dataframe df as fileName
    df.to_csv(fileName, index = False, sep = ',')
    # change working directory from 'project\volume\data\interim' to 'project\src\features'
    wdInterim2Feature()

# load fileName.csv in 'project\volume\data\interim'
def csv2Df(fileName):
    # change working directory from 'project\src\features' to 'project\volume\data\interim'
    wdFeature2Interim()
    # load fileName as tempDf which in dataframe format
    tempDf = pd.read_csv(fileName)
    # change working directory from 'project\volume\data\interim' to 'project\src\features'
    wdInterim2Feature()
    # return output dataframe
    return tempDf


### processors

# returns a list 'typeIndicator' that indicates the code type of each line
def createTypeIndicator(fileName):
    # read a single converted .txt file in 'project\volume\data\interim'
    tempFile = readTxt(fileName)

    # a list which will become commentIndicator column in the data frame for tempFile
    # where 0 indicates the line is pure code,
    # 0.5 indicates this line contains comment
    # 1 indicates pure comment,
    # 2 indicates macro
    typeIndicator = []

    # an indicator indicates current status, where 0 means code, 1 means comment, 2 means macro
    statusIndicator = 0
    previous = 0
    # for each line
    for i in range(len(tempFile)):
        # if it contains '/*', it indicates comments are starting at this line,
        if re.search('/\*', tempFile[i]):
            # update previous
            previous = statusIndicator
            # change statusIndicator to 1
            statusIndicator = 1
            # add the type status to commentIndicator
            typeIndicator.append(statusIndicator)
            # if there is an '*/' in this line, comment ends, change the statusIndicator back to previous
            if re.search('\*/', tempFile[i]):
                statusIndicator = previous
                # update previous
                previous = statusIndicator
        # if the line contains '*/', comment ends, change the statusIndicator back to previous
        elif re.search('\*/', tempFile[i]):
            statusIndicator = previous
            # update previous
            previous = statusIndicator
            # add the type status to commentIndicator
            typeIndicator.append(statusIndicator)
        # if the line contains '//', it contains inline comment, don't change statusIndicator but write 0.5
        elif re.search('//', tempFile[i]):
            typeIndicator.append(0.5)
        # if the line starts with '#if', macro are starting at this line
        elif re.match('#if|# if', tempFile[i]):
            # update previous
            previous = statusIndicator
            # change statusIndicator to 2
            statusIndicator = 2
            # add the type status to commentIndicator
            typeIndicator.append(statusIndicator)
        # if the line starts with '#if', macro ends. write typeStatus (2) before changing it back to 0
        elif re.match('#endif|# endif', tempFile[i]):
            # add the type status to commentIndicator
            typeIndicator.append(statusIndicator)
            # update previous
            previous = statusIndicator
            statusIndicator = 0
        else:
            # add the type status to commentIndicator
            typeIndicator.append(statusIndicator)
    # return list typeIndicator
    return typeIndicator
#
# # test cases for input 'bits_TJ.txt' on createTypeIndicator()
# typeIndicator = createTypeIndicator('bits_TJ.txt')
# print('1?', typeIndicator[74])
# print('1?', typeIndicator[0])
# print('0.5?', typeIndicator[265])
# print('0?', typeIndicator[251])
# print('2?', typeIndicator[91])
# print('2?', typeIndicator[15])
# print('441?', len(typeIndicator))
# # all passed




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
#
# create target dataframe for a single file
def createDataFrame(fileName):
    # store corresponding data in columns
    originalCode = readTxt(fileName)
    typeIndicator = createTypeIndicator(fileName)
    # create data frame in the structure described above
    tempDf = pd.DataFrame({
        'originalCode': originalCode,
        'typeIndicator': typeIndicator,
        'pureCode': None,
        'pureComment': None,
        'pureMacro': None
    })
    # fill last three columns with correct data
    # for each row
    for i in range(tempDf.shape[0]):
        # if typeIndicator is 0, it is pure code
        if tempDf.iloc[i, 1] == 0:
            # update column 'pureCode'
            tempDf.iloc[i, 2] = tempDf.iloc[i, 0]
        # if typeIndicator is 1, it is pure comment
        elif tempDf.iloc[i, 1] == 1:
            # update column 'pureComment'
            tempDf.iloc[i, 3] = tempDf.iloc[i, 0]
        # if typeIndicator is 2, it is pure macro
        elif tempDf.iloc[i, 1] == 2:
            # update column 'pureMacro'
            tempDf.iloc[i, 4] = tempDf.iloc[i, 0]
        # if typeIndicator is 0.5, it contains comment, but might contain code as well
        elif tempDf.iloc[i, 1] == 0.5:
            # split the line in the form 'tempCode//tempComment' by '//' but only once,
            tempCode, tempComment = tempDf.iloc[i, 0].split('//', 1)
            # store '//tempComment' in the 'pureComment' column
            tempDf.iloc[i, 3] = '//' + tempComment
            # if tempCode is not empty
            if tempCode != '':
                # store tempCode in the 'pureCode' column
                tempDf.iloc[i, 2] = tempCode
        # if the typeIndicator is none of 0, 0.5, 1, 2
        else:
            # fill the last three columns with 'Error'
            tempDf.iloc[i, 2] = 'Error'
            tempDf.iloc[i, 3] = 'Error'
            tempDf.iloc[i, 4] = 'Error'
    # return the target dataFrame
    return tempDf

# inputs:
# 2 csv file names, which their similarity by line score will be calculated
# type should be one of the following:
#           0: column 'pureCode' will be extracted
#           1: column 'pureComment' will be extracted
#           2: column 'pureMacro' will be extracted
#           all: default, column 'originalCode' will be extracted
# returns:
# a list contains 4 floats:
# index:    meaning
#     0:    mean of ratio
#     1:    standard deviation of ratio
#     2:    mean of match result
#     3:    standard deviation of match result
def similarityByLineBetween2(csv1, csv2, type = 'all'):
    wdFeature2Interim()
    df1 = pd.read_csv(csv1)
    df2 = pd.read_csv(csv2)
    wdInterim2Feature()

    if type == 'all':
        tempColumn = 'originalCode'
    elif type == 0:
        tempColumn = 'pureCode'
    elif type == 1:
        tempColumn = 'pureComment'
    elif type == 2:
        tempColumn = 'pureMacro'
    else:
        print('type input is incorrect. should be one of [0, 1, 2, all] or nothing')

    df1=df1.replace(np.nan, '')
    df2=df2.replace(np.nan, '')

    s1 = df1[tempColumn].tolist()
    s2 = df2[tempColumn].tolist()
    # s1 = list(filter('', s1))
    # s2 = list(filter('', s2))

    difference_list=[]
    element=[]
    for i in s1:
        a=i
        for j in s2:
            difference_list.append(difflib.SequenceMatcher(None, a, j).ratio())
            element.append(a+" VS "+j)

    match_list=[]
    for i in difference_list:
        if i == 1:
            match_list.append(1)
        else:
            match_list.append(0)

    match = pd.DataFrame(element, columns=['element'])
    match["ratio"]=difference_list
    match["match_result"]=match_list

    return [match["ratio"].mean(), match["ratio"].std(), match["match_result"].mean(), match["match_result"].std()]


# inputs the file name of the .c file
# returns the project name it belongs to
def getProject(fileName):
    return fileName.split('_', 1)[0]

# inputs the file name of the .c file
# returns the Author name
def getAuthor(fileName):
    return fileName.split('_', 1)[1].split('.', 1)[0]


# calculates average match score between a file and all files with a certain project type
# inputs:
# fileName is a string ends with .c
# projectName should be one of the following:
#           0: avg among projects in bitsNames will be calculated
#           1: avg among projects in cartNames will be calculated
#           2: avg among projects in proxyNames will be calculated
#           3: avg among projects in tshNames will be calculated
#           all: default, avg among projects in fileNames will be calculated
# type should be one of the following:
#           0: column 'pureCode' will be extracted
#           1: column 'pureComment' will be extracted
#           2: column 'pureMacro' will be extracted
#           all: default, column 'originalCode' will be extracted
# returns:
# the calculated avg score
def avgScore(fileName, projectName='all', type='all', fileNames=fileNames,
             bitsNames=bitsNames, cartNames=cartNames, proxyNames=proxyNames, tshNames=tshNames):
    if projectName == 'all':
        tempList = fileNames
    elif projectName == 0:
        tempList = bitsNames
    elif projectName == 1:
        tempList = cartNames
    elif projectName == 2:
        tempList = proxyNames
    elif projectName == 3:
        tempList = tshNames
    else:
        print('incorrect project Name')

    tempScores = []
    for i in range(len(tempList)):
        if fileName != tempList[i]:
            tempScores.append(similarityByLineBetween2(fileName[:-1] + 'csv', tempList[i][:-1] + 'csv', type)[2])
    return sum(tempScores) / len(tempScores)


# create dataframe with the format:
#           # bitsAvgScore	# cartAvgScore	# proxyAvgScore	 # tshAvgScore	# bitsScore0  # cartScore0	# proxyScore0  # tshScore0	# overallScore	# projectType	# authorName
# fileName1
# fileName2
def createSimilarityByLineDf(fileNames = fileNames):
    featuresByLine = pd.DataFrame(columns = ['bitsAvgScore', 'cartAvgScore', 'proxyAvgScore', 'tshAvgScore', 'overallScore',
                                             'bitsScore0', 'cartScore0', 'proxyScore0', 'tshScore0', 'overallScore0',
                                             'projectType', 'authorName'])
    for i in range(len(fileNames)):
        tempList0 = []
        for j in range(5):
            if j != 4:
                tempList0.append(avgScore(fileNames[i], projectName = j))
            else:
                tempList0.append(avgScore(fileNames[i], projectName = 'all'))

        for j in range(5):
            if j != 4:
                tempList0.append(avgScore(fileNames[i], projectName = j, type = 0))
            else:
                tempList0.append(avgScore(fileNames[i], projectName = 'all', type = 0))
        tempList0.append(getProject(fileNames[i]))
        tempList0.append(getAuthor(fileNames[i]))
        featuresByLine.loc[i] = tempList0
    return featuresByLine



## Models

# use sklearn library for further analyze
# including split training and testing sets
# and model training
def TrainModel(features, labels, tempLabelName = '', testSize = 0.25, randomState = 25, nEstimators = 100):
    print('#####################################################################')
    print('----------------Process Starts----------------')
    train_features, test_features, train_labels, test_labels = train_test_split(features, labels,
                                                                                test_size = testSize, random_state = randomState)

    # print('Training Features Shape:', train_features.shape)
    # print('Training Labels Shape:', train_labels.shape)
    # print('Testing Features Shape:', test_features.shape)
    # print('Testing Labels Shape:', test_labels.shape)

    # Train Model
    print('----Step: Train Model for ' + tempLabelName + ' Starts----')
    rf = RandomForestRegressor(n_estimators = nEstimators, random_state = randomState)
    rf.fit(train_features, train_labels)
    print('----Step: Train Model Ends----')

    # Make Predictions on the Test Set
    print('----Step: Make Prediction Starts----')
    predictions = rf.predict(test_features)
    errors = abs(predictions - test_labels)
    print('Mean Absolute Error:', round(np.mean(errors), 2), 'degrees.')
    print('----Step: Make Prediction Ends----')


    # Determine Performance Metrics
    print('----Step: Determine Performance Starts----')
    mape = 100 * (errors / (test_labels + 1))
    accuracy = 100 - np.mean(mape)
    print('Accuracy:', round(accuracy, 2), '%.')
    print('----Step: Determine Performance Ends----')

    # step: return the model for saving purpose should be added in future developments

## pycparser related
# def load(fileName):
#     name = fileName
#     wdFeature2Raw()
#     source = open(fileName).read()
#     wdRaw2Feature()
# preprocess files for CFFI
#     includes = preprocess(''.join(re.findall('\s*#include\s+.*', source)))
#     local_functions = FunctionList(source).funcs
#     includes = convert_function_declarations(includes, local_functions)
#
#     ffibuilder = cffi.FFI()
#     ffibuilder.cdef(includes)
#     ffibuilder.set_source(name, source)
#     ffibuilder.compile()
#
#     module = importlib.import_module(name)
#     return module.lib, module.ffi

## replacement plan for pycparser
# inputs a .c file name
# returns a list contains amount of existence of each patterns
def structureExtractor(fileName):
    wdFeature2Interim()
    file = pd.read_csv(fileName)
    wdInterim2Feature()
    tempColumn = 'pureCode'
    file = file[tempColumn]
    patterns = ['struct', 'return', 'int', 'char', '->', '<-', 'void', 'include']
    count = [0] * len(patterns)
    for i in range(len(patterns)):
        pattern = patterns[i]
        for j in range(len(file)):
            if pattern in str(file[j]):
                count[i] += 1
    return count

# no input
# returns a df that stores the structure features
def createStructureDf(fileNames = fileNames, patterns = patterns):
    tempCol = []
    for i in range(len(patterns)):
        tempCol.append('featureS' + str(i + 1))
    structure = pd.DataFrame(columns = tempCol)
    for i in range(len(fileNames)):
        tempList0 = structureExtractor(fileNames[i][:-1] + 'csv')
        structure.loc[i] = tempList0
    return structure

## similarityScoreByWord features
# inputs:
# fileName.c,
# project index[0-3]
# output:
# float
def similarity(fileName, projectName):
    wdFeature2Interim()
    bad_chars = ["*", "/"]
    useless_chars = ['/*', '*/', '*', '\n']
    # separate conditions by projectName
    # project bits
    if projectName == 0:
        documentA = open('bits_FYA.txt', 'r').read()
        documentB = open('bits_TJ.txt', 'r').read()
        documentC = open('bits_CX.txt', 'r').read()
        temp = open('bits_TEMP.txt').read()
        for i in bad_chars:
            temp = temp.replace(i, '')
        temp = temp.split("\n")
        temp = [x.strip(' ') for x in temp]
        temp = [x for x in temp if x != '']

    # project cart_driver
    if projectName == 1:
        documentA = open('cart_driver_FYA.txt', 'r').read()
        documentB = open('cart_driver_TJ.txt', 'r').read()
        documentC = open('cart_driver_CX.txt', 'r').read()
        temp = open('cart_driver_TEMP.txt').read()
        for i in bad_chars:
            temp = temp.replace(i, '')
        temp = temp.split("\n")
        temp = [x.strip(' ') for x in temp]
        temp = [x for x in temp if x != '']

    # project proxy
    if projectName == 2:
        documentA = open('proxy_FYA.txt', 'r').read()
        documentB = open('proxy_TJ.txt', 'r').read()
        documentC = open('proxy_CX.txt', 'r').read()
        temp = open('proxy_TEMP.txt').read()
        for i in bad_chars:
            temp = temp.replace(i, '')
        temp = temp.split("\n")
        temp = [x.strip(' ') for x in temp]
        temp = [x for x in temp if x != '']

    # project tsh
    if projectName == 3:
        documentA = open('tsh_FYA.txt', 'r').read()
        documentB = open('tsh_TJ.txt', 'r').read()
        documentC = open('tsh_CX.txt', 'r').read()
        temp = open('tsh_TEMP.txt').read()
        for i in bad_chars:
            temp = temp.replace(i, '')
        temp = temp.split("\n")
        temp = [x.strip(' ') for x in temp]
        temp = [x for x in temp if x != '']
    wdInterim2Feature()
    # template remove
    for i in temp:
        documentA = documentA.replace(i, '')
        documentB = documentB.replace(i, '')
        documentC = documentC.replace(i, '')
    # useless characters remove
    for i in useless_chars:
        documentA = documentA.replace(i, '')
        documentB = documentB.replace(i, '')
        documentC = documentC.replace(i, '')

    # separate similarity calculation by author name
    vectorizer = TfidfVectorizer()
    if 'FYA' in fileName:
        vectors = vectorizer.fit_transform([documentA, documentB])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities1 = linear_kernel(df[0:1], df).flatten()

        vectors = vectorizer.fit_transform([documentA, documentC])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities2 = linear_kernel(df[0:1], df).flatten()
        return (cosine_similarities1[1] + cosine_similarities2[1]) / 2
    if 'TJ' in fileName:
        vectors = vectorizer.fit_transform([documentB, documentA])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities1 = linear_kernel(df[0:1], df).flatten()

        vectors = vectorizer.fit_transform([documentB, documentC])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities2 = linear_kernel(df[0:1], df).flatten()
        return (cosine_similarities1[1] + cosine_similarities2[1]) / 2
    if 'CX' in fileName:
        vectors = vectorizer.fit_transform([documentC, documentA])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities1 = linear_kernel(df[0:1], df).flatten()

        vectors = vectorizer.fit_transform([documentC, documentB])
        feature_names = vectorizer.get_feature_names()
        dense = vectors.todense()
        denselist = dense.tolist()
        df = pd.DataFrame(denselist, columns=feature_names)
        cosine_similarities2 = linear_kernel(df[0:1], df).flatten()
        return (cosine_similarities1[1] + cosine_similarities2[1]) / 2

# no input
# returns a df that stores the structure features
def createSimilarityByWordDf(fileNames = fileNames):
    tempCol = []
    for i in range(4):
        tempCol.append('featureSimW' + str(i + 1))
    featuresByWord = pd.DataFrame(columns = tempCol)
    for i in range(len(fileNames)):
        tempList0 = []
        for j in range(4):
            tempList0.append(avgScore(fileNames[i], projectName = j))
        featuresByWord.loc[i] = tempList0
    return featuresByWord