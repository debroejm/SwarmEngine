# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8-80 compliant>

bl_info = {
    "name": "mEng MMD Format",
    "author": "Jamed De Broeck - original OBJ by Campbell Barton",
    "blender": (2, 6, 8),
    "version": (0, 0, 1),
    "location": "File > Import-Export",
    "description": "Export MMD file format",
    "category": "Import-Export"
}

if "bpy" in locals():
    import imp
    if "export_mmd" in locals():
        imp.reload(export_mmd)


import bpy
from bpy.props import (BoolProperty,
                       FloatProperty,
                       StringProperty,
                       EnumProperty,
                       )
from bpy_extras.io_utils import (ImportHelper,
                                 ExportHelper,
                                 path_reference_mode,
                                 axis_conversion,
                                 )

class ExportMMD(bpy.types.Operator, ExportHelper):
    """Save a mEng MMD File"""

    bl_idname = "export_scene.mmd"
    bl_label = 'Export MMD'
    bl_options = {'PRESET'}

    filename_ext = ".mmd"
    filter_glob = StringProperty(
            default="*.mmd",
            options={'HIDDEN'},
            )

    # context group
    use_selection = BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=False,
            )

    # extra data group
    use_normals = BoolProperty(
            name="Include Normals",
            description="",
            default=True,
            )
    use_uvs = BoolProperty(
            name="Include UVs",
            description="Write out the active UV coordinates",
            default=True,
            )
    use_bones = BoolProperty(
            name="Include Bones",
            description="Write out the active Bones",
            default=False,
            )
    use_bonedvertices = BoolProperty(
            name="Tag Vertices",
            description="Attach bone IDs to each Vertex",
            default=False,
            )
    use_triangles = BoolProperty(
            name="Triangulate Faces",
            description="Convert all faces to triangles",
            default=True,
            )

    # grouping group
    keep_vertex_order = BoolProperty(
            name="Keep Vertex Order",
            description="",
            default=False,
            )

    axis_forward = EnumProperty(
            name="Forward",
            items=(('X', "X Forward", ""),
                   ('Y', "Y Forward", ""),
                   ('Z', "Z Forward", ""),
                   ('-X', "-X Forward", ""),
                   ('-Y', "-Y Forward", ""),
                   ('-Z', "-Z Forward", ""),
                   ),
            default='-Z',
            )
    axis_up = EnumProperty(
            name="Up",
            items=(('X', "X Up", ""),
                   ('Y', "Y Up", ""),
                   ('Z', "Z Up", ""),
                   ('-X', "-X Up", ""),
                   ('-Y', "-Y Up", ""),
                   ('-Z', "-Z Up", ""),
                   ),
            default='Y',
            )
    global_scale = FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    path_mode = path_reference_mode

    check_extension = True

    def execute(self, context):
        from . import export_mmd

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = (Matrix.Scale(self.global_scale, 4) *
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4())

        keywords["global_matrix"] = global_matrix
        return export_mmd.save(self, context, **keywords)


def menu_func(self, context):
    self.layout.operator(ExportMMD.bl_idname, text="mEng (.mmd)")


def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)


def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()