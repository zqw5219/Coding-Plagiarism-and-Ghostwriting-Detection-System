from authorizerPackage import *

# apply Random Forest Model

# load features and labels generated before
sSByLine = csv2Df('similarityScoreByLine2.csv')
sSByWord = csv2Df('similarityScoreByWord.csv')
structure = csv2Df('structure.csv')

features = pd.concat([sSByLine.reset_index(drop = True), sSByWord], axis = 1)
features = pd.concat([features.reset_index(drop = True), structure], axis = 1)
# deal with dummy variables
features = pd.get_dummies(features)
# save labels
label1 = np.array(features['projectType_bits'])
label2 = np.array(features['projectType_cart'])
label3 = np.array(features['projectType_proxy'])
label4 = np.array(features['projectType_tsh'])
label5 = np.array(features['authorName_CX'])
label6 = np.array(features['authorName_FYA'])
label7 = np.array(features['authorName_TEMP'])
label8 = np.array(features['authorName_TJ'])

# save label names
labelNames = features.columns[-8:]
# Remove the labels from the features
for i in labelNames:
    features = features.drop(i, axis = 1)
# Save feature names
featureNames = list(features.columns)
# Convert to numpy array
features = np.array(features)

# use sklearn library for further analyze
# including split training and testing sets
# and model training

# each line predicts a single treatment in the following list (labelNames):
#        'projectType_bits',
#        'projectType_cart',
#        'projectType_proxy',
#        'projectType_tsh',
#        'authorName_CX',
#        'authorName_FYA',
#        'authorName_TEMP',
#        'authorName_TJ'
TrainModel(features, label1, 'projectType_bits')
TrainModel(features, label2, 'projectType_cart')
TrainModel(features, label3, 'projectType_proxy')
TrainModel(features, label4, 'projectType_tsh')
TrainModel(features, label5, 'authorName_CX')
TrainModel(features, label6, 'authorName_FYA')
TrainModel(features, label7, 'authorName_TEMP')
TrainModel(features, label8, 'authorName_TJ')