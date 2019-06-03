import os
import cv2
import numpy as np
from xml.dom.minidom import Document
try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

# pict_dir_path = input("Please input original pictures folder path:")
# xml_dir_path = input("Please input original xml folder path:")
# pict_save_path = input("Please input pictures cut save folder path:")
pict_dir_path = r"/home/qianzhihua/PycharmProjects/association/pict"
xml_dir_path = r"/home/qianzhihua/PycharmProjects/association/xml"
pict_save_path = r"/home/qianzhihua/PycharmProjects/association/pict_save"

xml_list = os.listdir(xml_dir_path)


# make dir to save different kid of posture
# sitting, standing, groveling/lying, talking, others
num = np.array([0, 0, 0, 0, 0])
posture = ["sitting", "standing", "groveling", "talking", "others"]
for dir_id in range(0, len(posture)):
    if os.path.exists(pict_save_path + "/" + posture[dir_id]) is False:
        os.mkdir(pict_save_path + "/" + posture[dir_id])
else:
    num[dir_id]= len(os.listdir(pict_save_path + "/" + posture[dir_id]))
for xml_id in range(0, len(xml_list)):
    xml_name = xml_list[xml_id].split('.')
    xml_front = xml_name[0]
    pict_full_name = xml_front + ".jpg"
    pict_path = pict_dir_path + "/" + pict_full_name
    xml_path = xml_dir_path + "/" + xml_list[xml_id]
    if os.path.exists(pict_path) is True:
        pict = cv2.imread(pict_path)
        tree = ET.ElementTree(file=xml_path)
        root = tree.getroot()
        for label in root.findall('object'):
             label_name = label[0].text
             x1 = int(label[4][0].text)
             y1 = int(label[4][1].text)
             x2 = int(label[4][2].text)
             y2 = int(label[4][3].text)
             pict_save = pict[y1:y2, x1:x2]
             for kid in range(0, len(posture)):
                 if label_name == posture[kid]:
                     num[kid] = num[kid] + 1
                     cv2.imwrite(pict_save_path + "/" + posture[kid] + "/" + str(num[kid]) + ".jpg", pict_save)







