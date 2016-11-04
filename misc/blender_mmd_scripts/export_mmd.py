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

# <pep8 compliant>

import os
import time

import bpy
import mathutils
import bpy_extras.io_utils


def name_compat(name):
    if name is None:
        return 'None'
    else:
        return name.replace(' ', '_')


def mesh_triangulate(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()


def write_file(filepath, objects, scene,
               EXPORT_TRI=True,
               EXPORT_NORMALS=True,
               EXPORT_UV=True,
               EXPORT_BONES=False,
               EXPORT_BVERTS=False,
               EXPORT_KEEP_VERT_ORDER=False,
               EXPORT_GLOBAL_MATRIX=None,
               EXPORT_PATH_MODE='AUTO',
               ):
    """
    Basic write function. The context and options must be already set
    This can be accessed externaly
    eg.
    write( 'c:\\test\\foobar.obj', Blender.Object.GetSelected() ) # Using default options.
    """

    if EXPORT_GLOBAL_MATRIX is None:
        EXPORT_GLOBAL_MATRIX = mathutils.Matrix()

    def veckey3d(v):
        return round(v.x, 6), round(v.y, 6), round(v.z, 6)

    def veckey2d(v):
        return round(v[0], 6), round(v[1], 6)

    def findVertexGroupName(face, vWeightMap):
        """
        Searches the vertexDict to see what groups is assigned to a given face.
        We use a frequency system in order to sort out the name because a given vetex can
        belong to two or more groups at the same time. To find the right name for the face
        we list all the possible vertex group names with their frequency and then sort by
        frequency in descend order. The top element is the one shared by the highest number
        of vertices is the face's group
        """
        weightDict = {}
        for vert_index in face.vertices:
            vWeights = vWeightMap[vert_index]
            for vGroupName, weight in vWeights:
                weightDict[vGroupName] = weightDict.get(vGroupName, 0.0) + weight

        if weightDict:
            return max((weight, vGroupName) for vGroupName, weight in weightDict.items())[1]
        else:
            return '(null)'
    
    print('OBJ Export path: %r' % filepath)

    time1 = time.time()

    file = open(filepath, "w", encoding="utf8", newline="\n")
    fw = file.write
    
    def writeBoneChildren(currentIndex, currentBone):
        if not (currentBone.children == None):
            tIndex = currentIndex + 1
            childCount = 0
            for tB in currentBone.children:
                if 'LIMB_LEG' in tB.name:
                    fw('jl ')
                elif 'LIMB_ARM' in tB.name:
                    fw('ja ')
                elif 'LIMB_HEAD' in tB.name:
                    fw('jh ')
                else:
                    fw('j ')
                fw('%.6f %.6f %.6f' % tB.tail_local[:])
                fw(' %i\n' % currentIndex)
                tCount = writeBoneChildren(tIndex, tB)
                tIndex = (tIndex + tCount + 1)
                childCount = childCount + tCount
                del tCount
            del tIndex
            return (len(currentBone.children) + childCount)
        else:
            return 0
            
    # Write Header
    fw('# Blender v%s MMD File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
    fw('# www.blender.org\n')
    fw('# MMD File for use in the Morchonic Engine\n')
    fw('# MMD is a simplified version of OBJ, with added support for Bones\n')

    # Initialize totals, these are updated each object
    totverts = totuvco = totno = 1

    face_vert_index = 1

    copy_set = set()

    # Get all meshes
    for ob_main in objects:

        # ignore dupli children
        if ob_main.parent and ob_main.parent.dupli_type in {'VERTS', 'FACES'}:
            # XXX
            print(ob_main.name, 'is a dupli child - ignoring')
            continue

        obs = []
        if ob_main.dupli_type != 'NONE':
            # XXX
            print('creating dupli_list on', ob_main.name)
            ob_main.dupli_list_create(scene)

            obs = [(dob.object, dob.matrix) for dob in ob_main.dupli_list]

            # XXX debug print
            print(ob_main.name, 'has', len(obs), 'dupli children')
        else:
            obs = [(ob_main, ob_main.matrix_world)]

        for ob, ob_mat in obs:
            uv_unique_count = no_unique_count = 0
            
            try:
                me = ob.to_mesh(scene, False, 'PREVIEW', calc_tessface=False)
            except RuntimeError:
                me = None

            if me is None:
                continue

            me.transform(EXPORT_GLOBAL_MATRIX * ob_mat)

            if EXPORT_TRI:
                # _must_ do this first since it re-allocs arrays
                mesh_triangulate(me)

            if EXPORT_UV:
                faceuv = len(me.uv_textures) > 0
                if faceuv:
                    uv_texture = me.uv_textures.active.data[:]
                    uv_layer = me.uv_layers.active.data[:]
            else:
                faceuv = False

            me_verts = me.vertices[:]

            # Make our own list so it can be sorted to reduce context switching
            face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]
            # faces = [ f for f in me.tessfaces ]

            if not (len(face_index_pairs) + len(me.vertices)):  # Make sure there is somthing to write

                # clean up
                bpy.data.meshes.remove(me)

                continue  # dont bother with this mesh.

            if EXPORT_NORMALS and face_index_pairs:
                me.calc_normals_split()
                # No need to call me.free_normals_split later, as this mesh is deleted anyway!
                loops = me.loops
            else:
                loops = []


            # -|Sort by Material, then images|- *MMD doesn't use materials
            # so we dont over context switch in the obj file.
            if EXPORT_KEEP_VERT_ORDER:
                pass
            else:
                sort_func = lambda a: a[0].use_smooth
                face_index_pairs.sort(key=sort_func)
                del sort_func
                
            # Bone
            if EXPORT_BONES:
                armat = ob.find_armature().data
                armat.transform(EXPORT_GLOBAL_MATRIX * ob_mat)
                me_bones = armat.bones[:]
                bIndex = 1
                for b in me_bones:
                    if (b.parent == None):
                        fw('j %.6f %.6f %.6f 0\n' % b.head_local[:])
                        if 'LIMB_LEG' in b.name:
                            fw('jl ')
                        elif 'LIMB_ARM' in b.name:
                            fw('ja ')
                        elif 'LIMB_HEAD' in b.name:
                            fw('jh ')
                        else:
                            fw('j ')
                        fw('%.6f %.6f %.6f' % b.tail_local[:])
                        fw(' %i\n' % bIndex)
                        bIndex = (bIndex + writeBoneChildren(bIndex+1, b) + 2)
                del bIndex

            # Vert
            for v in me_verts:
                if EXPORT_BVERTS:
                    fw('vb %.6f %.6f %.6f' % v.co[:])
                    fw('0 \n')
                else:
                    fw('v %.6f %.6f %.6f\n' % v.co[:])

            # UV
            if faceuv:
                # in case removing some of these dont get defined.
                uv = f_index = uv_index = uv_key = uv_val = uv_ls = None

                uv_face_mapping = [None] * len(face_index_pairs)

                uv_dict = {}
                uv_get = uv_dict.get
                for f, f_index in face_index_pairs:
                    uv_ls = uv_face_mapping[f_index] = []
                    for uv_index, l_index in enumerate(f.loop_indices):
                        uv = uv_layer[l_index].uv
                        uv_key = veckey2d(uv)
                        uv_val = uv_get(uv_key)
                        if uv_val is None:
                            uv_val = uv_dict[uv_key] = uv_unique_count
                            fw('vt %.6f %.6f\n' % uv[:])
                            uv_unique_count += 1
                        uv_ls.append(uv_val)

                del uv_dict, uv, f_index, uv_index, uv_ls, uv_get, uv_key, uv_val
                # Only need uv_unique_count and uv_face_mapping

            # NORMAL, Smooth/Non smoothed.
            if EXPORT_NORMALS:
                no_key = no_val = None
                normals_to_idx = {}
                no_get = normals_to_idx.get
                loops_to_normals = [0] * len(loops)
                for f, f_index in face_index_pairs:
                    for l_idx in f.loop_indices:
                        no_key = veckey3d(loops[l_idx].normal)
                        no_val = no_get(no_key)
                        if no_val is None:
                            no_val = normals_to_idx[no_key] = no_unique_count
                            fw('vn %.6f %.6f %.6f\n' % no_key)
                            no_unique_count += 1
                        loops_to_normals[l_idx] = no_val
                del normals_to_idx, no_get, no_key, no_val
            else:
                loops_to_normals = []

            if not faceuv:
                f_image = None
                
                
            for f, f_index in face_index_pairs:
            
                f_v = [(vi, me_verts[v_idx], l_idx) for vi, (v_idx, l_idx) in enumerate(zip(f.vertices, f.loop_indices))]
                
                fw('f')
                if faceuv:
                    if EXPORT_NORMALS:
                        for vi, v, li in f_v:
                            fw(" %d/%d/%d" %
                                       (totverts + v.index,
                                        totuvco + uv_face_mapping[f_index][vi],
                                        totno + loops_to_normals[li],
                                        ))  # vert, uv, normal
                    else:  # No Normals
                        for vi, v, li in f_v:
                            fw(" %d/%d" % (
                                       totverts + v.index,
                                       totuvco + uv_face_mapping[f_index][vi],
                                       ))  # vert, uv

                    face_vert_index += len(f_v)

                else:  # No UV's
                    if EXPORT_NORMALS:
                        for vi, v, li in f_v:
                            fw(" %d//%d" % (totverts + v.index, totno + loops_to_normals[li]))
                    else:  # No Normals
                        for vi, v, li in f_v:
                            fw(" %d" % (totverts + v.index))

                fw('\n')


            # Make the indices global rather then per mesh
            totverts += len(me_verts)
            totuvco += uv_unique_count
            totno += no_unique_count

            # clean up
            bpy.data.meshes.remove(me)

        if ob_main.dupli_type != 'NONE':
            ob_main.dupli_list_clear()

    file.close()

    # copy all collected files.
    bpy_extras.io_utils.path_reference_copy(copy_set)

    print("OBJ Export time: %.2f" % (time.time() - time1))


def _write(context, filepath,
              EXPORT_TRI,  # ok
              EXPORT_NORMALS,  # not yet
              EXPORT_UV,  # ok
              EXPORT_BONES,  # new for MMD
              EXPORT_BVERTS,  # new for MMD
              EXPORT_KEEP_VERT_ORDER,
              EXPORT_SEL_ONLY,  # ok
              EXPORT_GLOBAL_MATRIX,
              EXPORT_PATH_MODE,
              ):  # Not used

    base_name, ext = os.path.splitext(filepath)
    context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension

    scene = context.scene

    # Exit edit mode before exporting, so current object states are exported properly.
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    orig_frame = scene.frame_current

    if EXPORT_SEL_ONLY:
        objects = context.selected_objects
    else:
        objects = scene.objects

    full_path = ''.join(context_name)

    # erm... bit of a problem here, this can overwrite files when exporting frames. not too bad.
    # EXPORT THE FILE.
    write_file(full_path, objects, scene,
               EXPORT_TRI,
               EXPORT_NORMALS,
               EXPORT_UV,
               EXPORT_BONES,
               EXPORT_BVERTS,
               EXPORT_KEEP_VERT_ORDER,
               EXPORT_GLOBAL_MATRIX,
               EXPORT_PATH_MODE,
               )

    scene.frame_set(orig_frame, 0.0)

    # Restore old active scene.
#   orig_scene.makeCurrent()
#   Window.WaitCursor(0)


"""
Currently the exporter lacks these features:
* multiple scene export (only active scene is written)
* particles
"""


def save(operator, context, filepath="",
         use_triangles=True,
         use_normals=True,
         use_uvs=True,
         use_bones=False,
         use_bonedvertices=False,
         keep_vertex_order=False,
         use_selection=False,
         global_matrix=None,
         path_mode='AUTO'
         ):

    _write(context, filepath,
           EXPORT_TRI=use_triangles,
           EXPORT_NORMALS=use_normals,
           EXPORT_UV=use_uvs,
           EXPORT_BONES=use_bones,
           EXPORT_BVERTS=use_bonedvertices,
           EXPORT_KEEP_VERT_ORDER=keep_vertex_order,
           EXPORT_SEL_ONLY=use_selection,
           EXPORT_GLOBAL_MATRIX=global_matrix,
           EXPORT_PATH_MODE=path_mode,
           )

    return {'FINISHED'}
