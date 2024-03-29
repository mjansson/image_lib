#!/usr/bin/env python

"""Ninja build configurator for image library"""

import sys
import os

sys.path.insert(0, os.path.join('build', 'ninja'))

import generator

dependlibs = ['image', 'foundation']

generator = generator.Generator(project = 'image', dependlibs = dependlibs, variables = [('bundleidentifier', 'com.maniccoder.image.$(binname)')])
target = generator.target
writer = generator.writer
toolchain = generator.toolchain
extrasources = []

image_sources = ['freeimage.c', 'image.c', 'version.c']

image_lib = generator.lib(module = 'image', sources = image_sources + extrasources)

#if not target.is_ios() and not target.is_android() and not target.is_tizen():
#  configs = [config for config in toolchain.configs if config not in ['profile', 'deploy']]
#  if not configs == []:
#    generator.bin('bin2hex', ['main.c'], 'bin2hex', basepath = 'tools', implicit_deps = [foundation_lib], libs = ['foundation'], configs = configs)

if generator.skip_tests():
  sys.exit()

includepaths = generator.test_includepaths()

test_cases = []
if toolchain.is_monolithic() or target.is_ios() or target.is_android() or target.is_tizen():
  #Build one fat binary with all test cases
  test_resources = []
  test_extrasources = []
  test_cases += ['all']
  if target.is_ios():
    test_resources = [os.path.join('all', 'ios', item) for item in [
      'test-all.plist', 'Images.xcassets', 'test-all.xib'
    ]]
    test_extrasources = [os.path.join('all', 'ios', item) for item in [
      'viewcontroller.m'
    ]]
  elif target.is_android():
    test_resources = [os.path.join('all', 'android', item) for item in [
      'AndroidManifest.xml', os.path.join('layout', 'main.xml'), os.path.join('values', 'strings.xml'),
      os.path.join('drawable-ldpi', 'icon.png'), os.path.join('drawable-mdpi', 'icon.png'), os.path.join('drawable-hdpi', 'icon.png'),
      os.path.join('drawable-xhdpi', 'icon.png'), os.path.join('drawable-xxhdpi', 'icon.png'), os.path.join('drawable-xxxhdpi', 'icon.png')
    ]]
    test_extrasources = [os.path.join('test', 'all', 'android', 'java', 'com', 'maniccoder', 'image', 'test', item) for item in [
      'TestActivity.java'
    ]]
  elif target.is_tizen():
    test_resources = [os.path.join('all', 'tizen', item) for item in [
      'tizen-manifest.xml', os.path.join('res', 'tizenapp.png')
    ]]
  sources = [os.path.join(module, 'main.c') for module in test_cases] + test_extrasources
  dependlibs = ['test'] + dependlibs
  if target.is_ios() or target.is_android() or target.is_tizen():
    generator.app(module = '', sources = sources, binname = 'test-image', basepath = 'test', implicit_deps = [image_lib], libs = dependlibs, dependlibs = dependlibs, resources = test_resources, includepaths = includepaths)
  else:
    generator.bin(module = '', sources = sources, binname = 'test-image', basepath = 'test', implicit_deps = [image_lib], libs = dependlibs, dependlibs = dependlibs, includepaths = includepaths)
else:
  sources = ['main.c']
  #Build one binary per test case
  if not generator.is_subninja:
    generator.bin(module = 'all', sources = sources, binname = 'test-all', basepath = 'test', implicit_deps = [image_lib], libs = dependlibs, dependlibs = dependlibs, includepaths = includepaths)
  dependlibs = ['test'] + dependlibs
  for test in test_cases:
    generator.bin(module = test, sources = sources, binname = 'test-' + test, basepath = 'test', implicit_deps = [image_lib], libs = dependlibs, dependlibs = dependlibs, includepaths = includepaths)
