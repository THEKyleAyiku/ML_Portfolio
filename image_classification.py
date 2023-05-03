# -*- coding: utf-8 -*-
"""Image Classification.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1zg_3P0K8Hxuf4yVtLn9_HEd5dzGJvBSy
"""

# Reading in the data set.
from google.colab import files
uploaded = files.upload()
import io
import numpy as np
import pandas as pd
import keras
import os
import tensorflow as tf

import matplotlib.pyplot as plt
data = pd.read_csv(io.BytesIO(uploaded['wonders_of_world_images.csv']))

# Dividing the data into train and test.
(x_train, y_train), ()