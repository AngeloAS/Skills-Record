import scipy.io as scio
import os
import cv2 as cv
import numpy


def distance(point1, point2):
    return numpy.sqrt(numpy.sum((point1 - point2) ** 2))


mat_dict_path = "F:/BaiduNetdiskDownload/part_A_final/test_data/ground_truth"
pict_dict_path = "F:/BaiduNetdiskDownload/part_A_final/test_data/images"
mat_dict = os.listdir(mat_dict_path)
pict_dict = os.listdir(pict_dict_path)
for mat_id in range(0,len(mat_dict)):
    mat_path = os.path.join(mat_dict_path, mat_dict[mat_id])
    mat = scio.loadmat(mat_path)
    mat_point_all = mat["image_info"]
    point_all = numpy.array(mat_point_all[0][0])[0, 0][0].astype(int)

    # 找到mat文件对应的picture
    mat_name = mat_dict[mat_id].split('.')
    mat_name_front = mat_name[0]
    pict_name_front = mat_name_front[3:]
    pict_name = pict_name_front + ".jpg"
    pict_path = os.path.join(pict_dict_path,pict_name)
    pict = cv.imread(pict_path)
    '''
    # 画点图
    for point_id in range(0,len(point_all)):
        point_x = point_all[point_id][0]
        point_y = point_all[point_id][1]
        cv.circle(pict, (point_x, point_y), 5, (255, 255, 0), 1, 8, 0);
    cv.imshow("image", pict)
    cv.waitKey(0)
    '''
    for point_id in range(0, len(point_all)):
        # 遍历寻找最短欧氏距离
        min_euclid = float('inf')
        min_euclid_index = 0
        for point_index in range(0, len(point_all)):
            if point_id == point_index:
                continue
            length = distance(point_all[point_id], point_all[point_index])
            if length < min_euclid:
                min_euclid = length
                min_euclid_index = point_index
        mid_point = (point_all[point_id] + point_all[min_euclid_index])/2
        print(point_all[point_id], point_all[min_euclid_index], mid_point)
        overshadow = max(abs(mid_point[0]-point_all[point_id][0]), abs(mid_point[1]-point_all[point_id][1]))
        xmin = point_all[point_id][0]-overshadow
        xmax = point_all[point_id][0]+overshadow
        ymin = point_all[point_id][1]-overshadow
        ymax = point_all[point_id][1]+overshadow
        cv.rectangle(pict, (xmin.astype(int), ymin.astype(int)), (xmax.astype(int), ymax.astype(int)), (0, 0, 255), 2)
    cv.imshow("image", pict)
    cv.waitKey(0)





