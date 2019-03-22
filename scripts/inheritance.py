#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generate Header file with Inheritance Info
"""
# from clang.cindex import TranslationUnit
from typing import Dict, List, Deque
from collections import deque
from clang.cindex import CursorKind
from clang.cindex import Index
from clang.cindex import Config
from os import path
# from graphviz import Digraph

Config.set_library_file('C:\\Program Files\\LLVM\\bin\\libclang.dll')

# dot = Digraph(comment='The Round Table')


def each_class_cursor(cursor):
	# lab = cursor.spelling.decode() + '\n'
	# for i in dir(cursor):
	# 	if i.startswith('is') and getattr(cursor, i)():
	# 		lab += str(i) + ':' + str(getattr(cursor, i)())
	# dot.node(str(id(cursor)), lab)
	for c in cursor.get_children():
		# dot.edge(str(id(cursor)), str(id(c)))
		# print(c.spelling, end=' ')
		if c.kind == CursorKind.CLASS_DECL:
			yield c
		for cls in each_class_cursor(c):
			yield cls


def each_inheritance_relation(cursor):
	for cls in each_class_cursor(cursor):
		for c in cls.get_children():
			if c.kind == CursorKind.CXX_BASE_SPECIFIER:
				yield cls.displayname, c.get_definition().displayname


tags = dict()  # type: Dict[bytes,int]
child = dict()  # type: Dict[bytes,List[bytes]]
parent = dict()  # type: Dict[bytes,bytes]

if __name__ == '__main__':
	# demo parsing the base class and parent class relations
	# cpp_file_path = os.path.join(kInputsDir, 'main.cpp')
	index = Index.create()
	if path.getmtime('include/Shapes.hpp') < path.getmtime('include/Constants.hpp'):
		exit(0)
	tu = index.parse(b'include/Shapes.hpp')
	# print(tu.cursor)
	for this, par in each_inheritance_relation(tu.cursor):
		# print(this, par)
		if this not in child:
			child[this] = []
			parent[this] = None
		if par not in child:
			child[par] = []
			parent[par] = None
		child[par].append(this)
		parent[this] = par

	root = None
	for i in parent:
		if parent[i] is None:
			root = i
	# print(root)

	Q = deque()  # type: Deque[bytes]
	Q.append(root)
	tags[root] = 1

	maxPad = len(bin(tags[root]))

	while Q:
		u = Q.popleft()  # type:bytes
		for index, v in enumerate(child[u]):
			tags[v] = (tags[u] << 4) + index + 1
			maxPad = max(maxPad, len(bin(tags[v])))
			Q.append(v)

	with open('include/Constants.hpp', 'w') as f:
		print('#ifndef CONSTANTS_H', file=f)
		print('#define CONSTANTS_H', file=f)
		print('enum Type {', file=f)
		for i in tags:
			print(i.upper(), tags[i])
			print(i.upper(), '=', tags[i], ',', file=f)
		print('};', file=f)
		print('#endif // CONSTANTS_H', file=f)
