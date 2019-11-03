# Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
from __future__ import division
import os

import numpy
from io import BytesIO
import cv2 as cv

from matplotlib import pyplot

import requests
import torch
from torch.jit import ScriptModule, script_method, trace, Tensor
from PIL import Image
from maskrcnn_benchmark.config import cfg
from predictor import COCODemo
from maskrcnn_benchmark.structures.image_list import ImageList

if __name__ == "__main__":
    # load config from file and command-line arguments

    project_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cfg.merge_from_file(
        os.path.join(project_dir,
                     "configs/e2e_faster_rcnn_R_50_FPN_1x.yaml"))
    cfg.merge_from_list(["MODEL.DEVICE", "cuda"])
    # cfg.merge_from_list(["MODEL.DEVICE", "cpu"])
    cfg.freeze()

    # prepare object that handles inference plus adds predictions on top of image
    coco_demo = COCODemo(
        cfg,
        confidence_threshold=0.5,
        show_mask_heatmaps=False,
        masks_per_dim=2,
        min_image_size=960,
    )


def single_image_to_top_predictions(image):
    image = image.float() / 255.0
    image = image.permute(2, 0, 1)
    # we are loading images with OpenCV, so we don't need to convert them
    # to BGR, they are already! So all we need to do is to normalize
    # by 255 if we want to convert to BGR255 format, or flip the channels
    # if we want it to be in RGB in [0-1] range.
    if cfg.INPUT.TO_BGR255:
        image = image * 255
    else:
        image = image[[2, 1, 0]]

    # we absolutely want fixed size (int) here (or we run into a tracing error (or bug?)
    # or we might later decide to make things work with variable size...
    image = image - torch.tensor(cfg.INPUT.PIXEL_MEAN)[:, None, None]
    # should also do variance...
    image_list = ImageList(image.unsqueeze(0), [(int(image.size(-2)), int(image.size(-1)))])
    image_list = image_list.to(coco_demo.device)
    results = coco_demo.model(image_list)
    results = [o.to(coco_demo.cpu_device) for o in results]
    result = results[0]
    scores = result.get_field("scores")
    keep = (scores >= coco_demo.confidence_threshold)
    result = (result.bbox[keep],
              result.get_field("labels")[keep],
              # result.get_field("mask")[keep],
              scores[keep])

    return result


def fetch_image(url):
    response = requests.get(url)
    return Image.open(BytesIO(response.content)).convert("RGB")


if __name__ == "__main__":
    pil_image = Image.open("006167.jpg").convert("RGB")
    pil_image = pil_image.resize((1312, 960), Image.BILINEAR)

    # convert to BGR format
    image = torch.from_numpy(numpy.array(pil_image)[:, :, [2, 1, 0]])
    original_image = image


    for p in coco_demo.model.parameters():
        p.requires_grad_(False)
    resss = single_image_to_top_predictions(image)
    print(resss[0])
    print(resss[0].size())
    traced_model = torch.jit.trace(single_image_to_top_predictions, (image,), optimize=False)
    traced_model.save('model2_cpu.pt')

    image3 = Image.open("006167.jpg").convert("RGB")
    # image3 = Image.open("/data/tanglin/peoplecount-trace/maskrcnn-benchmark/demo/non_object/01881_hist.jpg")
    image3 = image3.resize((1312, 960), Image.BILINEAR)
    image3 = torch.from_numpy(numpy.array(image3)[:, :, [2, 1, 0]])

    loaded2 = torch.jit.load("model2_cpu.pt")
    result_image4 = loaded2(image3)
    print(result_image4[0])

    # try:
    #     result_image4 = loaded2(image3)
    #     # print('no objects')
    # except RuntimeError:
    #     result_image4 = torch.Tensor([])


