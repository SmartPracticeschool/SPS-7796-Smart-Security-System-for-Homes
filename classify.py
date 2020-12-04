from watson_developer_cloud import VisualRecognitionV3

visual_recognition = VisualRecognitionV3(
    version='DefaultCustomModel_1698588367',
    iam_apikey='LMcK9dZAdQkr1_3_4ckBIOyq8G5kpf0W1Rne61wPK7hQ'
)

import json
from watson_developer_cloud import VisualRecognitionV3

visual_recognition = VisualRecognitionV3(
    '2018-03-19',
    iam_apikey='LMcK9dZAdQkr1_3_4ckBIOyq8G5kpf0W1Rne61wPK7hQ')

with open('./fruitbowl.jpg', 'rb') as images_file:
    classes = visual_recognition.classify(
        images_file,
        threshold='0.6',
	classifier_ids='DefaultCustomModel_1698588367').get_result()
print(json.dumps(classes, indent=2))