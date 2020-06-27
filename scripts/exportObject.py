#!/usr/bin/env python3.7
# -*- coding: utf-8 -*-
"""
Export objects as lines
"""

import bpy

C = bpy.context

for obj in C.selected_objects:
    edges = obj.data.edges
    verts = obj.data.vertices
    for edge in edges:
        print(
            "LINE",
            verts[edge.vertices[1]].co.x,
            verts[edge.vertices[1]].co.y,
            verts[edge.vertices[0]].co.x,
            verts[edge.vertices[0]].co.y,
        )
