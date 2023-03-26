import sys
import os
import numpy as np
import torch
import cv2
import glob

parent_dir = os.path.abspath(os.path.join(os.getcwd(), '..'))
sys.path.insert(0, parent_dir + "/MiDaS")

import utils
from midas.dpt_depth import DPTDepthModel
from midas.model_loader import load_model
from midas.transforms import Resize
from torchvision.transforms import Compose


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print("Device: %s" % device)

model, transform, net_w, net_h = load_model(device, "C:/Users/Flourek/CLionProjects/Stereorizer/weights"
                                                    "/dpt_beit_large_512.pt", model_type="dpt_beit_large_512",
                                                     optimize=False,  height=None, square=False)
