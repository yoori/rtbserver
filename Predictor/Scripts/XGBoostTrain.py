#!/usr/bin/env python

import pandas as pd
import xgboost as xgb
from sklearn.metrics import log_loss
from sklearn.datasets import load_svmlight_file
from sklearn.svm import SVC
#import matplotlib.pyplot as plt
import numpy as np
#import graphviz
import argparse

class MatrixReader:
  @staticmethod
  def read(file, labelcol) :
    print 'to read "' + file + '"'
    if file.endswith('.libsvm') :
      data, labels = load_svmlight_file(file) # xgb.DMatrix(args.train_file[0])
    else :
      train_df = pd.read_csv(args.train_file[0], header=0)
      full_matrix = train_df.as_matrix()
      labels = full_matrix[:, labelcol] 
      data = np.delete(full_matrix, args.labelcol, 1)
    print 'from read "' + file + '"'
    return data, labels

parser = argparse.ArgumentParser(description='xgboost train util.')

parser.add_argument(
  'model_file',
  metavar='<model file>',
  type=str,
  nargs=1,
  help='model out file')

parser.add_argument(
  'train_file',
  metavar='<train file>',
  type=str,
  nargs=1,
  help='train file')

parser.add_argument(
  'test_file',
  metavar='<test file>',
  type=str,
  nargs='*',
  help='test file')

parser.add_argument(
  '--labelcol',
  type=int,
  nargs='?',
  default=0,
  help='label column index')

parser.add_argument(
  '-n',
  dest='num_rounds',
  type=int,
  nargs='?',
  default=10,
  help='label column index')

parser.add_argument(
  '-d',
  dest='max_depth',
  type=int,
  nargs='?',
  default=5,
  help='max model depth')

parser.add_argument(
  '--seed',
  dest='seed',
  type=int,
  nargs='?',
  default=1234,
  help='seed')

parser.add_argument(
  '--eta',
  dest='eta',
  type=float,
  nargs='?',
  default=1,
  help='eta train argument')

parser.add_argument(
  '--lambda',
  dest='plambda',
  type=float,
  nargs='?',
  default=0,
  help='lambda train argument')

parser.add_argument(
  '--alpha',
  dest='alpha',
  type=float,
  nargs='?',
  default=0,
  help='alpha train argument')

parser.add_argument(
  '--lambda-bias',
  dest='lambda_bias',
  type=float,
  nargs='?',
  default=0,
  help='lambda_bias train argument')

parser.add_argument(
  '--type',
  dest='type',
  type=str,
  nargs='?',
  default='xgbtree',
  help='model type')

args = parser.parse_args()
#print args

# load file from text file, also binary buffer generated by xgboost
train_matrix, train_labels = MatrixReader.read(args.train_file[0], args.labelcol)
train = xgb.DMatrix(train_matrix, train_labels);

tests = []

for test_file in args.test_file :
  test_matrix, test_labels = MatrixReader.read(test_file, args.labelcol)
  tests.append(xgb.DMatrix(test_matrix, test_labels))

if (args.type == 'xgbtree') or (args.type == 'xgblinear') :
  param =  {
    'booster': ('gblinear' if args.type == 'xgblinear' else 'gbtree'),
    'max_depth': args.max_depth, #'max_depth':19,
    'eta': args.eta, #'eta':0.87359697667115777,
    'lambda': args.plambda,
    'alpha': args.alpha,
    'lambda_bias': args.lambda_bias,
    'silent': True,
    'nthread': 1,
    #'gamma': 0.54712756519884242,
    #'min_child_weight': 9,
    #'max_delta_step':2.1356713625514594,
    #'subsample': 0.7812377557204605,
    'eval_metric': 'logloss',
    #'colsample_bytree': 0.73772915728179478,
    'seed': args.seed,
    'objective':"reg:logistic"}

  # specify validations set to watch performance
  watchlist = [(train, 'train')]
  for i, cur_test in enumerate(tests) :
    watchlist.append((cur_test, 'test#' + str(i)))

  # first loop for find best iteration
  bst_check = xgb.train(param, train, args.num_rounds, watchlist, early_stopping_rounds=100)

  # repeat train for best iteration
  bst = xgb.train(param, train, bst_check.best_iteration + 1, watchlist)

  # save model
  bst.save_model(args.model_file[0])

  if len(tests) > 0 :
    preds = bst.predict(tests[0])
    res_log_loss = log_loss(tests[0].get_label(), preds)
    print("LOGLOSS: " + str(res_log_loss))

else :
  print("Unknown type: " + str(args.type))
  booster = SVC()
  booster.fit(train_matrix, train_labels)
