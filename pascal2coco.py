# -*- coding:utf-8 -*-
# !/usr/bin/env python
'''
此代码用于将VOC数据集格式转化为COCO数据集格式
此代码需要cocoAPI,如果是本地路径，需要编译下cocoAPI
但需注意：生成后train.json、val.json、test.json的categories需要类别顺序一致，如果不一致的话训练标签会乱，需要修改成一致的
'''
import argparse
import json
import matplotlib.pyplot as plt
import cv2
from labelme import utils
import numpy as np
import glob
import PIL.Image
import os,sys
 
 
class PascalVOC2coco(object):
    def __init__(self, xml=[], save_json_path='./new.json'):
        '''
        :param xml: ����Pascal VOC��xml�ļ�·����ɵ��б�
        :param save_json_path: json����λ��
        '''
        self.xml = xml
        self.save_json_path = save_json_path
        self.images = []
        self.categories = []
        self.annotations = []
        # self.data_coco = {}
        self.label = []
        self.annID = 1
        self.height = 0
        self.width = 0
        self.ob = []
        self.supercategory = "standing"
        if self.supercategory not in self.label:
            self.categories.append(self.categorie())
            self.label.append(self.supercategory)
        self.supercategory = "sitting"
        if self.supercategory not in self.label:
            self.categories.append(self.categorie())
            self.label.append(self.supercategory)
        self.supercategory = "lying"
        if self.supercategory not in self.label:
            self.categories.append(self.categorie())
            self.label.append(self.supercategory)
 
        self.save_json()
 
    def data_transfer(self):
        for num, json_file in enumerate(self.xml):
 
            # �������
            sys.stdout.write('\r>> Converting image %d/%d' % (
                num + 1, len(self.xml)))
            sys.stdout.flush()
 
            self.json_file = json_file
            #print("self.json", self.json_file)
            self.num = num 
            #print(self.num)
            path = os.path.dirname(self.json_file)
            #print(path)
            path = os.path.dirname(path)
            #print(path)
            # path=os.path.split(self.json_file)[0]
            # path=os.path.split(path)[0]
            obj_path = glob.glob(os.path.join(path, 'SegmentationObject', '*.png'))
            #print(obj_path)
            with open(json_file, 'r') as fp:
                #print(fp)
                flag = 0
                for p in fp:
                    #print(p)
                    # if 'folder' in p:
                    #     folder =p.split('>')[1].split('<')[0]
                    f_name = 1
                    if '<filename>' in p:
                        self.filen_ame = p.split('>')[1].split('<')[0]
                        #print(self.filen_ame)
                        f_name = 0
 
                        self.path = os.path.join(path, 'SegmentationObject', self.filen_ame.split('.')[0] + '.png')
                        #if self.path not in obj_path:
                        #    break
 
 
                    if '<width>' in p:
                        self.width = int(p.split('>')[1].split('<')[0])
                        #print(self.width)
                    if '<height>' in p:
                        self.height = int(p.split('>')[1].split('<')[0])
 
                        self.images.append(self.image())
                        #print(self.image())
 
                    if flag == 1:
                        self.supercategory = self.ob[0]
                        if self.supercategory not in self.label:
                            self.categories.append(self.categorie())
                            print(self.supercategory)
                            self.label.append(self.supercategory)
 
                        # �߽��
                        x1 = int(float(self.ob[-4]))
                        y1 = int(float(self.ob[-3]))
                        x2 = int(float(self.ob[-2]))
                        y2 = int(float(self.ob[-1]))
                        self.rectangle = [x1, y1, x2, y2]
                        self.bbox = [x1, y1, x2 - x1, y2 - y1]  # COCO ��Ӧ��ʽ[x,y,w,h]
                        self.area = abs((float(self.ob[-4])-float(self.ob[-2]))*(float(self.ob[-3])-float(self.ob[-1])))
 
                        self.annotations.append(self.annotation())
                        self.annID += 1
                        self.ob = []
                        flag = 0
                    elif f_name == 1:
                        if '<name>' in p and '<name>xiaoxianyu' not in p:
                            self.ob.append(p.split('>')[1].split('<')[0])
 
                        if '<xmin>' in p:
                            self.ob.append(p.split('>')[1].split('<')[0])
 
                        if '<ymin>' in p:
                            self.ob.append(p.split('>')[1].split('<')[0])
 
                        if '<xmax>' in p:
                            self.ob.append(p.split('>')[1].split('<')[0])
 
                        if '<ymax>' in p:
                            self.ob.append(p.split('>')[1].split('<')[0])
                            flag = 1
 
                    '''
                    if '<object>' in p:
                        # ���
                        print(next(fp))
                        d = [next(fp).split('>')[1].split('<')[0] for _ in range(7)]
                        self.supercategory = d[0]
                        if self.supercategory not in self.label:
                            self.categories.append(self.categorie())
                            self.label.append(self.supercategory)
                        # �߽��
                        x1 = int(d[-4]);
                        y1 = int(d[-3]);
                        x2 = int(d[-2]);
                        y2 = int(d[-1])
                        self.rectangle = [x1, y1, x2, y2]
                        self.bbox = [x1, y1, x2 - x1, y2 - y1]  # COCO ��Ӧ��ʽ[x,y,w,h]
                        self.annotations.append(self.annotation())
                        self.annID += 1
                     '''
 
        sys.stdout.write('\n')
        sys.stdout.flush()
 
    def image(self):
        image = {}
        image['height'] = self.height
        image['width'] = self.width
        image['id'] = self.num + 1
        image['file_name'] = self.filen_ame
        return image
 
    def categorie(self):
        categorie = {}
        categorie['supercategory'] = self.supercategory
        categorie['id'] = len(self.label) + 1  # 0 Ĭ��Ϊ����
        categorie['name'] = self.supercategory
        return categorie

    def annotation(self):
        annotation = {}
        # annotation['segmentation'] = [self.getsegmentation()]
        annotation['segmentation'] = [list(map(float, self.getsegmentation()))]
        annotation['iscrowd'] = 0
        annotation['image_id'] = self.num + 1
        # annotation['bbox'] = list(map(float, self.bbox))
        annotation['bbox'] = self.bbox
        annotation['category_id'] = self.getcatid(self.supercategory)
        annotation['id'] = self.annID
        annotation['area']=self.area
        return annotation

    def getcatid(self, label):
        for categorie in self.categories:
            if label == categorie['name']:
                return categorie['id']
        return -1
 
    def getsegmentation(self):
 
        try:
            mask_1 = cv2.imread(self.path, 0)
            mask = np.zeros_like(mask_1, np.uint8)
            rectangle = self.rectangle
            mask[rectangle[1]:rectangle[3], rectangle[0]:rectangle[2]] = mask_1[rectangle[1]:rectangle[3],
                                                                         rectangle[0]:rectangle[2]]
 
            # ��������е�����ֵ
            mean_x = (rectangle[0] + rectangle[2]) // 2
            mean_y = (rectangle[1] + rectangle[3]) // 2
 
            end = min((mask.shape[1], int(rectangle[2]) + 1))
            start = max((0, int(rectangle[0]) - 1))
 
            flag = True
            for i in range(mean_x, end):
                x_ = i;
                y_ = mean_y
                pixels = mask_1[y_, x_]
                if pixels != 0 and pixels != 220:
                    mask = (mask == pixels).astype(np.uint8)
                    flag = False
                    break
            if flag:
                for i in range(mean_x, start, -1):
                    x_ = i;
                    y_ = mean_y
                    pixels = mask_1[y_, x_]
                    if pixels != 0 and pixels != 220:
                        mask = (mask == pixels).astype(np.uint8)
                        break
            self.mask = mask
 
            return self.mask2polygons()
 
        except:
            return [0]
 
    def mask2polygons(self):
        contours = cv2.findContours(self.mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        bbox=[]
        for cont in contours[1]:
            [bbox.append(i) for i in list(cont.flatten())]
        return bbox
 
    # '''
    def getbbox(self, points):
        # img = np.zeros([self.height,self.width],np.uint8)
        # cv2.polylines(img, [np.asarray(points)], True, 1, lineType=cv2.LINE_AA)  # ���߽���
        # cv2.fillPoly(img, [np.asarray(points)], 1)  # ������� �ڲ�����ֵΪ1
        polygons = points
        mask = self.polygons_to_mask([self.height, self.width], polygons)
        return self.mask2box(mask)
 
    def mask2box(self, mask):
        index = np.argwhere(mask == 1)
        rows = index[:, 0]
        clos = index[:, 1]
        left_top_r = np.min(rows)  # y
        left_top_c = np.min(clos)  # x
 
        # �������½����к�
        right_bottom_r = np.max(rows)
        right_bottom_c = np.max(clos)
 
        # return [(left_top_r,left_top_c),(right_bottom_r,right_bottom_c)]
        # return [(left_top_c, left_top_r), (right_bottom_c, right_bottom_r)]
        # return [left_top_c, left_top_r, right_bottom_c, right_bottom_r]  # [x1,y1,x2,y2]
        return [left_top_c, left_top_r, right_bottom_c - left_top_c,
                right_bottom_r - left_top_r]
 
    def polygons_to_mask(self, img_shape, polygons):
        mask = np.zeros(img_shape, dtype=np.uint8)
        mask = PIL.Image.fromarray(mask)
        xy = list(map(tuple, polygons))
        PIL.ImageDraw.Draw(mask).polygon(xy=xy, outline=1, fill=1)
        mask = np.array(mask, dtype=bool)
        return mask
 
    # '''
    def data2coco(self):
        data_coco = {}
        data_coco['images'] = self.images
        data_coco['categories'] = self.categories
        data_coco['annotations'] = self.annotations
        return data_coco
 
    def save_json(self):
        self.data_transfer()
        self.data_coco = self.data2coco()
        json.dump(self.data_coco, open(self.save_json_path, 'w'), indent=4)  # indent=4 ����������ʾ

xml_file = list()
MAML_dir_name = ["b1", "b2", "m1", "m2", "s1", "s2", "s3", "t1", "w1", "w2", "w3", "w4", "w5", "w5", "w6", "w7", "w8",
                 "w9", "w10"]
basedir = r'G:\论文数据集1\0903MAML'
savedir = r'G:\论文数据集1\0903MAMLCOCO'
Annotations = r'C:\Users\AngeloAS\Desktop\Annotations'  # 所有XML文件所在文件夹
for M in MAML_dir_name:
    dirs = os.path.join(basedir, M)
    _savedir = os.path.join(savedir, M)  # 保存各个小场景数据集的位置
    traindir = os.path.join(dirs, "train")
    testdir = os.path.join(dirs, "test")
    trainjpg = os.listdir(traindir)
    trainxml = [_.split(".")[0] + ".xml" for _ in trainjpg]
    for xml in trainxml:
        xml_file.append(os.path.join(Annotations,xml))
    PascalVOC2coco(xml_file, os.path.join(_savedir, "train.json"))  # 生成训练集

    xml_file.clear()
    testjpg = os.listdir(testdir)
    testxml = [_.split(".")[0] + ".xml" for _ in testjpg]
    for xml in testxml:
        xml_file.append(os.path.join(Annotations, xml))
    PascalVOC2coco(xml_file, os.path.join(_savedir, "test.json"))  # 生成测试集

    xml_file.clear()
    valjpg = os.listdir(testdir)
    xml = [_.split(".")[0] + ".xml" for _ in valjpg]
    for xml in testxml:
        xml_file.append(os.path.join(Annotations, xml))
    PascalVOC2coco(xml_file, os.path.join(_savedir, "val.json"))  # 生成测试集