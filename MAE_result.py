import os
from voc_eval import *
import numpy as np
import collections

path_label = r'/home/qianzhihua/VOC_result/VOC_result/labels_pose'
ann = r'/home/qianzhihua/VOC_result/VOC_result/test_ano'

files = os.listdir(path_label)
num = len(files)
recs = {}
C = 0
C1 = 0
C2 = 0
C3 = 0
XX = 0
XX1 = 0
XX2 = 0
XX3 = 0
class1 = 'standing'
class2 = 'sitting'
class3 = 'lying'
for file in files:
    xml_name = file.replace('_hist.txt', '')
    xml_file = file.replace('txt', 'xml').replace('_hist', '')
    recs[xml_name] = parse_rec(os.path.join(ann, xml_file))
    X = len([obj['name'] for obj in recs[xml_name]])
    X_class1 = len([obj['name'] for obj in recs[xml_name] if obj['name'] == class1])
    X_class2 = len([obj['name'] for obj in recs[xml_name] if obj['name'] == class2])
    X_class3 = len([obj['name'] for obj in recs[xml_name] if obj['name'] == class3])
    XX = XX + X
    XX1 = XX1 + X_class1
    XX2 = XX2 + X_class2
    XX3 = XX3 + X_class3

    with open(os.path.join(path_label, file)) as f_label:
        label = collections.Counter(f_label)
        F_class1 = label['1\n']
        F_class2 = label['2\n']
        F_class3 = label['3\n']
        F = F_class1 + F_class2 + F_class3
    C = C + abs(X-F)
    C1 = C1 + abs(X_class1 - F_class1)
    C2 = C2 + abs(X_class2 - F_class2)
    C3 = C3 + abs(X_class3 - F_class3)

# MAEALL = float(C/num)
# print(MAEALL)
MAE1 = float(C1/num)
MAE2 = float(C2/num)
MAE3 = float(C3/num)

print("\n")
print("total MAE: ", float((C1+C2+C3)/num))
print("standing MAE: ", MAE1)
print("sitting MAE: ", MAE2)
print("lying MAE: ", MAE3)
print("\n")

# B1 = float(C1/XX1)
# B2 = float(C2/XX2)
# B3 = float(C3/XX3)
# print(B1)
# print(B2)
# print(B3)
